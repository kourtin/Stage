#if !defined(KINECT_H_T17ISX17)
#define KINECT_H_T17ISX17

#include <libfreenect.hpp>
#include <opencv/cv.h>

struct kinect {
	kinect();
	virtual ~kinect();
	bool depth_to(cv::Mat&);
private:
	struct kinect_device : public Freenect::FreenectDevice {
		kinect_device(freenect_context* ctx, int i);
		virtual ~kinect_device();
		void VideoCallback(void* rgb, uint32_t t);
		void DepthCallback(void* d, uint32_t t);
		bool prochaine_depth_map(cv::Mat& out);
	private:
		cv::Mat dipah_;
		bool nouvelle_frame_depth_;
	};
	
	Freenect::Freenect freenect_;
	kinect_device& kinect_;
};



#endif /* end of include guard: KINECT_H_T17ISX17 */
