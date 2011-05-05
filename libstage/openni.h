#if !defined(OPENNI_H_AS1HOU9J)
#define OPENNI_H_AS1HOU9J

#include "kinect.h"
#include <opencv/cv.h>
#include <XnCppWrapper.h>
#include <exception>
#include <string>

struct exception_openni : public std::exception {
	exception_openni(XnStatus e) : err_(e) {}
	virtual ~exception_openni() throw() {}
	std::string raison() { return xnGetStatusString(err_); }
private:
	XnStatus err_;
};

struct openni : public kinect {
	openni();
	virtual ~openni();
	virtual void init();
	bool depth_to(cv::Mat&);
	bool img_to(cv::Mat&);
private:
	void exception_if_error(XnStatus z) { if(z != XN_STATUS_OK) throw exception_openni(z); }
	XnStatus etat_;
	xn::Context contexte_;
	xn::DepthGenerator gen_depth_;
	xn::ImageGenerator gen_image_;
	XnMapOutputMode map_mode_;
	
	cv::Mat depth_out_;
};

#endif /* end of include guard: OPENNI_H_AS1HOU9J */
