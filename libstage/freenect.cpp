#include "freenect.h"
#include <iostream>

// convertisseur freenect::c_ = convertisseur();

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
		// std::cout << "depth" << std::endl;
		return true;
	}	
	m_.unlock();
	return false;
}

bool freenect::kinect_device::prochaine_image(cv::Mat& out) {
	m2_.lock();
	if(nouvelle_image_) {
		// float mat_cam[9] = {5.7581573486328125e+02, 0., 320., 0., 5.7581573486328125e+02,
		// 			240., 0., 0., 1.};
		// 		cv::Mat dist = cv::Mat::zeros(8, 1, CV_64F);
		// 		dist.at<double>(0) = 0.17710499257899193;
		// 		dist.at<double>(1) = -0.83152290437826637;
		// 		dist.at<double>(2) = -0.023601182007631782;
		// 		dist.at<double>(3) = 0.013186630055783820;
		// 		dist.at<double>(4) =  1.0365227884736543;
		// 		// cv::Mat matcam(3, 3, CV_64F, mat_cam);
		// 		cv::Mat matcam = cv::Mat::eye(3,3,CV_64F);
		// 		matcam.at<double>(0,0) = 5.7581573486328125e+02;
		// 		matcam.at<double>(1,1) = 5.7581573486328125e+02;
		// 		matcam.at<double>(0,2) = 320;
		// 		matcam.at<double>(1,2) = 240;
		// 		static std::clock_t t = std::clock();
		// 		static bool ok = false;
		// if(std::clock() - t > 2 * CLOCKS_PER_SEC) {
		// 			std::cout << "BIP" << std::endl;
		// 			t = std::clock();
		// 			ok = !ok;
		// 		}
		// 		if(ok)
		// 			cv::undistort(img_, out, matcam, dist);
		// 		else
			img_.copyTo(out);
		nouvelle_image_ = false;
		m2_.unlock();
		// std::cout << "img" << std::endl;
		return true;
	}	
	m2_.unlock();
	return false;
}