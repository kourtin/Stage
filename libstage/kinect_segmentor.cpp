#include "kinect_segmentor.h"

#include "kinect.h"
#include "objet_store.h"
#include <ctmf.h>
#include <cvblob.h>
#include <iostream>
#include <algorithm>
#include <vector>

float raw_depth_to_meters(int raw_depth)
{
  if (raw_depth < 2047)
  {
   return 1.0 / (raw_depth * -0.0030711016 + 3.3309495161);
  }
  return 0;
}

cv::Vec2f match_depth_point(cv::Mat& depth_image, unsigned int x_d, unsigned int y_d) {
	cv::Mat R = cv::Mat::zeros(3, 3, CV_64F);
	R.at<double>(0,0) = 9.9984628826577793e-01;
	R.at<double>(0,1) = 1.2635359098409581e-03;
	R.at<double>(0,2) = -1.7487233004436643e-02;
	R.at<double>(1,0) = -1.4779096108364480e-03;
	R.at<double>(1,1) = 9.9992385683542895e-01;
	R.at<double>(1,2) = -1.2251380107679535e-02;
	R.at<double>(2,0) = 1.7470421412464927e-02;
	R.at<double>(2,1) = 1.2275341476520762e-02;
	R.at<double>(2,2) = 9.9977202419716948e-01;
	cv::Mat T = cv::Mat::zeros(3, 1, CV_64F);
	T.at<double>(0,0) = 1.9985242312092553e-02;
	T.at<double>(1,0) = -7.4423738761617583e-04;
	T.at<double>(2,0) = -1.0916736334336222e-02;
	
	
	static const float fx_rgb = 5.0747843343676203e+02;
	static const float fy_rgb = 5.0710971347789956e+02;
	static const float cx_rgb = 3.3453347206758190e+02;
	static const float cy_rgb = 2.2639738341744041e+02;
	static const float k1_rgb = 1.7710499257899193e-01;
	static const float k2_rgb = -8.3152290437826637e-01;
	static const float p1_rgb = -2.3601182007631782e-02;
	static const float p2_rgb = 1.3186630055783820e-02;
	static const float k3_rgb = 1.0365227884736543e+00;
	static const float fx_d = 5.9421434211923247e+02;
	static const float fy_d = 5.9104053696870778e+02;
	static const float cx_d = 3.3930780975300314e+02;
	static const float cy_d = 2.4273913761751615e+02;
	static const float k1_d = -2.6386489753128833e-01;
	static const float k2_d = 9.9966832163729757e-01;
	static const float p1_d = -7.6275862143610667e-04;
	static const float p2_d = 5.0350940090814270e-03;
	static const float k3_d = -1.3053628089976321e+00;
	
	cv::Mat p3D = cv::Mat::zeros(3, 1, CV_64F);
	float depth = 1;//raw_depth_to_meters(depth_image.at<unsigned char>(y_d, x_d));
	p3D.at<float>(0,0) = (x_d - cx_d) * depth * 1.0 / fx_d;
	p3D.at<float>(1,0) = (y_d - cy_d) * depth * 1.0 / fy_d;
	p3D.at<float>(2,0) = depth;
	
	cv::Mat transforme = R * p3D + T;
	cv::Vec2f retour;
	retour[0] = (transforme.at<float>(0,0) * fx_rgb / transforme.at<float>(2,0) + cx_rgb);
	retour[1] = (transforme.at<float>(1,0) * fy_rgb / transforme.at<float>(2,0) + cy_rgb);
	
	return retour;
}

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
	typedef std::vector<double> histogramme;
	typedef histogramme::iterator iterator;
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
	downscale_kde_(48), downscale_(24), 
	depth_map_(cv::Size(640, 480), CV_16UC1), 
	image_depth_(cv::Size(640, 480), CV_8UC1), 
	image_depth2_(cv::Size(640, 480), CV_8UC1),
	petite_image_(cv::Size(640 / downscale_, 480 / downscale_), CV_8UC1), 
	image_tmp_(cv::Size(640 / downscale_kde_, 480 / downscale_kde_), CV_8UC1), 
	image_tmp_ipl_(petite_image_),
	label_img_(cvCreateImage(cvGetSize(&image_tmp_ipl_), IPL_DEPTH_LABEL, 1)), 
	kinect_(k), median_size_(3), kernel_bandwidth_(5), segments_(12) {
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
		cv::medianBlur(image_depth_, image_depth_, median_size_);
		// Marche pas... ?
		// ctmf(image_depth_.data, image_depth2_.data, image_depth_.cols, image_depth_.rows, 1, 1, median_size_, 1, 2048 * 1024);
		// Création d'une miniature
		miniaturiser(image_depth_, image_tmp_, downscale_kde_);
		miniaturiser(image_depth_, petite_image_, downscale_);
		// Estimation de densité
		estimateur_noyau_8bit kde(kernel_bandwidth_);
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
		int nb = 0;
		segmenteur::iterator sit = seg.begin();
		unsigned char borne_inf = *sit++, borne_sup;
		int segg = 0;
		for(segment_liste::iterator it = segments_.begin(); it != segments_.end() && sit != seg.end(); ++it) {
			cv::Mat& mon_segment = *(*it);
			borne_sup = *sit++;
			std::fill(mon_segment.data, mon_segment.data + mon_segment.rows * mon_segment.cols, 0);
			std::transform(petite_image_.data, petite_image_.data + petite_image_.rows * petite_image_.cols, mon_segment.data, remplisseur(borne_inf, borne_sup));
			cv::medianBlur(mon_segment, mon_segment, 3);
			typedef std::vector<std::vector<cv::Point> > contour_vector;
			contour_vector contours;
			std::vector<cv::Vec4i> hierarchie;
			cv::findContours(mon_segment, contours, hierarchie, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			for(contour_vector::iterator it2 = contours.begin(); it2 != contours.end(); ++it2) {
				std::vector<cv::Point>& mon_contour = *it2;
				cv::Rect rect = cv::boundingRect(cv::Mat(mon_contour));
				blobbos blb;
				// Transforme les points dans les coordonnées de l'image couleur
				cv::Vec2f inf_gauche = match_depth_point(image_depth_, rect.x * downscale_, rect.y * downscale_);
				cv::Vec2f sup_droit = match_depth_point(image_depth_, (rect.x + rect.width) * downscale_, (rect.y + rect.height) * downscale_);
				
				std::cout << "infgauche: "<< inf_gauche[0] << ", " << inf_gauche[1] << std::endl;
				std::cout << "supdroit : "<< sup_droit[0] << ", " << sup_droit[1] << std::endl;
				
				blb.rect = floatrect(inf_gauche[0] * 1.0 / image_depth_.cols, inf_gauche[1] * 1.0 / image_depth_.rows, sup_droit[0] * 1.0 / image_depth_.cols, sup_droit[1] * 1.0 / image_depth_.rows);
				blb.id = nb++;
				blb.rect.id = nb - 1;
				blobs_.push_back(blb);
				objet* o = store->get_in(blb.rect);
				if(o && o->id() > 0 && o->present() && std::find(deja_associes.begin(), deja_associes.end(), o->id()) == deja_associes.end()) {
					// if(image_depth_.at<unsigned char>(o->y() * image_depth_.rows, o->x() * image_depth_.cols) != 255) {
						deja_associes.push_back(o->id());
						o->rect(blb.rect);
					// } else {
						// o->rect(floatrect());
					// }
				}
			}
			// cvb::CvBlobs blobs;
			// 			IplImage seg = mon_segment;
			// 			cvLabel(&seg, label_img_, blobs);
			// 			// std::cout << "Segment " << segg << ", " << blobs.size() << " blobs." << std::endl;
			// 			for (cvb::CvBlobs::const_iterator itb=blobs.begin(); itb!=blobs.end(); ++itb) {
			// 				// Traiter le blob
			// 				cvb::CvBlob& b = *itb->second;
			// 				blobbos blb; 
			// 				blb.rect = floatrect(b.minx * 1.0 / mon_segment.cols, b.miny * 1.0 / mon_segment.rows, b.maxx * 1.0 / mon_segment.cols, b.maxy * 1.0 / mon_segment.rows);
			// 				blb.id = nb++;
			// 				blb.rect.id = nb - 1;
			// 				blobs_.push_back(blb);
			// 				objet* o = store->get_in(blb.rect);
			// 				if(b.area > 1														// aire > 2
			// 					&& o && o->id() > 0 											// id valide
			// 					&& o->present() 												// présent
			// 					&& cvb::cvGetLabel(label_img_, o->x() * mon_segment.cols, 		// sur le blob
			// 						o->y() * mon_segment.rows) == b.label
			// 					&& std::find(deja_associes.begin(), 							// pas déjà affecté
			// 						deja_associes.end(), o->id()) == deja_associes.end()) {
			// 					// Associons le blob à un objet
			// 					// if(image_depth2_.at<unsigned char>(o->y() * image_depth2_.rows, o->x() * image_depth2_.cols) != 255) {
			// 						o->z(image_depth2_.at<unsigned char>(o->y() * image_depth2_.rows, o->x() * image_depth2_.cols) * 1.0 / 255);
			// 						o->rect(blb.rect);
			// 					// }
			// 					// else {
			// 						// o->z(-1);
			// 						// o->rect(floatrect());
			// 					// }
			// 					// std::cout << "Found ID=" << o->id() << " with blob " << blb.id << std::endl;
			// 					deja_associes.push_back(o->id());
			// 				}
			// 			}
			borne_inf = borne_sup;
			++segg;
		}
	}
}