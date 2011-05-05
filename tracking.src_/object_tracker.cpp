#include "object_tracker.h"
#include "estimation_noyau.h"
#include <iostream>

using namespace cvb;

object_tracker::object_tracker(cv::Size s) : blobby_(s, CV_8UC3), taille_median_(41), increment_(24), bandwidth_(3), kde_(256), h_(256), m_8bit_(s.height, s.width, CV_8UC1), m_tmp1_(s, CV_8UC1) {
	for(size_t i=0; i<max_segments; ++i)
		segments_nb_.push_back(cv::Mat(m_8bit_.rows / increment_, m_8bit_.cols / increment_, CV_8UC1));
}
object_tracker::~object_tracker() {}

void object_tracker::increment(size_t i) {
	increment_ = std::max((size_t)1, i);
	// m_8bit_ = cv::Mat(m_8bit_.rows / increment_, m_8bit_.cols / increment_, CV_8UC1);
}

void object_tracker::taille_median(size_t t) {
	if(t % 2 == 0)
		taille_median_ = t + 1;
	else
		taille_median_ = t;
	
	taille_median_ = std::min(std::max(static_cast<int>(taille_median_), 1), 201);
}

void object_tracker::process(cv::Mat& m) {
	// Conversion en 8 bits
	m.convertTo(m_8bit_, CV_8UC1, 255.0 / 2048.0);

	// std::cout << "mini: " << mini << " ;; " << "maxi: " << maxi << std::endl;
	// Filtre médian
	// m_8bit_.copyTo(m_tmp1_);
	cv::medianBlur(m_8bit_, m_tmp1_, taille_median_);
	// cv::bilateralFilter(m_8bit_, m_tmp1_, taille_median_, 21, 21);
	// cv::blur(m_8bit_, m_tmp1_, cv::Size(taille_median_, taille_median_));
	//
	// Construction d'une image de petite taille
	unsigned char mini = 255;
	unsigned char maxi = 0;
	cv::Mat petite_image(m.rows / increment_, m.cols / increment_, CV_8UC1);
	// cv::Mat petite_image2(m.rows / increment_, m.cols / increment_, CV_8UC1);
	int y = 0;
	for(int i=0; i<m.rows; i += increment_) {
		int x = 0;
		for(int j=0; j<m.cols; j += increment_) {
			unsigned char v = m_tmp1_.at<unsigned char>(i,j);
			petite_image.at<unsigned char>(y,x) = v;
			if(v > maxi && v < 255)
				maxi = v;
			if(v < mini)
				mini = v;
			++x;
		}
		++y;
	}
	// std::cout << "max=" << (int)maxi << "; min=" << (int)mini << std::endl;
	
	// Adapté librement de E. Parvizi et Q. J. Wu, "Multiple Object Tracking Based on Adaptive Depth Segmentation", p. 273-277, mai. 2008.
	
	// 1) Filtre médian
	/*
		Les performances du filtre médian d'OpenCV font passer le framerate de 60fps à 20fps.
	*/
	// cv::medianBlur(m2, m_tmp1_, taille_median_);
	// petite_image.copyTo(m_8bit_);
	
	// // 2) Construction de l'histogramme
	// // 	.1) Vidage
	// // std::vector<float> h_avant(h_);
	// for(size_t i=0; i<h_.size(); ++i)
	// 	h_[i] = 0;
	// 
	// // 	.2) Remplissage
	// for(int i=0; i<petite_image.rows; ++i) {
	// 	for(int j=0; j<petite_image.cols; ++j) {
	// 		size_t v = petite_image.at<unsigned char>(i, j);
	// 		// v = v * (maxi - mini) + mini;
	// 		// if(v == 0 || v == h_.size()-1) continue;
	// 		int m = std::min(v, h_.size()-1);
	// 		size_t valeur = std::max(m, 0);
	// 		++h_[valeur];
	// 	}
	// }
	// // 	.3) Normalisation
	// for(size_t i=0; i<h_.size(); ++i) {
	// 	h_[i] /= petite_image.cols * petite_image.rows;
	// }
	
	// 3) Estimation par noyau gaussien
	//	.1) Récupération des valeurs
	std::vector<float> valeurs;
	for(int i=0; i<petite_image.rows; ++i) {
		for(int j=0; j<petite_image.cols; ++j) {
			unsigned char v = petite_image.at<unsigned char>(i, j);
			if(v == 255) { continue; }
			v = (v - mini) * 1.0 / (maxi - mini) * 255;
			// v = v * (maxi - mini) * 1.0/255.0 + mini;
			int m = std::min((long unsigned int)v, h_.size()-1);
			size_t valeur = std::max(m, 0);
			valeurs.push_back(valeur);
		}
	}
	
	//	.2) Estimation
	estimation_noyau esti(valeurs, bandwidth_ /** (255.0/(maxi-mini)*1.0)*/);
	for(size_t i=0; i<255; ++i) {
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
			// unsigned char v = ((i-1) - mini) * 1.0 / (maxi - mini) * 255;
			if(e != croissance) { // nouveau minimum local
				segments_.push_back(i-1);
				// std::cout << (int)(i-1) << " = " << (int)v << "; ";
			}
			e = croissance;
		} else
			e = stagnation;
	}
	segments_.push_back(255);
	// std::cout << std::endl;
	
	// std::cout << segments_.size() << std::endl;
	
	// Itération sur chaque pixel pour l'assigner à son segment
	// std::vector<float> occupations(segments_.size()-1);
	for(size_t n=1; n<segments_.size(); ++n) {
		// occupations[n-1] = 0;
		for(int i=0; i<petite_image.rows; ++i) {
			for(int j=0; j<petite_image.cols; ++j) {
			
				// std::cout << "::::" << n << ";;;; "<<x<<":"<<y << std::endl;
				unsigned char v = petite_image.at<unsigned char>(i, j);
				v = (v - mini) * 1.0 / (maxi - mini) * 255;
				if(petite_image.at<unsigned char>(i, j) != 255 && segments_[n-1] <= v && v < segments_[n])
					segments_nb_[n-1].at<unsigned char>(i,j) = 255;
				// else if(v < segments_[n])
					// segments_nb_[n-1].at<unsigned char>(i,j) = 255;
				else
					segments_nb_[n-1].at<unsigned char>(i,j) = 0;
					
				// if(segments_nb_[n-1].at<unsigned char>(i,j) != 0)
					// ++occupations[n-1];
			}
		}
		// cv::Mat itmp(segments_nb_[n-1].rows, segments_nb_[n-1].cols, CV_8UC1);
		// cv::medianBlur(segments_nb_[n-1], itmp, 19);
		// itmp.copyTo(segments_nb_[n-1]);
	}
	
	// Détection de blob sur chaque segment
	std::vector<blobby> blobs_avant(blobs_);
	int i = 0;
	blobs_.clear();
	int nb = 0;
	for(size_t j=0; j<segments_nb_.size() - 1; ++j) {
		// if(occupations[j] * 1.0 / (petite_image.rows * petite_image.cols) > 0.6)
			// break;
		cv::Mat m3(segments_nb_[j].rows, segments_nb_[j].cols, CV_8UC1);
		cv::medianBlur(segments_nb_[j], m3, 3);
		IplImage m_ipl = m3;
		CvBlobs blobs;
		IplImage *labelImg = cvCreateImage(cvGetSize(&m_ipl), IPL_DEPTH_LABEL, 1);
		cvLabel(&m_ipl, labelImg, blobs);
		for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it) {
			++nb;
			blobby bob(i, nb, *(it->second), labelImg, petite_image);
			if(bob.valide()) {
				blobs_.push_back(bob);
			}
		}
		cvReleaseImage(&labelImg);
		++i;
	}
	
	// Association des blobs
	matcher_(blobs_);
	
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