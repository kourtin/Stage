#include "video/camera.h"
#include "CinderOpenCV.h"
#include <algorithm>

camera::camera() : mit_(cv::Size(640, 480), CV_8UC3) {
	const ci::Capture::DeviceRef ps3eye = ci::Capture::findDeviceByNameContains("PS3");
	capt_ = ci::Capture(640, 480, ps3eye);
}

camera::~camera() {
	capt_.stop();
}

void camera::img_to(cv::Mat& m) {
	if(!capt_.isCapturing())
		capt_.start();
	if(capt_ && capt_.checkNewFrame()) {
		// mit_ = toOcv(capt_.getSurface());
		// std::copy(mit_.data, mit_.data + mit_.rows * mit_.cols * 3, m.data);
		// m.data = mit_.data;
		m.data = capt_.getSurface().getData();
	}
}

ci::Surface8u camera::surface() {
	return capt_.getSurface();
}