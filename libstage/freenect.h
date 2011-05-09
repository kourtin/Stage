#if !defined(FREENECT_H_YXW40CUJ)
#define FREENECT_H_YXW40CUJ

#include "kinect.h"
#include <libfreenect.hpp>
#include <opencv/cv.h>
#include <pthread.h>

class Mutex {
public:
	Mutex() {
		pthread_mutex_init( &m_mutex, NULL );
	}
	void lock() {
		pthread_mutex_lock( &m_mutex );
	}
	void unlock() {
		pthread_mutex_unlock( &m_mutex );
	}
private:
	pthread_mutex_t m_mutex;
};

struct freenect : public kinect {
	freenect();
	virtual void init();
	virtual ~freenect();
	virtual void update();
	virtual bool depth_to(cv::Mat&);
	virtual bool img_to(cv::Mat&);
private:
	struct kinect_device : public Freenect::FreenectDevice {
		kinect_device(freenect_context* ctx, int i);
		virtual ~kinect_device();
		void VideoCallback(void* rgb, uint32_t t);
		void DepthCallback(void* d, uint32_t t);
		bool prochaine_depth_map(cv::Mat& out);
		bool prochaine_image(cv::Mat& out);
	private:
		cv::Mat dipah_, img_;
		bool nouvelle_frame_depth_, nouvelle_image_;
		Mutex m_, m2_;
	};
	
	Freenect::Freenect freenect_;
	kinect_device* kinect_;
	// static convertisseur c_;
};

#endif /* end of include guard: FREENECT_H_YXW40CUJ */
