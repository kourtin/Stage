#if !defined(KINECT_H_VYZ77VMO)
#define KINECT_H_VYZ77VMO

#include <opencv/cv.h>

struct kinect {
	kinect();
	virtual ~kinect();
	virtual void init() = 0;
	virtual bool depth_to(cv::Mat&) = 0;
	virtual bool img_to(cv::Mat&) = 0;
};

#endif /* end of include guard: KINECT_H_VYZ77VMO */
