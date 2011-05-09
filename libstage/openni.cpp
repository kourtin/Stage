#include "openni.h"
#include <iostream>
#include <limits>

// openni_async::openni_async() : etat_(XN_STATUS_OK), depth_out_(cv::Size(XN_VGA_X_RES, XN_VGA_Y_RES), CV_16UC1), img_out_(cv::Size(XN_VGA_X_RES, XN_VGA_Y_RES), CV_8UC3) {
// 	// Initialise le contexte
// 	exception_if_error(contexte_.Init());
// 	// Initialise le générateur de carte de profondeur
// 	exception_if_error(gen_depth_.Create(contexte_));
// 	// Initialise le générateur d'image
// 	exception_if_error(gen_image_.Create(contexte_));
// 	// Met la résolution voulue
// 	map_mode_.nXRes = XN_VGA_X_RES;
// 	map_mode_.nYRes = XN_VGA_Y_RES;
// 	map_mode_.nFPS = 30;
// 	exception_if_error(gen_depth_.SetMapOutputMode(map_mode_));
// 	exception_if_error(gen_image_.SetMapOutputMode(map_mode_));
// 	// Dit au contexte de commencer à envoyer la purée
// 	exception_if_error(contexte_.StartGeneratingAll());
// 	// Fait en sorte d'aligner les images
// 	gen_depth_.GetAlternativeViewPointCap().SetViewPoint(gen_image_);
// }
// 
// openni_async::~openni_async() {
// 	
// }
// 
// void openni_async::operator()() {
// 	for(;;) {
// 		contexte_.WaitAnyUpdateAll();
// 		if(gen_depth_.IsNewDataAvailable()) {
// 			const XnDepthPixel* depth_img = gen_depth_.GetDepthMap();
// 			for(int y=0; y < XN_VGA_Y_RES; ++y) {
// 				for(int x=0; x < XN_VGA_X_RES; ++x) {
// 					/*
// 						TODO Trouver la vraie équation
// 					*/
// 					depth_out_.at<unsigned short>(y,x) = 2048*(depth_img[y * XN_VGA_X_RES + x]*1.0 / 6000);
// 				}
// 			}
// 		}
// 		if(gen_image_.IsNewDataAvailable()) {
// 			unsigned char* img = reinterpret_cast<unsigned char*>(const_cast<XnRGB24Pixel*>(gen_image_.GetRGB24ImageMap()));
// 			std::copy(img, img + XN_VGA_X_RES * XN_VGA_Y_RES * 3, img_out_.data);
// 		}
// 	}
// }

openni::openni() : etat_(XN_STATUS_OK) {
	
}

void openni::init() {
	// Initialise le contexte
	exception_if_error(contexte_.Init());
	// Initialise le générateur de carte de profondeur
	exception_if_error(gen_depth_.Create(contexte_));
	// Initialise le générateur d'image
	exception_if_error(gen_image_.Create(contexte_));
	// Met la résolution voulue
	map_mode_.nXRes = XN_VGA_X_RES;
	map_mode_.nYRes = XN_VGA_Y_RES;
	map_mode_.nFPS = 30;
	exception_if_error(gen_depth_.SetMapOutputMode(map_mode_));
	exception_if_error(gen_image_.SetMapOutputMode(map_mode_));
	// Dit au contexte de commencer à envoyer la purée
	exception_if_error(contexte_.StartGeneratingAll());
	// Fait en sorte d'aligner les images
	gen_depth_.GetAlternativeViewPointCap().SetViewPoint(gen_image_);
}

openni::~openni() {
	// Ferme le contexte
	contexte_.Shutdown();
}

void openni::update() {
	// if(gen_depth_.IsNewDataAvailable())
		// contexte_.WaitOneUpdateAll(gen_depth_);
	contexte_.WaitNoneUpdateAll();
	// else if(gen_image_.IsNewDataAvailable())
		// contexte_.WaitOneUpdateAll(gen_image_);
}

bool openni::depth_to(cv::Mat& out) {
	// if(gen_depth_.IsNewDataAvailable()) {
		// std::cout << "depth" << std::endl;
		const XnDepthPixel* depth_img = gen_depth_.GetDepthMap();
		// std::copy(depth_img, depth_img + XN_VGA_X_RES * XN_VGA_Y_RES, out.data);
		for(int y=0; y < XN_VGA_Y_RES; ++y) {
			for(int x=0; x < XN_VGA_X_RES; ++x) {
				/*
					TODO Trouver la vraie équation
				*/
				out.at<unsigned short>(y,x) = 2048*(depth_img[y * XN_VGA_X_RES + x]*1.0 / 6000);
				// std::cout << depth_img[y * XN_VGA_X_RES + x] << std::endl;
			}
		}
		return true;
	// }
	// return false;
}

bool openni::img_to(cv::Mat& out) {
	// return false;
	// if(gen_image_.IsNewDataAvailable()) {
		// std::cout << "depth" << std::endl;
		unsigned char* img = reinterpret_cast<unsigned char*>(const_cast<XnRGB24Pixel*>(gen_image_.GetRGB24ImageMap()));
		std::copy(img, img + XN_VGA_X_RES * XN_VGA_Y_RES * 3, out.data);
		return true;
	// }
	// return false;
}