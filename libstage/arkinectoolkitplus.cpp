#include "arkinectoolkitplus.h"
#include "kinect.h"
#include "objet_store.h"


// void miniaturiser2(cv::Mat original, cv::Mat reduit, unsigned int facteur) {
// 	int y = 0;
// 	for(int i = 0; i < original.rows; i += facteur) {
// 		int x = 0;
// 		for(int j = 0; j < original.cols; j += facteur) {
// 			unsigned char v = original.at<unsigned char>(i,j);
// 			reduit.at<unsigned char>(y,x) = v;
// 			++x;
// 		}
// 		++y;
// 	}
// }
#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>
arkinectoolkitplus::arkinectoolkitplus(kinect* k) : image_couleur_(cv::Size(640, 480), CV_8UC3), image_nb_(cv::Size(640, 480), CV_8UC1), tracker_(new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6>(640,480)), kinect_(k) {
	tracker_->init(/*"libartoolkitplus/sample/simple/data/no_distortion.cal"*/"libartoolkitplus/kinect_burrus_rgbdemo.cal", 1.0f, 1000.0f);
	tracker_->setBorderWidth(0.125f);
	tracker_->activateAutoThreshold(true);//setThreshold(150);
	tracker_->setUndistortionMode(ARToolKitPlus::UNDIST_LUT);
	tracker_->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);
	tracker_->setMarkerMode(ARToolKitPlus::MARKER_ID_BCH);
	tracker_->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_LUM);
	tracker_->setImageProcessingMode(ARToolKitPlus::IMAGE_HALF_RES);
	// tracker_->setUseDetectLite(false);
}

arkinectoolkitplus::~arkinectoolkitplus() {
	
}
#include <iostream>
void arkinectoolkitplus::operator()(objet_store* store) {
	if(!kinect_) return;
	int threshold = 100;
	if(kinect_->img_to(image_couleur_)) {
		// cv::cvtColor(image_couleur_, image_nb_, CV_RGB2GRAY);
		unsigned char* src = image_couleur_.data;
		unsigned char* dest = image_nb_.data;
		for(int i=0; i<640*480; ++i) {
			float r = *src++ * 1.0 / 255;
			float g = *src++ * 1.0 / 255;
			float b = *src++ * 1.0 / 255;
			float gr = 0.2989 * r + 0.5870 * g + 0.1140 * b;
			*dest++ = static_cast<unsigned char>(gr / 1.0 * 255);
		}
		
		// Processe
		ARToolKitPlus::ARMarkerInfo* infos;
		int nb_marqueurs;
		int marker_id;
		// std::cout << "coucou" << std::endl;
		if((marker_id = tracker_->arDetectMarker(image_nb_.data, threshold, &infos, &nb_marqueurs)) < 0)
			return;
		// std::cout << "trouved marqueur " << marker_id << " avec confiance " << (float)tracker_->getConfidence() << std::endl;
		// if(nb_marqueurs > 0)
			// std::cout << nb_marqueurs << " marqueurs." << std::endl;
		for(int i=0; i<nb_marqueurs; ++i) {
							// std::cout << "#" << infos[i].id << ": " << infos[i].pos[0] << "," << infos[i].pos[1] << std::endl;
							objet& o = store->get_or_create(infos[i].id);
							o.x(infos[i].pos[0] * 1.0 / 640);
							o.y(infos[i].pos[1] * 1.0/ 480);
							o.present(true);
						}
	}
}