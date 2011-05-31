#if defined(COMPILES_WITH_LIB_artoolkitplus)

#include "sondes/artoolkitplus.h"
#include "objets/objet_store.h"
#include "maths.h"
#include <CoreFoundation/CoreFoundation.h>
#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>
#include <ARToolKitPlus/TrackerMultiMarkerImpl.h>
#include <cinder/Rect.h>
#include <cinder/Vector.h>
#include <cinder/Matrix.h>
#include <cmath>

artoolkitplus::artoolkitplus(cv::Mat& img) : image_couleur_(img), image_nb_(cv::Size(640, 480), CV_8UC1), tracker_(new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6>(640,480)), tracker_world_(new ARToolKitPlus::TrackerMultiMarkerImpl<6,6,6, 1, 16>(640,480)) {
	char app_path[1024];
	char fichier[1024], fichierworld[1024];
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef mainBundleURL = CFBundleCopyBundleURL( mainBundle);
	CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
	CFStringGetCString( cfStringRef, app_path, 1024, kCFStringEncodingASCII);	
	CFRelease( mainBundleURL);
	CFRelease( cfStringRef);
	sprintf(fichier,"%s/Contents/Resources/no_distortion.cal",app_path); // kinect_burrus_rgbdemo.cal
	sprintf(fichierworld,"%s/Contents/Resources/worldreference.cfg",app_path); // kinect_burrus_rgbdemo.cal
	tracker_->init(fichier, 1.0f, 1000.0f);
	tracker_->setBorderWidth(0.125f);
	tracker_->setPatternWidth(80);
	tracker_->activateAutoThreshold(true);
	tracker_->setUndistortionMode(ARToolKitPlus::UNDIST_NONE);
	tracker_->activateVignettingCompensation(true);
	tracker_->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);
	tracker_->setMarkerMode(ARToolKitPlus::MARKER_ID_BCH);
	tracker_->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_LUM);
	
	// tracker_world_->init(fichier, fichierworld, 1.0f, 1000.0f);
	// tracker_world_->setBorderWidth(0.125f);
	// // tracker_world_->setPatternWidth(80);
	// tracker_world_->activateAutoThreshold(true);
	// tracker_world_->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);
	// tracker_world_->setMarkerMode(ARToolKitPlus::MARKER_ID_BCH);
	// tracker_world_->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_LUM);
}

artoolkitplus::~artoolkitplus() {}

void artoolkitplus::operator()(objet_store* store) {
	if(!store) return;
	
	// world::instance().set_projection(ci::Matrix44f(tracker_->getProjectionMatrix()));
	
	int threshold = 100;
	
	// Convertit l'image en niveaux de gris
	cv::cvtColor(image_couleur_, image_nb_, CV_BGR2GRAY);

	ARToolKitPlus::ARMarkerInfo* infos;
	int nb_marqueurs;
	int marker_id;
	
	// Eteint les marqueurs qu'il faut
	// for(liste_alive::iterator it = alive_.begin(); it != alive_.end(); ++it) {
		// alive_info inf = *it;
		// if(inf.second > temps_max_alive_)
	// }
	
	for(objet_store::iterator it = store->begin(); it != store->end(); ++it) {
		(*it)->present(false);
	}
	
	// Détecte le monde
	// int numDetected = tracker_world_->calc(image_nb_.data);
	// ci::Matrix44f mtrx_world;
	// if(tracker_world_->getModelViewMatrix()[12] != 0) {
		// mtrx_world = ci::Matrix44f(tracker_world_->getModelViewMatrix());
		// world::instance().set_world(mtrx_world);
	// }
	
	// Détecte des marqueurs
	if((marker_id = tracker_->arDetectMarker(image_nb_.data, threshold, &infos, &nb_marqueurs)) < 0)
		return;
	
	for(int i=0; i<nb_marqueurs; ++i) {
		float patt_w = 6.0;
		float patt_c[2] = { 0.0, 0.0 };
		float patt_tr[3][4];
		
		if(infos[i].id <= 0/* || infos[i].id == 565 || infos[i].id == 566 || infos[i].id == 629 || infos[i].id == 630 || infos[i].id == 693 || infos[i].id == 694*/)
			continue;
		
		// Sélectionne l'objet correspondant à cet identifiant et commence à remplir les infos
		objet& o = store->get_or_create(infos[i].id);
		float x_cam = infos[i].pos[0] * 1.0 / 640;
		float y_cam = infos[i].pos[1] * 1.0 / 480;
		
		// Estimation de pose
		tracker_->executeSingleMarkerPoseEstimator(infos + i, patt_c, patt_w, patt_tr);
		float x, y, z;
		angles_euler(patt_tr, x, y, z);
		// On a l'angle de rotation par rapport à la camera
		o.r(z + M_PI);
		o.x(x_cam);
		o.y(y_cam);
		// On a une estimation de la profondeur
		o.z(patt_tr[2][3] / 500);
		
		// ci::Vec3f coords_pose(patt_tr[0][3], patt_tr[1][3], patt_tr[2][3]);
		// ci::Vec3f coords_poseworld(world::instance().get_world().inverted().transformPoint(coords_pose));
		// o.x(coords_poseworld.x);
		// o.y(coords_poseworld.y);
		// o.z(coords_poseworld.z);
		
		// Fait un rectangle qui inclut tous les points
		float padding = 5;
		ci::Rectf r(infos[i].pos[0] - padding, infos[i].pos[1] - padding, infos[i].pos[0] + padding, infos[i].pos[1] + padding);
		// ci::Vec2f point_hautgauche, point_hautdroit, point_basgauche, point_basdroit;
		for(unsigned int j=0; j < 4; ++j) {
			ci::Vec2f pnt_actuel(infos[i].vertex[j][0], infos[i].vertex[j][1]);
			r.include(pnt_actuel);
		}
		
		// Ce rectangle est sur l'objet (en relatif)
		o.rect(ci::Rectf(r.getX1() * 1.0/640 - x_cam, r.getY1() * 1.0/480 - y_cam, r.getX2() * 1.0/640 - x_cam, r.getY2() * 1.0/480 - y_cam));
		// Ah oui, l'objet est présent
		o.present(true);
	}
}

#endif