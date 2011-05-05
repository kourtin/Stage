#if !defined(KINECT_H_T17ISX17)
#define KINECT_H_T17ISX17

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

class convertisseur {
	float depth_lut[2048];
	static const float k1 = 1.1863;
	static const float k2 = 2842.5;
	static const float k3 = 0.1236;
public:
	convertisseur() {
		for(size_t i=0; i<2048; ++i)
			depth_lut[i] = k3 * tanf((i*1.0)/k2 + k1);
	}
	float operator()(size_t depth) {
		return depth_lut[std::min(std::max(depth, static_cast<size_t>(400)), static_cast<size_t>(2000))];
	}
	unsigned int scl(float f) {
		return std::max(std::min(f, 3.0f), 0.5f) * 2047.0 / 6.0 + 0.37409f;
	}
};

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
		Mutex m_;
	};
	
	Freenect::Freenect freenect_;
	kinect_device& kinect_;
	static convertisseur c_;
};



#endif /* end of include guard: KINECT_H_T17ISX17 */
