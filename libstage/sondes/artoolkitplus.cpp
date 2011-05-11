#if defined(COMPILES_WITH_LIB_artoolkitplus)

#include "sondes/artoolkitplus.h"
#include "objets/objet_store.h"
#include <CoreFoundation/CoreFoundation.h>
#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>
#include <cinder/Rect.h>
#include <cinder/Vector.h>
#include <cmath>

void angles_euler(float m[3][4], float& x, float& y, float& z) {
	if(std::abs(m[2][0]) != 1) {
		float th1 = -asin(m[2][0]);
		// float th2 = M_PI - th1;
		float psi1 = atan2(m[2][1] / cos(th1), m[2][2] / cos(th1));
		// float psi2 = atan2(m[2][1] / cos(th2), m[2][2] / cos(th2));
		float phi1 = atan2(m[1][0] / cos(th1), m[0][0] / cos(th1));
		// float phi2 = atan2(m[1][0] / cos(th2), m[0][0] / cos(th2));
		x = psi1;
		y = th1;
		z = phi1;
	} else {
		z = 0;
		if(m[2][0] == -1) {
			y = M_PI / 2;
			x = z + atan2(m[0][1], m[0][2]);
		} else {
			y = -M_PI / 2;
			x = -z + atan2(-m[0][1], -m[0][2]);
		}
	}
}

artoolkitplus::artoolkitplus(cv::Mat& img) : image_couleur_(img), image_nb_(cv::Size(640, 480), CV_8UC1), tracker_(new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6>(640,480)) {
	char app_path[1024];
	char fichier[1024];
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef mainBundleURL = CFBundleCopyBundleURL( mainBundle);
	CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
	CFStringGetCString( cfStringRef, app_path, 1024, kCFStringEncodingASCII);	
	CFRelease( mainBundleURL);
	CFRelease( cfStringRef);
	sprintf(fichier,"%s/Contents/Resources/no_distortion.cal",app_path); // kinect_burrus_rgbdemo.cal
	tracker_->init(fichier, 1.0f, 1000.0f);
	tracker_->setBorderWidth(0.125f);
	tracker_->activateAutoThreshold(true);
	tracker_->setUndistortionMode(ARToolKitPlus::UNDIST_NONE);
	tracker_->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);
	tracker_->setMarkerMode(ARToolKitPlus::MARKER_ID_BCH);
	tracker_->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_LUM);
}

artoolkitplus::~artoolkitplus() {}

void artoolkitplus::operator()(objet_store* store) {
	if(!store) return;
	
	int threshold = 100;
	
	// Convertit l'image en niveaux de gris
	cv::cvtColor(image_couleur_, image_nb_, CV_BGR2GRAY);

	ARToolKitPlus::ARMarkerInfo* infos;
	int nb_marqueurs;
	int marker_id;
	
	// Détecte des marqueurs
	if((marker_id = tracker_->arDetectMarker(image_nb_.data, threshold, &infos, &nb_marqueurs)) < 0)
		return;
	
	for(int i=0; i<nb_marqueurs; ++i) {
		float patt_w = 6.0;
		float patt_c[2] = { 0.0, 0.0 };
		float patt_tr[3][4];
		
		if(infos[i].id <= 0)
			continue;
		
		// Sélectionne l'objet correspondant à cet identifiant et commence à remplir les infos
		objet& o = store->get_or_create(infos[i].id);
		o.x(infos[i].pos[0] * 1.0 / 640);
		o.y(infos[i].pos[1] * 1.0 / 480);
		
		// Fait un rectangle qui inclut tous les points
		float padding = 5;
		ci::Rectf r(infos[i].pos[0] - padding, infos[i].pos[1] - padding, infos[i].pos[0] + padding, infos[i].pos[1] + padding);
		ci::Vec2f point_hautgauche, point_hautdroit, point_basgauche, point_basdroit;
		for(unsigned int j=0; j < 4; ++j) {
			ci::Vec2f pnt_actuel(infos[i].vertex[j][0], infos[i].vertex[j][1]);
			r.include(pnt_actuel);
		}
		
		// Ce rectangle est sur l'objet (en relatif)
		o.rect(ci::Rectf(r.getX1() * 1.0/640 - o.x_c(), r.getY1() * 1.0/480 - o.y_c(), r.getX2() * 1.0/640 - o.x_c(), r.getY2() * 1.0/480 - o.y_c()));
		// Ah oui, l'objet est présent
		o.present(true);
		
		// Estimation de pose
		tracker_->executeSingleMarkerPoseEstimator(infos + i, patt_c, patt_w, patt_tr);
		float x, y, z;
		angles_euler(patt_tr, x, y, z);
		// On a l'angle de rotation par rapport à la camera
		o.r(z);
		// On a une estimation de la profondeur
		o.z(patt_tr[2][3] / 500);
	}
}

#endif