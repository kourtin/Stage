//
//  KinectCamera.cpp
//  reacTIVision
//
//  Created by Jonathan Aceituno on 28/04/11.
//  Copyright 2011 Jonathan Aceituno. All rights reserved.
//

#if !defined (__x86_64__) && !defined (__ppc64__)

#include "KinectCamera.h"

KinectCamera::kinect_device::kinect_device(freenect_context* ctx, int i) : Freenect::FreenectDevice(ctx, i), img_(new unsigned char[FREENECT_VIDEO_RGB_SIZE]) {}

void rgb2gray(int width, int height, unsigned char *src, unsigned char *dest) {
	for (int i=height*width/2;i>0;i--) {
		float r = *src++ * 1.0 / 255;
		float g = *src++ * 1.0 / 255;
		float b = *src++ * 1.0 / 255;
		// Ref : http://www.mathworks.com/help/toolbox/images/ref/rgb2gray.html
		float gr = 0.2989 * r + 0.5870 * g + 0.1140 * b;
		*dest++ = static_cast<unsigned char>(gr / 1.0 * 255);
	}
}

KinectCamera::KinectCamera(const char* cfg):CameraEngine(cfg), kinect_(freenect_.createDevice<kinect_device>(0))
{
	cameraID = -1;
	
	buffer = NULL;
	
	running=false;
	lost_frames=0;
	
	timeout = 5000;
}

KinectCamera::~KinectCamera()
{
	if (buffer!=NULL) delete []buffer;
}

void KinectCamera::kinect_device::VideoCallback(void* rgb, uint32_t t) {
	m_.lock();
	img_ = static_cast<unsigned char*>(rgb);
	nouvelle_image_ = true;
	m_.unlock();
}

void KinectCamera::kinect_device::DepthCallback(void* d, uint32_t t) {}

bool KinectCamera::kinect_device::prochaine_image(unsigned char* out) {
	m_.lock();
	if(nouvelle_image_) {
		// Copie
		std::copy(img_, img_ + getVideoBufferSize(), out);
		nouvelle_image_ = false;
		m_.unlock();
		return true;
	}	
	m_.unlock();
	return false;
}

bool KinectCamera::findCamera() {
	
	/*OSErr err;
	
	if(!(pVdg = vdgNew()))
	{
		//printf("vdgNew: failed to allocate\n");
		return false;
	}
	
	if(err = vdgInit(pVdg))
	{
		//printf("vdgInit err=%d\n", err);
		printf("no camera found\n");
		return false;
	}*/
	
	cameraID = 0;
	return true;
}

