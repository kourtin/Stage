#include "comportement.h"
#include "liveosc.h"
#include "objet_store.h"
#include "tuio.h"
#include <osc/OscPrintReceivedElements.h>

#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cinder/app/AppBasic.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>

using namespace ci;
using namespace ci::app;

#include "processus_sonores.h"

#include "kinectivision.h"
#include "kinect_segmentor.h"
#include "arkinectoolkitplus.h"
#include "kinect.h"

#include "CinderOpenCv.h"

struct oscillathor : public comportement {
	oscillathor(objet& o) : comportement(o) {}
	virtual void operator()() {
		oscil_.frequence(440 * objet_attache().x());
	}
private:
	oscillateur oscil_;
};

struct affiche_objets : public AppBasic {
	affiche_objets() : store_(), tuio_(store_), last_msg_("Reactivision"), alphi_(0), comp_(0), k_(&kinect_), k2_(&kinect_), k3_(&kinect_), reactivision_(true), which_image_(false), s_(0) {}
	
	virtual ~affiche_objets() {
		if(alphi_)
			delete alphi_;
		if(comp_)
			delete comp_;
	}
	
	void setup() {
		srand(time(NULL));
		master_pd::instance();
		setFrameRate(60.0);
		image_cam1_ = gl::Texture(Surface(getWindowWidth(), getWindowHeight(), false));
		image_cam2_ = gl::Texture(Surface(getWindowWidth(), getWindowHeight(), false));
	}
	
	void update() {
		for(objet_store::iterator it = store_.begin(); it != store_.end(); ++it) {
			(*it)->present(false);
			(*it)->rect() = floatrect();
		}
		if(reactivision_) {
			k_(&store_);
			image_cam1_ = gl::Texture(fromOcv(k_.image()));
		}
		else {
			k3_(&store_);
			image_cam1_ = gl::Texture(fromOcv(k3_.image()));
		}
		k2_(&store_);
		// image_cam2_ = gl::Texture(fromOcv(*k2_.segments_[s_]));
		image_cam2_ = gl::Texture(fromOcv(k2_.petite_image_));
		// if(store_.objets().size() >= 1 && alphi_ == 0) {
		// 			alphi_ = new objet_alpha(*(*(store_.begin())));
		// 		}
		// 		if(store_.objets().size() >= 1 && alphi_ != 0) {
		// 			(*alphi_).operator()();
		// 		}
		// 		if(store_.objets().size() >= 2 && comp_ == 0) {
		// 			comp_ = new oscillathor(*(*(++store_.begin())));
		// 		}
		// 		if(store_.objets().size() >= 2 && comp_ != 0) {
		// 			(*comp_).operator()();
		// 		}
		// if(liveosc_.pending()) {
			// std::ostringstream oss;
			// osc::ReceivedMessage m(liveosc_.next());
			// oss << "Last message: " << m;
			// last_msg_ = oss.str();
		// }
	}
	
	void draw() {
		static float sec = getElapsedSeconds();
		static float fra = getElapsedFrames();
		static float fps = 0;
		
		float seconds = getElapsedSeconds();
		float frames = getElapsedFrames();
		
		if(seconds - sec > 1) {
			fps = frames - fra;
			fra = frames;
			sec = seconds;
		}
		
		std::ostringstream oss;
		if(seconds > 0) {
			oss << fps << " fps. Segment " << s_;
		}
		gl::clear(Color(0.f, 0.f, 0.f));
		gl::color(Color(1.0, 1.0, 1.0));
		if(which_image_)
			gl::draw(image_cam2_, getWindowBounds());
		else
			gl::draw(image_cam1_, getWindowBounds());
		
		// if(false)
		for(size_t i = 0; i < k2_.blobs_.size(); ++i) {
			gl::color(ColorA(1.0 - (i * 1.0 / k2_.blobs_.size()), 0.0, 0.0, 1.0));
			blobbos& o = k2_.blobs_[i];
			Rectf r(o.rect.x1 * getWindowWidth(), o.rect.y1 * getWindowHeight(), o.rect.x2 * getWindowWidth(), o.rect.y2 * getWindowHeight());
			gl::drawLine(r.getUpperLeft(), r.getUpperRight());
			gl::drawLine(r.getLowerLeft(), r.getLowerRight());
			gl::drawLine(r.getUpperLeft(), r.getLowerLeft());
			gl::drawLine(r.getUpperRight(), r.getLowerRight());
			std::ostringstream oss;
			oss << o.id;
			gl::drawString(oss.str(), r.getUpperLeft());
		}
		
		for(objet_store::iterator it = store_.begin(); it != store_.end(); ++it) {
			objet& o = *(*it);
			Vec2f position = Vec2f(o.x() * getWindowWidth(), o.y() * getWindowHeight());
			gl::color(Color((o.present() ? 1.0 : 0.5) * 1.0 * (o.id() % 2 == 0), (o.present() ? 1.0 : 0.5) * 1.0 * (o.id() % 3 == 0), (o.present() ? 1.0 : 0.5) * 1.0 * (o.id() % 2 == 1)));
			// gl::drawSolidRect(Rectf(o.rect().x1 * getWindowWidth(), o.rect().y1 * getWindowHeight(), o.rect().x2 * getWindowWidth(), o.rect().y2 * getWindowHeight()));
			if(o.present()) {
				Rectf r(o.rect().x1 * getWindowWidth(), o.rect().y1 * getWindowHeight(), o.rect().x2 * getWindowWidth(), o.rect().y2 * getWindowHeight());
				gl::drawLine(r.getUpperLeft(), r.getUpperRight());
				gl::drawLine(r.getLowerLeft(), r.getLowerRight());
				gl::drawLine(r.getUpperLeft(), r.getLowerLeft());
				gl::drawLine(r.getUpperRight(), r.getLowerRight());
			}
			gl::drawSolidCircle(position, 3);
			gl::color(Color(1.0, 1.0, 1.0));
			if(o.present()) {
				std::ostringstream oss;
				oss << o.id();
				if(o.rect().id > 0)
					oss << " (" << o.rect().id << ")";
				gl::drawString(oss.str(), position);
			}
		}
										
		gl::color(Color(1.0, 1.0, 1.0));
		gl::drawString(oss.str(), Vec2f(10, 10));
		gl::drawString(last_msg_, Vec2f(10, 20));
	}
	
	void prepareSettings(Settings* settings) {
		settings->setWindowSize(640, 480);
		settings->setFrameRate(60.f);
		settings->setTitle("Coucou");
	}
	void keyUp(KeyEvent e) {
		if(e.getChar()=='a' && s_ > 0)
			--s_;
		if(e.getChar()=='z' && s_ < 11)
			++s_;
		if(e.getChar()=='s') {
			if(!reactivision_) {
				last_msg_ = "Reactivision";
				reactivision_ = true;
			} else {
				last_msg_ = "ARToolkitplus";
				reactivision_ = false;
			}
		}	
		if(e.getChar()=='i') {
			if(!which_image_) {
				which_image_ = true;
			} else {
				which_image_ = false;
			}
		}
			
	}
private:	
	objet_store store_;
	tuio_demux tuio_;
	// liveosc_receive liveosc_;
	std::string last_msg_;
	objet_alpha* alphi_;
	comportement* comp_;
	kinect kinect_;
	kinectivision k_;
	kinect_segmentor k2_;
	arkinectoolkitplus k3_;
	gl::Texture image_cam1_, image_cam2_;
	bool reactivision_, which_image_;
	int s_;
};

CINDER_APP_BASIC(affiche_objets, RendererGl)