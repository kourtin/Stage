#include <opportuniste.h>
#include <comportements/comportement.h>
#include <interface/objet_alpha.h>
#include "CinderOpenCv.h"
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cinder/app/AppBasic.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/ImageIo.h>
#include <cinder/cairo/Cairo.h>

using namespace ci;
using namespace ci::app;

struct affiche_objets : public AppBasic {
	affiche_objets() : render_to_(640, 480, true), alphi_(0) {}
	
	virtual ~affiche_objets() {
		if(alphi_)
			delete alphi_;
	}
	
	void setup() {
		image_cam_ = gl::Texture(render_to_.getSurface());
		rndring_ = gl::Texture(render_to_.getSurface());
		gl::enableAlphaBlending(true);
		
		controle_.activer_camera(true);
		controle_.activer_sonde_artkplus(true);
	}
	
	void update() {
		controle_.update();
		image_cam_.update(controle_.img());
		if(controle_.store().objets().size() >= 1 && alphi_ == 0 && (*(*(controle_.store().begin()))).id() > 0) {
			alphi_ = new objet_alpha(*(*(controle_.store().begin())));
		}
	}
	
	void draw() {
		cairo::Context ctx(render_to_);
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
			oss << fps << " fps";
		}
		
		ctx.save();
		ctx.setSourceRgba(0, 0, 0, 0);
		ctx.setOperator(1);
		ctx.paint();
		ctx.restore();
		
		ctx.setSourceRgba(0,0,0,0.5);
		ctx.paint();
		
		for(objet_store::iterator it = controle_.store().begin(); it != controle_.store().end(); ++it) {
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
			
			// std::ostringstream oss2;
			// 			oss2 << o.rect_abs();
			// 			ctx.moveTo(position_c);
			// 			ctx.showText(oss2.str());
			// 			ctx.stroke();
			
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
		
		ctx.setFontSize(12);				
		ctx.setSource(Color(1.0, 1.0, 1.0));
		ctx.moveTo(Vec2f(10, 10));
		ctx.showText(oss.str());
		ctx.stroke();
		
		rndring_.update(render_to_.getSurface());
		gl::clear(Color(0.f, 0.f, 0.f));
		gl::draw(image_cam_, Vec2f(0,0));
		gl::draw(rndring_, Vec2f(0,0));
	}
	
	void prepareSettings(Settings* settings) {
		settings->setWindowSize(640, 480);
		settings->setFrameRate(25.f);
		settings->setTitle("Tritrilipompon");
	}
	void keyUp(KeyEvent e) {
		// if(e.getChar()=='a' && s_ > 0)
		// 			--s_;
		// 		if(e.getChar()=='z' && s_ < 11)
		// 			++s_;
		// 		if(e.getChar()=='s') {
		// 			if(!reactivision_) {
		// 				last_msg_ = "Reactivision";
		// 				reactivision_ = true;
		// 			} else {
		// 				last_msg_ = "ARToolkitplus";
		// 				reactivision_ = false;
		// 			}
		// 		}	
		// 		if(e.getChar()=='i') {
		// 			if(!which_image_) {
		// 				which_image_ = true;
		// 			} else {
		// 				which_image_ = false;
		// 			}
		// 		}
		// TODO: faire quitter	
	}
private:		
	opportuniste controle_;
	gl::Texture image_cam_, rndring_;
	ci::cairo::SurfaceImage render_to_;
	comportement* alphi_;
};

CINDER_APP_BASIC(affiche_objets, RendererGl(0))