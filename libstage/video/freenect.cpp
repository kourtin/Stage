#if defined(COMPILES_WITH_LIB_freenect)

#include "video/freenect.h"
#include <iostream>

freenect::freenect() {}

void freenect::init() {
	kinect_ = &freenect_.createDevice<kinect_device>(0);
	kinect_->startDepth();
	kinect_->startVideo();
}

freenect::~freenect() {
	if(kinect_) {
		kinect_->stopVideo();
		kinect_->stopDepth();
	}
}

bool freenect::depth_to(cv::Mat& o) {
	if(kinect_)
		return kinect_->prochaine_depth_map(o);
	else
		return false;
}

bool freenect::img_to(cv::Mat& o) {
	if(kinect_)
		return kinect_->prochaine_image(o);
	else
		return false;
}

freenect::kinect_device::kinect_device(freenect_context* ctx, int i) : Freenect::FreenectDevice(ctx, i), dipah_(cv::Size(640,480), CV_16UC1), img_(cv::Size(640,480), CV_8UC3) {}

freenect::kinect_device::~kinect_device() {}

void freenect::kinect_device::VideoCallback(void* rgb, uint32_t t) {
	m2_.lock();
	img_.data = static_cast<unsigned char*>(rgb);
	nouvelle_image_ = true;
	m2_.unlock();
}

void freenect::kinect_device::DepthCallback(void* d, uint32_t t) {
	m_.lock();
	dipah_.data = static_cast<unsigned char*>(d);
	nouvelle_frame_depth_ = true;
	m_.unlock();
}

bool freenect::kinect_device::prochaine_depth_map(cv::Mat& out) {
	m_.lock();
	if(nouvelle_frame_depth_) {
		dipah_.copyTo(out);
		nouvelle_frame_depth_ = false;
		m_.unlock();
		return true;
	}
	m_.unlock();
	return false;
}

bool freenect::kinect_device::prochaine_image(cv::Mat& out) {
	m2_.lock();
	if(nouvelle_image_) {
		img_.copyTo(out);
		nouvelle_image_ = false;
		m2_.unlock();
		return true;
	}	
	m2_.unlock();
	return false;
}

#endif