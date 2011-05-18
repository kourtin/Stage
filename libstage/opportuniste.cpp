#include "opportuniste.h"

#if defined(COMPILES_WITH_LIB_artoolkitplus)
#include "sondes/artoolkitplus.h"
#endif

#if defined(COMPILES_WITH_LIB_reactivision)
#include "sondes/reactivision.h"
#endif

#if defined(COMPILES_WITH_LIB_freenect)
#include "video/freenect.h"
#include "sondes/depth_segmentor.h"
#endif

#if defined(COMPILES_WITH_LIB_openni)
#include "video/openni.h"
#include "sondes/depth_segmentor.h"
#endif

#include "sondes/tuio.h"

#include "couplage_virtuel/thread_couplage.h"
#include "son/master_pd.h"
#include "comportements/comportement.h"
#include <opencv/cv.h>
#include <algorithm>

void opportuniste::activer_sonde_artkplus(bool b) {
	if(sonde_artkplus_activee()) {
		delete sonde_artkplus_;
		sonde_artkplus_ = 0;
	}
	#if defined(COMPILES_WITH_LIB_artoolkitplus)
	if(b)
		sonde_artkplus_ = new artoolkitplus(img_);
	#endif
}

void opportuniste::activer_sonde_reactivision(bool b) {
	if(sonde_reactivision_activee()) {
		delete sonde_reactivision_;
		sonde_reactivision_ = 0;
	}
	#if defined(COMPILES_WITH_LIB_reactivision)
	if(b)
		sonde_reactivision_ = new reactivision(img_);
	#endif
}

void opportuniste::activer_sonde_kinectsegmentor(bool b) {
	if(sonde_kinectsegmentor_activee()) {
		delete sonde_kinectsegmentor_;
		sonde_kinectsegmentor_ = 0;
	}
	#if defined(COMPILES_WITH_LIB_freenect) || defined(COMPILES_WITH_LIB_openni)
	if(b)
		sonde_kinectsegmentor_ = new depth_segmentor(depth_);
	#endif
}

void opportuniste::activer_sonde_tuio(bool b) {
	if(sonde_tuio_activee()) {
		delete sonde_tuio_;
		sonde_tuio_ = 0;
	}
	#if defined(COMPILES_WITH_LIB_tuio)
	if(b)
		sonde_tuio_ = new tuio_demux;
	#endif
}

void opportuniste::activer_kinect(bool b) {
	if(kinect_active()) {
		delete kinect_;
		kinect_ = 0;
	}
	#if /*defined(PIPICACA) */defined(COMPILES_WITH_LIB_openni)
	if(b) {
		activer_camera(false);
		kinect_ = new openni;
		kinect_->init();
	}
	#elif defined(COMPILES_WITH_LIB_freenect)
	if(b) {
		activer_camera(false);
		kinect_ = new freenect;
		kinect_->init();
	}
	#endif
}

void opportuniste::activer_camera(bool b) {
	if(camera_active()) {
		delete camera_;
		camera_ = 0;
	}
	if(b) {
		activer_kinect(false);
		camera_ = new camera;
	}
}

void opportuniste::update() {
	// Met à jour les trucs de video
	if(kinect_active()) {
		kinect_->update();
		kinect_->img_to(img_);
		kinect_->depth_to(depth_);
	}
	if(camera_active()) {
		camera_->img_to(img_);
	}
	// Met à jour les sondes
	if(sonde_artkplus_activee())
		(*sonde_artkplus_)(&store_);
	if(sonde_reactivision_activee())
		(*sonde_reactivision_)(&store_);
	if(sonde_tuio_activee())
		(*sonde_tuio_)(&store_);
	if(sonde_kinectsegmentor_activee())
		(*sonde_kinectsegmentor_)(&store_);
	// Exécute les comportements
	for(objet_store::iterator it = store_.begin(); it != store_.end(); ++it) {
		if((*it)->est_attache())
			(*it)->comportement_attache()->operator()();
		// (*it)->present(false);
	}
}

opportuniste::opportuniste() : img_(cv::Size(640, 480), CV_8UC3), depth_(cv::Size(640, 480), CV_16UC1), sonde_artkplus_(0), sonde_reactivision_(0), sonde_kinectsegmentor_(0), sonde_tuio_(0), kinect_(0), camera_(0) {
	thread_couplage::init();
	master_pd::instance();
}

opportuniste::~opportuniste() {
	if(sonde_artkplus_)
		delete sonde_artkplus_;
	if(sonde_reactivision_)
		delete sonde_reactivision_;
	if(sonde_kinectsegmentor_)
		delete sonde_kinectsegmentor_;
}