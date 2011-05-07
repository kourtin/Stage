#include "kinect_segmentor.h"

#include "kinect.h"
#include "objet_store.h"
#include <figtree.h>
#include <ctmf.h>
#include <cvblob.h>
#include <iostream>
#include <algorithm>
#include <vector>

void miniaturiser(cv::Mat original, cv::Mat reduit, unsigned int facteur) {
	int y = 0;
	for(int i = 0; i < original.rows; i += facteur) {
		int x = 0;
		for(int j = 0; j < original.cols; j += facteur) {
			unsigned char v = original.at<unsigned char>(i,j);
			reduit.at<unsigned char>(y,x) = v;
			++x;
		}
		++y;
	}
}

struct estimateur_noyau_8bit {
	typedef std::vector<double>::iterator iterator;
	estimateur_noyau_8bit(double bandwidth) : densite_(255), bandwidth_(bandwidth) {}
	virtual ~estimateur_noyau_8bit() {}
	template <class Itt>
	void operator()(Itt deb, Itt fin) {
		unsigned int nb_valeurs = std::distance(deb, fin);
		double facteur = 1.0 / (bandwidth_ * nb_valeurs);
		for(size_t i = 0; i < densite_.size(); ++i) {
			double acc = 0.0;
			for(Itt it = deb; it != fin; ++it) {
				acc += gauss_kernel(1.0 * (i - (*it)) / (bandwidth_));
			}
			// std::cout << "Densité(" << i << ") = " << (facteur*acc) << std::endl;
			densite_[i] = facteur * acc;
		}
	}
	iterator begin() { return densite_.begin(); }
	iterator end() { return densite_.end(); }
private:
	double gauss_kernel(double u) {
		return 1.0 / sqrt(2 * M_PI) * exp(-1.0 / 2 * u * u);
	}
	std::vector<double> densite_;
	float bandwidth_;
};

struct compteur {
	compteur() : c_(0) {}
	unsigned int operator()() {
		return c_++;
	}
private:
	unsigned int c_;
};

struct estimateur_noyau_figtree {
	typedef std::vector<double>::iterator iterator;
	estimateur_noyau_figtree(double bandwidth, int nb_valeurs = 10000) : densite_(255), target_(255), source_(nb_valeurs), poids_(nb_valeurs), bandwidth_(bandwidth) {
		std::generate(target_.begin(), target_.end(), compteur());
	}
	virtual ~estimateur_noyau_figtree() {}
	template <class Itt>
	void operator()(Itt deb, Itt fin) {
		unsigned int nb_valeurs = std::distance(deb, fin);
		double facteur = 1.0 / (bandwidth_ * nb_valeurs);
		poids_.resize(nb_valeurs);
		source_.resize(nb_valeurs);
		std::copy(deb, fin, source_.begin());
		std::fill(poids_.begin(), poids_.end(), facteur * 1.0 / sqrt(2 * M_PI));
		figtree(1, nb_valeurs, densite_.size(), 1, &(*source_.begin()), sqrt(2) * bandwidth_, &(*poids_.begin()), &(*target_.begin()), 0.01, &(*densite_.begin()));
	}
	iterator begin() { return densite_.begin(); }
	iterator end() { return densite_.end(); }
private:
	std::vector<double> densite_;
	std::vector<double> target_;
	std::vector<double> source_;
	std::vector<double> poids_;
	float bandwidth_;
};

struct segmenteur {
	typedef std::vector<unsigned int> liste_segments;
	typedef liste_segments::iterator iterator;
	segmenteur(unsigned int max) : max_(max) {}
	template <class Itt>
	void operator()(Itt deb, Itt fin) {
		enum etat { croissance, decroissance, stagnation };
		etat e = stagnation;
		float last = -1;
		for(int i = 1; deb != fin && segments_.size() < max_; ++i) {
			double current = *deb++;
			// std::cout << i << ". Current: " << current << "; Last: " << last << std::endl;
			if(current < last)
				e = decroissance;
			else if(current > last) {
				if(e != croissance) {
					segments_.push_back(i-1);
					// std::cout << "!!!!!!!!!!!!!!" << std::endl;
				}
				e = croissance;
			} else
				e = stagnation;
			last = current;
		}
		segments_.push_back(255);
	}
	iterator begin() { return segments_.begin(); }
	iterator end() { return segments_.end(); }
	unsigned int size() { return segments_.size(); }
private:
	liste_segments segments_;
	unsigned int max_;
};

struct remplisseur {
	remplisseur(unsigned char a, unsigned char b) : a_(a), b_(b) {}
	unsigned char operator()(unsigned char v) {
		return (a_ <= v && v < b_) ? 255 : 0;
	}
private:
	unsigned char a_, b_;
};

kinect_segmentor::kinect_segmentor(kinect* k) : 
	downscale_kde_(16), downscale_(16), 
	depth_map_(cv::Size(640, 480), CV_16UC1), 
	image_depth_(cv::Size(640, 480), CV_8UC1), 
	image_depth2_(cv::Size(640, 480), CV_8UC1),
	petite_image_(cv::Size(640 / downscale_, 480 / downscale_), CV_8UC1), 
	image_tmp_(cv::Size(640 / downscale_kde_, 480 / downscale_kde_), CV_8UC1), 
	image_tmp_ipl_(petite_image_),
	label_img_(cvCreateImage(cvGetSize(&image_tmp_ipl_), IPL_DEPTH_LABEL, 1)), 
	kinect_(k), median_size_(39), kernel_bandwidth_(1.0), segments_(30) {
	init_segments();
}

