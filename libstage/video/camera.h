#if !defined(CAMERA_H_V0RKX2FG)
#define CAMERA_H_V0RKX2FG

#include <cinder/Capture.h>
#include <opencv/cv.h>

struct camera {
	camera();
	virtual ~camera();
	void img_to(cv::Mat& m);
	ci::Surface8u surface();
private:
	ci::Capture capt_;
	cv::Mat mit_;
};

#endif /* end of include guard: CAMERA_H_V0RKX2FG */
