//
//  KinectCamera.h
//  reacTIVision
//
//  Created by Jonathan Aceituno on 28/04/11.
//  Copyright 2011 Jonathan Aceituno. All rights reserved.
//

#if !defined (__x86_64__) && !defined (__ppc64__)
#ifndef macvdCamera_H
#define macvdCamera_H

#include "../common/cameraEngine.h"
#include <libfreenect.hpp>
#include <unistd.h>
#define hibyte(x) (unsigned char)((x)>>8)

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

class KinectCamera : public CameraEngine
{
public:
	KinectCamera(const char* config_file);
	~KinectCamera();
	
	bool findCamera();
	bool initCamera();
	bool startCamera();
	unsigned char* getFrame();
	bool stopCamera();
	bool stillRunning();
	bool resetCamera();
	bool closeCamera();
	
	int getCameraSettingStep(int mode) { return 0; }
	bool setCameraSettingAuto(int mode, bool flag) { return false; }
	bool setCameraSetting(int mode, int value) { return false; }
	int getCameraSetting(int mode) { return 0; }
	int getMaxCameraSetting(int mode) { return 0; }
	int getMinCameraSetting(int mode) { return 0; }
	
	void showSettingsDialog();
	// void drawGUI(SDL_Surface *display) {};
	
private:
	struct kinect_device : public Freenect::FreenectDevice {
		kinect_device(freenect_context* ctx, int i);
		virtual ~kinect_device();
		void VideoCallback(void* rgb, uint32_t t);
		void DepthCallback(void* d, uint32_t t);
		bool prochaine_image(unsigned char* out);
	private:
		unsigned char* img_;
		bool nouvelle_image_;
		Mutex m_;
	};
	Freenect::Freenect freenect_;
	kinect_device& kinect_;
	
	Rect		dstPortBounds;
	Rect		dstDisplayBounds;
	void*		pDstData;
	long		dstDataSize;
};

#endif
#endif
