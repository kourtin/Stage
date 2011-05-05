#include "kinect.h"
#include <iostream>

convertisseur kinect::c_ = convertisseur();

kinect::kinect() : kinect_(freenect_.createDevice<kinect_device>(0)) {
	kinect_.startDepth();
}

kinect::~kinect() {
	kinect_.stopDepth();
}

bool kinect::depth_to(cv::Mat& o) {
	return kinect_.prochaine_depth_map(o);
}

kinect::kinect_device::kinect_device(freenect_context* ctx, int i) : Freenect::FreenectDevice(ctx, i), dipah_(cv::Size(640,480), CV_16UC1) {}

kinect::kinect_device::~kinect_device() {}

void kinect::kinect_device::VideoCallback(void* rgb, uint32_t t) {}

void kinect::kinect_device::DepthCallback(void* d, uint32_t t) {
	m_.lock();
	dipah_.data = static_cast<unsigned char*>(d);
	// std::cout << "min: " << mini << " -- max: " << maxi << std::endl;
	// memcpy(dipah_.data, d, FREENECT_DEPTH_SIZE);
	nouvelle_frame_depth_ = true;
	m_.unlock();
}

bool kinect::kinect_device::prochaine_depth_map(cv::Mat& out) {
	m_.lock();
	if(nouvelle_frame_depth_) {
		dipah_.copyTo(out);
		// unsigned short v_avant = 1;
		float mini = 2049;
		float maxi = 1;
		void* d = out.data;
		// for(size_t i=0; i<640*480; ++i) {
		// 	unsigned short v = static_cast<unsigned short*>(d)[i];
		// 	if(v >= 2047) {
		// 		continue;
		// 	}
		// 	// else v_avant = v;
		// 	
		// 	// v = c_.scl(c_(v));
		// 	
		// 	static_cast<unsigned short*>(d)[i] = v;
		// 	if(v > maxi)
		// 		maxi = v;
		// 	if(v < mini)
		// 		mini = v;
		// }
		// std::cout << "min: " << mini << ";;; max: " << maxi << std::endl;
		nouvelle_frame_depth_ = false;
		m_.unlock();
		return true;
	}	
	m_.unlock();
	return false;
}