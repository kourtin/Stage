#include "arkinectoolkitplus.h"
#include "kinect.h"
#include "objet_store.h"

#include <cmath>

void angles_euler(float m[3][4], float& x, float& y, float& z) {
	if(std::abs(m[2][0]) != 1) {
		float th1 = -asin(m[2][0]);
		// float th2 = M_PI - th1;
		float psi1 = atan2(m[2][1] / cos(th1), m[2][2] / cos(th1));
		// float psi2 = atan2(m[2][1] / cos(th2), m[2][2] / cos(th2));
		float phi1 = atan2(m[1][0] / cos(th1), m[0][0] / cos(th1));
		// float phi2 = atan2(m[1][0] / cos(th2), m[0][0] / cos(th2));
		x = psi1;
		y = th1;
		z = phi1;
	} else {
		z = 0;
		if(m[2][0] == -1) {
			y = M_PI / 2;
			x = z + atan2(m[0][1], m[0][2]);
		} else {
			y = -M_PI / 2;
			x = -z + atan2(-m[0][1], -m[0][2]);
		}
	}
}

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
arkinectoolkitplus::arkinectoolkitplus(cv::Mat& img) : image_couleur_(img), image_nb_(cv::Size(640, 480), CV_8UC1), tracker_(new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6>(640,480)) {
	tracker_->init("libartoolkitplus/sample/simple/data/no_distortion.cal"/*"libartoolkitplus/kinect_burrus_rgbdemo.cal"*/, 1.0f, 1000.0f);
	tracker_->setBorderWidth(0.125f);
	tracker_->activateAutoThreshold(true);
	// tracker_->setThreshold(150);
	// tracker_->setNumAutoThresholdRetries(100);
	tracker_->setUndistortionMode(ARToolKitPlus::UNDIST_NONE);
	tracker_->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);
	tracker_->setMarkerMode(ARToolKitPlus::MARKER_ID_BCH);
	tracker_->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_LUM);
	// tracker_->setImageProcessingMode(ARToolKitPlus::IMAGE_HALF_RES);
	// tracker_->setUseDetectLite(false);
}

arkinectoolkitplus::~arkinectoolkitplus() {
	
}
#include <iostream>
void arkinectoolkitplus::operator()(objet_store* store) {
	int threshold = 100;
	cv::cvtColor(image_couleur_, image_nb_, CV_BGR2GRAY);
	// unsigned char* src = image_couleur_.data;
	// 		unsigned char* dest = image_nb_.data;
	// 		for(int i=0; i<640*480; ++i) {
	// 			float r = *src++ * 1.0 / 255;
	// 			float g = *src++ * 1.0 / 255;
	// 			float b = *src++ * 1.0 / 255;
	// 			float gr = 0.2989 * r + 0.5870 * g + 0.1140 * b;
	// 			*dest++ = static_cast<unsigned char>(gr / 1.0 * 255);
	// 		}
	
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
		float patt_w = 6.0;
		float patt_c[2] = { 0.0, 0.0 };
		float patt_tr[3][4];
		// std::cout << "#" << infos[i].id << ": " << infos[i].pos[0] << "," << infos[i].pos[1] << std::endl;
		if(infos[i].id <= 0)
			continue;
		objet& o = store->get_or_create(infos[i].id);
		o.x(infos[i].pos[0] * 1.0 / 640);
		o.y(infos[i].pos[1] * 1.0/ 480);
		o.present(true);
		// Estimation de pose
		tracker_->executeSingleMarkerPoseEstimator(infos + i, patt_c, patt_w, patt_tr);
		// std::cout << "#" << infos[i].id << atan2(patt_tr[][], patt_tr[][]) << std::endl;
		// for(int k=0; k<3; ++k) {
		// 	for(int l=0; l<4; ++l)
		// 		std::cout << patt_tr[k][l] << "\t\t";
		// 	std::cout << std::endl;
		// }
		float x, y, z;
		angles_euler(patt_tr, x, y, z);
		o.r(z);
		o.z(patt_tr[2][3] / 500);
		// std::cout << "#" << infos[i].id << ": x=" << patt_tr[0][3] << ", y=" << patt_tr[1][3] << ", z=" << patt_tr[2][3] << std::endl;
	}
}