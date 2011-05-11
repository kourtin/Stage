#if !defined(OPPORTUNISTE_H_S3TZOH4A)
#define OPPORTUNISTE_H_S3TZOH4A

// #include "processus_sonores.h"
// #include "kinectivision.h"
// #include "kinect_segmentor.h"
// #include "arkinectoolkitplus.h"
// #include "openni.h"
// #include "freenect.h"
#include "sondes/sonde_environnementale.h"
#include "objets/objet_store.h"
#include "video/kinect.h"
#include "video/camera.h"
#include <opencv/cv.h>
#include "CinderOpenCV.h"
#include <vector>

struct opportuniste {
	opportuniste();
	virtual ~opportuniste();
	void activer_sonde_artkplus(bool);
	bool sonde_artkplus_activee() { return sonde_artkplus_ != 0; }
	void activer_sonde_reactivision(bool);
	bool sonde_reactivision_activee() { return sonde_reactivision_ != 0; }
	void activer_sonde_kinectsegmentor(bool);
	bool sonde_kinectsegmentor_activee() { return sonde_kinectsegmentor_ != 0; }
	void activer_sonde_tuio(bool);
	bool sonde_tuio_activee() { return sonde_tuio_ != 0; }
	void activer_kinect(bool);
	bool kinect_active() { return kinect_ != 0; }
	void activer_camera(bool);
	bool camera_active() { return camera_ != 0; }
	void update();
	ci::ImageSourceRef img() { return ci::fromOcv(img_); }
	objet_store& store() { return store_; }
private:
	cv::Mat img_, depth_;
	sonde_environnementale* sonde_artkplus_;
	sonde_environnementale* sonde_reactivision_;
	sonde_environnementale* sonde_kinectsegmentor_;
	sonde_environnementale* sonde_tuio_;
	objet_store store_;
	kinect* kinect_;
	camera* camera_;
};

#endif /* end of include guard: OPPORTUNISTE_H_S3TZOH4A */
