#include "kinect.h"

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
	dipah_.data = static_cast<uchar*>(d);
	nouvelle_frame_depth_ = true;
}

bool kinect::kinect_device::prochaine_depth_map(cv::Mat& out) {
	if(nouvelle_frame_depth_) {
		dipah_.copyTo(out);
		nouvelle_frame_depth_ = false;
		return true;
	}
	return false;
}