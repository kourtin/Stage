#include "video/camera.h"
#include "CinderOpenCV.h"
#include <algorithm>

camera::camera() : capt_(640, 480), mit_(cv::Size(640, 480), CV_8UC3) {}

camera::~camera() {
	capt_.stop();
}

void camera::img_to(cv::Mat& m) {
	if(!capt_.isCapturing())
		capt_.start();
	if(capt_ && capt_.checkNewFrame()) {
		mit_ = toOcv(capt_.getSurface());
		std::copy(mit_.data, mit_.data + mit_.rows * mit_.cols * 3, m.data);
	}
}

ci::Surface8u camera::surface() {
	return capt_.getSurface();
}