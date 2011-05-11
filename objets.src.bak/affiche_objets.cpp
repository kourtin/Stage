#include "objet_alpha.h"
#include "comportement.h"
#include "liveosc.h"
#include "objet_store.h"
#include "objet.h"
#include "couplage_virtuel.h"
#include "tuio.h"
#include <osc/OscPrintReceivedElements.h>

#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cinder/app/AppBasic.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/ImageIo.h>
#include <cinder/cairo/Cairo.h>
#include <cinder/Capture.h>



using namespace ci;
using namespace ci::app;

#include "processus_sonores.h"

#include "kinectivision.h"
#include "kinect_segmentor.h"
#include "arkinectoolkitplus.h"
#include "openni.h"
#include "freenect.h"

#include "CinderOpenCv.h"

struct affiche_objets : public AppBasic {
	affiche_objets() : mat_img_(cv::Size(640, 480), CV_8UC3), mat_depth_(cv::Size(640, 480), CV_16UC1), store_(), tuio_(store_), last_msg_("ARToolkitplus"), alphi_(0), comp_(0), kinect_(new openni), k_(mat_img_), k2_(mat_depth_), k3_(mat_img_), render_to_(640, 480, true), fnt_("Lucida Grande", 16.f), reactivision_(false), which_image_(false), s_(0) {
		calculateur_couplage::init();
		if(kinect_)
			kinect_->init();
	}
	
	virtual ~affiche_objets() {
		if(alphi_)
			delete alphi_;
		if(comp_)
			delete comp_;
	}
	
	void setup() {
		srand(time(NULL));
		master_pd::instance();
		image_cam_ = gl::Texture(fromOcv(mat_img_));
		rndring_ = gl::Texture(render_to_.getSurface());
		gl::enableAlphaBlending(true);
	}
	
	void update() {
		for(objet_store::iterator it = store_.begin(); it != store_.end(); ++it) {
			if((*it)->est_attache())
				(*it)->comportement_attache()->operator()();
			(*it)->present(false);
		}
		if(kinect_) {
			kinect_->update();
			kinect_->img_to(mat_img_);
			image_cam_.update(fromOcv(mat_img_));
			kinect_->depth_to(mat_depth_);
		} else {
			static ci::Capture c(640, 480);
			if(!c.isCapturing()) {
				c.start();
			} else {
				if(c && c.checkNewFrame()) {
					mat_img_ = toOcv(c.getSurface());
					// std::copy(pipi.data, pipi.data + pipi.cols * pipi.rows * 3, mat_img_.data);
					image_cam_.update(c.getSurface());
				}
			}
		}
		if(reactivision_)
			k_(&store_);
		else
			k3_(&store_);
		if(kinect_)
			k2_(&store_);
		
		if(store_.objets().size() >= 1 && alphi_ == 0 && (*(*(store_.begin()))).id() > 0) {
			alphi_ = new objet_alpha(*(*(store_.begin())));
		}
	}
	
	void draw() {
		cairo::Context ctx( render_to_ );
		// 
		static float sec = getElapsedSeconds();
		static float fra = getElapsedFrames();
		// static float sec = std::clock() * 1.0 / CLOCKS_PER_SEC;
		// static float fra = 0;
		static float fps = 0;
		
		float seconds = /*std::clock() * 1.0 / CLOCKS_PER_SEC;*/getElapsedSeconds();
		float frames = getElapsedFrames();
		// ++fra;
		
		if(seconds - sec > 1) {
			fps = /*fra;*/frames - fra;
			fra = /*0;*/frames;
			sec = seconds;
		}
		
		std::ostringstream oss;
		if(seconds > 0) {
			oss << fps << " fps. Segment " << s_;
			oss << ". Objets: " << store_.objets().size();
		}
		
		ctx.save();
		ctx.setSourceRgba(0, 0, 0, 0);
		ctx.setOperator(1);
		ctx.paint();
		ctx.restore();
		
		ctx.setSourceRgba(0,0,0,0.5);
		ctx.paint();
		
		for(objet_store::iterator it = store_.begin(); it != store_.end(); ++it) {
			objet& o = *(*it);
			Vec2f position = Vec2f(o.x() * getWindowWidth(), o.y() * getWindowHeight());
			Vec2f position_c = Vec2f(o.x_c() * getWindowWidth(), o.y_c() * getWindowHeight());
			
			ctx.setSource(ColorA(1.0 * (o.id() % 2 == 0), 1.0 * (o.id() % 3 == 0), 1.0 * (o.id() % 2 == 1), 0.3));
			ctx.circle(position_c, 2);
			ctx.fill();
			ctx.setLineWidth(1);
			ctx.line(position, position_c);
			ctx.stroke();
			ctx.setSourceRgba(1, 1, 1, 0.7);
			ctx.setLineWidth(3);
			ctx.line(position, Vec2f(position.x + 20*cos(o.r()), position.y + 20* sin(o.r())));
			ctx.stroke();
			
			std::ostringstream oss2;
			oss2 << o.rect_abs();
			ctx.moveTo(position_c);
			ctx.showText(oss2.str());
			ctx.stroke();
			
			ctx.setSource(ColorA(1.0 * (o.id() % 2 == 0), 1.0 * (o.id() % 3 == 0), 1.0 *(o.id() % 2 == 1), (o.present() ? 0.9 : 0.3)));
			// if(o.present()) {
				Rectf r = o.rect_abs_scaled(Vec2f(getWindowWidth(), getWindowHeight()));
				ctx.setLineWidth(1);
				ctx.rectangle(r);
				ctx.stroke();
			// }
			ctx.circle(position, 5);
			ctx.fill();
			
			ctx.setSource(Color(1.0, 1.0, 1.0));
			if(o.est_attache())
				o.comportement_attache()->draw(ctx, getWindowWidth(), getWindowHeight());
		}
		
		ctx.setFont(fnt_);
		ctx.setFontSize(12);				
		ctx.setSource(Color(1.0, 1.0, 1.0));
		ctx.moveTo(Vec2f(10, 10));
		ctx.showText(oss.str());
		ctx.stroke();
		ctx.moveTo(Vec2f(10, 30));
		ctx.showText(last_msg_);
		ctx.stroke();
		
		rndring_.update(render_to_.getSurface());
		gl::clear(Color(0.f, 0.f, 0.f));
		gl::draw(image_cam_, Vec2f(0,0));
		gl::draw(rndring_, Vec2f(0,0));
		// gl::drawString(oss.str(), Vec2f(0, 0));
	}
	
	void prepareSettings(Settings* settings) {
		settings->setWindowSize(640, 480);
		settings->setFrameRate(25.f);
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
		// TODO: faire quitter	
	}
private:		
	cv::Mat mat_img_, mat_depth_;
	objet_store store_;
	tuio_demux tuio_;
	// liveosc_receive liveosc_;
	std::string last_msg_;
	objet_alpha* alphi_;
	comportement* comp_, *comp2_;
	kinect* kinect_;
	kinectivision k_;
	kinect_segmentor k2_;
	arkinectoolkitplus k3_;
	gl::Texture image_cam_, rndring_;
	ci::cairo::SurfaceImage render_to_;
	ci::Font fnt_;
	bool reactivision_, which_image_;
	int s_;
};

CINDER_APP_BASIC(affiche_objets, RendererGl(0))