bool KinectCamera::initCamera() {
	
	if (cameraID < 0) return false;
	readSettings();
	
	/*OSErr err;
	
	if (config.device!=SETTING_AUTO) {
		if(err = vdgRequestSettingsNoGUI(pVdg))
		{
			printf("camera setup cancelled\n");
			//printf("vdgRequestSettings err=%d\n", err);
			return false;
		}
	} else {
		if(err = vdgRequestSettings(pVdg))
		{
			printf("camera setup cancelled\n");
			//printf("vdgRequestSettings err=%d\n", err);
			return false;
		}		
	}
	
    long nameLength = 256;
	if (err = vdgGetDeviceNameAndFlags(pVdg, cameraName, &nameLength, NULL))
	{
		sprintf(cameraName,"unknown camera");
	}
	
	long milliSecPerFrame;
	Fixed framerate;
	long bytesPerSecond;
	if (err = vdgGetDataRate(pVdg, &milliSecPerFrame, &framerate, &bytesPerSecond))
	{
		fps = 30;
	} else fps = (int)(framerate/65536);*/
	fps = 30;
	
	//fps = vdgGetFrameRate(pVdg);	
	//printf("%d\n",fps);
	
	/*if(err = vdgPreflightGrabbing(pVdg))
	{
		//printf("vdgPreflightGrabbing err=%d\n", err);
		return false;
	}
	
	vdImageDesc = (ImageDescriptionHandle)NewHandle(0);
	if (err = vdgGetImageDescription( pVdg, 
									 vdImageDesc))
	{
		//printf("vdgGetImageDescription err=%d\n", err);
		return false;
	}
	
	int max_width = (*vdImageDesc)->width;
	int max_height = (*vdImageDesc)->height;
	
	if ((config.width>0) && (config.height>0)) {
		dstPortBounds.left = config.xoff;
		dstPortBounds.right = config.xoff+config.width;
		dstPortBounds.top = config.yoff;
		dstPortBounds.bottom = config.yoff+config.height;
		
		if (dstPortBounds.left<0) dstPortBounds.left = 0;
		if (dstPortBounds.right>max_width) dstPortBounds.right = max_width;
		if (dstPortBounds.top<0) dstPortBounds.top = 0;
		if (dstPortBounds.bottom>max_height) dstPortBounds.bottom = max_height;
	} else {
		dstPortBounds.left = 0;
		dstPortBounds.right = max_width;
		dstPortBounds.top = 0;
		dstPortBounds.bottom = max_height;
	}
	
	if (err = createOffscreenGWorld(	&dstPort,
									//kYUV420CodecType,
									//kComponentVideoSigned,
									//kComponentVideoCodecType,
									k422YpCbCr8CodecType,
									&dstPortBounds))
	{
		printf("createOffscreenGWorld err=%d\n", err);
		return false;	
	}
	
	// Get buffer from GWorld
	pDstData = GetPixBaseAddr(GetGWorldPixMap(dstPort));
	dstDataSize = GetPixRowBytes(GetGWorldPixMap(dstPort)) * (dstPortBounds.bottom - dstPortBounds.top); 
	dstDisplayBounds = dstPortBounds;
	
	
	// Set the decompression destination to the offscreen GWorld
	if (err = vdgSetDestination(	pVdg, dstPort ))
	{
		//printf("vdgSetDestination err=%d\n", err);
		return false;
	}
	*/
	this->width = 640;
	this->height = 480;
//	this->width =dstPortBounds.right - dstPortBounds.left;
//	this->height = dstPortBounds.bottom - dstPortBounds.top;
	
	buffer = new unsigned char[this->width*this->height*bytes];
	return true;
}

unsigned char* KinectCamera::getFrame()
{
	/*OSErr   err;
	int		isUpdated = 0;
	
	if (!vdgIsGrabbing(pVdg)) return NULL;
	
	if (err = vdgIdle( pVdg, &isUpdated))
	{
		//printf("could not grab frame\n");
		return NULL;
	}
	
	if (isUpdated)
	{
		unsigned char *src = (unsigned char*)pDstData;
		unsigned char *dest = buffer;
		
		switch (colour) {
			case true: {
				uyvy2rgb(width,height,src,dest);
				break;
			}
			case false: {
				uyvy2gray(width,height,src,dest);
				break;
			}
		}
		lost_frames=0;
		timeout = 1000;
		return buffer;
	} else {
		usleep(1000);
		lost_frames++;
		if (lost_frames>timeout) running=false; // give up after 5 (at init) or 2 (at runtime) seconds
		return NULL;
	}*/
	kinect_.prochaine_image(buffer);
	return buffer;
}

bool KinectCamera::startCamera()
{
	
	/*OSErr err;
	if (err = vdgStartGrabbing(pVdg))
	{
		printf("could not start camera\n");
		return false;
	}*/
	
	kinect_.startVideo();
	
	running = true;
	return true;
}

bool KinectCamera::stopCamera()
{
	running=false;
	
	/*OSErr err;
	if (err = vdgStopGrabbing(pVdg))
	{
		printf("errors while stopping camera\n");
		return false;
	}*/
	kinect_.stopVideo();
	
	return true;
}

bool KinectCamera::stillRunning() {
	return running;
}

bool KinectCamera::resetCamera()
{
	return (stopCamera() && startCamera());
}

bool KinectCamera::closeCamera()
{
	
	/*if (dstPort)
	{
		disposeOffscreenGWorld(dstPort);
		dstPort = NULL;
	}
	
	if (vdImageDesc)
	{
		DisposeHandle((Handle)vdImageDesc);
		vdImageDesc = NULL;
	}
	
	if (pVdg)
	{
		vdgUninit(pVdg);
		vdgDelete(pVdg);
		pVdg = NULL;
	}*/
	stopCamera();
	
	return true;
}

void KinectCamera::showSettingsDialog() {
	/*vdgStopGrabbing(pVdg);
	vdgShowSettings(pVdg);	
	vdgPreflightGrabbing(pVdg);
	vdgStartGrabbing(pVdg);*/
}

#endif