kinect_segmentor::~kinect_segmentor() {
	for(segment_liste::iterator it = segments_.begin(); it != segments_.end(); ++it) {
		cv::Mat* mat = *it;
		if(mat)
			delete mat;
	}
	cvReleaseImage(&label_img_);
}

void kinect_segmentor::init_segments() {
	for(segment_liste::iterator it = segments_.begin(); it != segments_.end(); ++it) {
		*it = new cv::Mat(petite_image_.size(), CV_8UC1);
	}
}

void kinect_segmentor::operator()(objet_store* store) {
	if(!kinect_) return;
	if(kinect_->depth_to(depth_map_)) {
		// Conversion en 8 bits
		depth_map_.convertTo(image_depth_, CV_8UC1, 255.0 / 2048.0);
		// Filtre médian
		// cv::medianBlur(image_depth_, image_depth_, median_size_);
		// Marche pas... ?
		// ctmf(image_depth2_.data, image_depth_.data, image_depth_.rows, image_depth_.cols, 1, 1, median_size_, 1, 2048 * 1024);
		// Met à jour les positions z
		for(objet_store::iterator it = store->begin(); it != store->end(); ++it) {
			objet* o = *it;
			unsigned char valeur_sur_o = image_depth_.at<unsigned char>(o->y_c() * image_depth_.rows, o->x_c() * image_depth_.cols);
			// std::cout << (int)valeur_sur_o << std::endl;
			if(valeur_sur_o != 0)
				o->z((valeur_sur_o - 22) * 1.0 / (43-28));
		}
		return;
		// Création d'une miniature
		miniaturiser(image_depth_, image_tmp_, downscale_kde_);
		miniaturiser(image_depth_, petite_image_, downscale_);
		// Estimation de densité
		estimateur_noyau_figtree kde(kernel_bandwidth_, image_tmp_.rows * image_tmp_.cols);
		kde(image_tmp_.data, image_tmp_.data + image_tmp_.rows * image_tmp_.cols);
		// Segmentation de la densité
		segmenteur seg(segments_.size());
		seg(kde.begin(), kde.end());
		// Préparation des images correspondant à chaque segment
		// Ensuite, pour chaque segment, faire une analyse de composantes connexes (blob extraction)
		// Et pour chaque blob, s'il est à l'emplacement d'un objet déjà présent dans le store, l'associer à cet objet
		// en changeant ses caractéristiques (bounding box)
		blobs_.clear();
		std::vector<int> deja_associes;
		deja_associes.clear();
		int nb = 0;
		segmenteur::iterator sit = seg.begin();
		unsigned char borne_inf = *sit++, borne_sup;
		int segg = 0;
		for(segment_liste::iterator it = segments_.begin(); it != segments_.end() && sit != seg.end(); ++it) {
			cv::Mat& mon_segment = *(*it);
			borne_sup = *sit++;
			std::fill(mon_segment.data, mon_segment.data + mon_segment.rows * mon_segment.cols, 0);
			std::transform(petite_image_.data, petite_image_.data + petite_image_.rows * petite_image_.cols, mon_segment.data, remplisseur(borne_inf, borne_sup));
			// cv::medianBlur(mon_segment, mon_segment, 3);
			typedef std::vector<std::vector<cv::Point> > contour_vector;
			contour_vector contours;
			std::vector<cv::Vec4i> hierarchie;
			cv::findContours(mon_segment, contours, hierarchie, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			for(contour_vector::iterator it2 = contours.begin(); it2 != contours.end(); ++it2) {
				std::vector<cv::Point>& mon_contour = *it2;
				cv::Rect rect = cv::boundingRect(cv::Mat(mon_contour));
				blobbos blb;
				// Transforme les points dans les coordonnées de l'image couleur
				cv::Vec2f inf_gauche(rect.x * downscale_, rect.y * downscale_);// match_depth_point(image_depth_, rect.x * downscale_, rect.y * downscale_);
				cv::Vec2f sup_droit((rect.x + rect.width) * downscale_, (rect.y + rect.height) * downscale_);// = match_depth_point(image_depth_, );
				// std::cout << "infgauche: "<< inf_gauche[0] << ", " << inf_gauche[1] << std::endl;
				// std::cout << "supdroit : "<< sup_droit[0] << ", " << sup_droit[1] << std::endl;
				
				blb.rect = floatrect(inf_gauche[0] * 1.0 / image_depth_.cols, inf_gauche[1] * 1.0 / image_depth_.rows, sup_droit[0] * 1.0 / image_depth_.cols, sup_droit[1] * 1.0 / image_depth_.rows);
				blb.id = nb++;
				blb.rect.id = nb - 1;
				blobs_.push_back(blb);
				objet* o = store->get_in(blb.rect);
				if(o && o->present() && std::find(deja_associes.begin(), deja_associes.end(), o->id()) == deja_associes.end()) {
					// Teste si l'objet tombe bien sur le segment
					unsigned char valeur_sur_o = image_depth_.at<unsigned char>(o->y_c() * image_depth_.rows, o->x_c() * image_depth_.cols);
					if(borne_inf <= valeur_sur_o && valeur_sur_o < borne_sup) {
						// Transforme les points dans le repère d'origine (x, y)
						o->rect(floatrect(blb.rect.x1 - o->x_c(), blb.rect.y1 - o->y_c(), blb.rect.x2 - o->x_c(), blb.rect.y2 - o->y_c()));
						// Associe
						deja_associes.push_back(o->id());
						// Peut couper l'image plus précisément
						
					}
				}
			}
			borne_inf = borne_sup;
			++segg;
		}
	}
}