#include "object_tracker.h"
#include "estimation_noyau.h"
#include <iostream>

using namespace cvb;

object_tracker::object_tracker(cv::Size s) : m_8bit_(s, CV_8UC1), m_tmp1_(s, CV_8UC1), blobby_(s, CV_8UC3), taille_median_(49), increment_(32), bandwidth_(3), kde_(256), h_(256) {
	increment(increment_);
}
object_tracker::~object_tracker() {}

void object_tracker::increment(size_t i) {
	increment_ = std::max((size_t)1, i);
	// (ré)Initialisation des segments noir et blanc
	segments_nb_.clear();
	for(size_t i=0; i<max_segments; ++i)
		segments_nb_.push_back(cv::Mat(m_8bit_.rows / increment_, m_8bit_.cols / increment_, CV_8UC1));
}

void object_tracker::taille_median(size_t t) {
	if(t % 2 == 0)
		taille_median_ = t + 1;
	else
		taille_median_ = t;
	
	taille_median_ = std::min(std::max(static_cast<int>(taille_median_), 1), 201);
}

void object_tracker::process(cv::Mat& m) {
	// Construction d'une image de petite taille
	cv::Mat petite_image(m.rows * 1.0 / increment_, m.cols * 1.0 / increment_, CV_8UC1);
	int y = 0;
	for(int i=0; i<m.rows; i += increment_) {
		int x = 0;
		for(int j=0; j<m.cols; j += increment_) {
			// Va chercher la médiane
			std::vector<unsigned short> vals;
			for(size_t k=i; k<i+increment_; ++k) {
				for(size_t l=j; l<j+increment_; ++j) {
					vals.push_back(m.at<unsigned short>(k,l));
				}
			}
			std::sort(vals.begin(), vals.end());
			petite_image.at<unsigned char>(y,x) = std::max(static_cast<unsigned char>(0), std::min(static_cast<unsigned char>(255), static_cast<unsigned char>(vals[vals.size()/2] * 255.0 / 2048.0)));
			++x;
		}
		++y;
	}
	// cv::flip(m, m, 1);
	// Retourne l'image
	cv::flip(petite_image, petite_image, 1);
	// cv::Mat m2(m.size(), CV_8UC1);
	// m.convertTo(m2, CV_8UC1, 255.0/2048 /* 1.0/4.0 de http://code.google.com/p/libfreenect-kernel/source/browse/simple_opencv/freenectopencv.cpp */);
	// m.convertTo(m_tmp1_, CV_8UC1, 255.0/KINECT_MAX_DEPTH);
	// m2.copyTo(m_tmp1_);
	
	// Adapté librement de E. Parvizi et Q. J. Wu, "Multiple Object Tracking Based on Adaptive Depth Segmentation", p. 273-277, mai. 2008.
	
	// 1) Filtre médian
	/*
		Les performances du filtre médian d'OpenCV font passer le framerate de 60fps à 20fps.
	*/
	// cv::medianBlur(m2, m_tmp1_, taille_median_);
	// m_tmp1_.copyTo(m_8bit_);
	petite_image.copyTo(m_8bit_);
	
	// 2) Construction de l'histogramme
	// 	.1) Vidage
	std::vector<float> h_avant(h_);
	for(size_t i=0; i<h_.size(); ++i)
		h_[i] = 0;
	
	// 	.2) Remplissage
	for(int i=0; i<petite_image.rows; ++i) {
		for(int j=0; j<petite_image.cols; ++j) {
			size_t v = petite_image.at<unsigned char>(i, j);
			if(v == 0 || v == h_.size()-1) continue;
			int m = std::min(v, h_.size()-1);
			size_t valeur = std::max(m, 0);
			++h_[valeur];
		}
	}
	// 	.3) Normalisation
	for(size_t i=0; i<h_.size(); ++i) {
		h_[i] /= petite_image.cols * petite_image.rows;
	}
	
	// 3) Estimation par noyau gaussien
	// TODO: Essayer pondération par rapport au taux de variation (pour virer les trucs fixes)
	//	.1) Récupération des valeurs
	std::vector<float> valeurs;
	for(int i=0; i<petite_image.rows; ++i) {
		for(int j=0; j<petite_image.cols; ++j) {
			unsigned char v = petite_image.at<unsigned char>(i, j);
			if(v == 0 || v == h_.size()-1) continue;
			int m = std::min((long unsigned int)v, h_.size()-1);
			size_t valeur = std::max(m, 0);
			// if(valeur > 200)
				// std::cout << "v: " << 1.0 / std::abs(h_[valeur] - h_avant[valeur]) << std::endl;
			// valeur *= std::min(1.0 * std::abs(h_[valeur] - h_avant[valeur]), 1.0);
			valeurs.push_back(valeur);
		}
	}
	
	//	.2) Estimation
	estimation_noyau esti(valeurs, bandwidth_);
	for(size_t i=0; i<kde_.size(); ++i) {
		kde_[i] = esti(i);
	}
	// 4) Extraction des segments
	segments_.clear();
	segments_.push_back(0);
	enum etat { croissance, decroissance, stagnation };
	etat e = stagnation;
	for(size_t i=1; i<kde_.size() && segments_.size() < max_segments - 1; ++i) {
		if(kde_[i] < kde_[i-1]) {
			e = decroissance;
		} else if(kde_[i] > kde_[i-1]) {
			if(e == decroissance) // nouveau minimum local
				segments_.push_back(i);
			e = croissance;
		}
	}
	segments_.push_back(255);
	
	// std::cout << segments_.size() << std::endl;
	
	// Itération sur chaque pixel pour l'assigner à son segment
	for(int i=0; i<petite_image.rows; ++i) {
		for(int j=0; j<petite_image.cols; ++j) {
			for(size_t n=1; n<segments_.size(); ++n) {
				// std::cout << "::::" << n << ";;;; "<<x<<":"<<y << std::endl;
				unsigned char v = petite_image.at<unsigned char>(i, j);
				if(v < 200 && segments_[n-1] <= v && v < segments_[n])
					segments_nb_[n-1].at<unsigned char>(i,j) = 255;
				// else if(v < segments_[n])
					// segments_nb[n-1].at<unsigned char>(y,x) = 255;
				else
					segments_nb_[n-1].at<unsigned char>(i,j) = 0;
			}
		}
	}
	
	// Détection de blob sur chaque segment
	std::vector<blobby> blobs_avant(blobs_);
	int i = 0;
	blobs_.clear();
	int nb = 0;
	for(size_t j=0; j<segments_.size() - 1; ++j) {
		cv::Mat m3 = segments_nb_[j];
		IplImage m_ipl = m3;
		CvBlobs blobs;
		IplImage *labelImg = cvCreateImage(cvGetSize(&m_ipl), IPL_DEPTH_LABEL, 1);
		cvLabel(&m_ipl, labelImg, blobs);
		for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it) {
			++nb;
			blobby bob(i, nb, *(it->second), labelImg, petite_image, 1);
			if(bob.valide())
				blobs_.push_back(bob);
		}
		++i;
	}
	
	// Association des objets
	for(std::vector<objet>::iterator it = objets_.begin(); it != objets_.end(); ++it) {
		it->actualiser(blobs_);
	}
	// Association avec la frame d'avant
	// if(blobs_avant.size() > 0) {
	// 	for(std::vector<blobby>::iterator it = blobs_.begin(); it != blobs_.end(); ++it) {
	// 		blobby& b1 = *it;
	// 		blobby* blob_min = 0;
	// 		float min = std::numeric_limits<float>::max();
	// 		for(std::vector<blobby>::iterator it2 = blobs_avant.begin(); it2 != blobs_avant.end(); ++it2) {
	// 			blobby& b2 = *it2;
	// 			float d = b2.distance(b1);
	// 			if(d < min) {
	// 				min = d;
	// 				blob_min = &b2;
	// 			}
	// 		}
	// 		if(blob_min != 0)
	// 			b1.label(blob_min->label());
	// 	}
	// }
}