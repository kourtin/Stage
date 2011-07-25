#include <opportuniste.h>
#include <comportements/comportement.h>
#include <interface/objet_alpha2.h>
#include <interface/shake_menu.h>
#include "CinderOpenCv.h"
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <numeric>

#include <cinder/app/AppBasic.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/ImageIo.h>
#include <cinder/cairo/Cairo.h>

using namespace ci;
using namespace ci::app;

struct affiche_objets : public AppBasic {
	affiche_objets() : render_to_(640, 480, true), alphi_(0), fnt_("Cabin", 10), image_(true) {}
	
	virtual ~affiche_objets() {
		if(alphi_)
			delete alphi_;
	}
	
	void setup() {
		image_cam_ = gl::Texture(render_to_.getSurface());
		rndring_ = gl::Texture(render_to_.getSurface());
		gl::enableAlphaBlending(true);
		
		controle_.activer_camera(true);
		// controle_.activer_kinect(true);
		controle_.activer_sonde_artkplus(true);
		// controle_.activer_sonde_kinectsegmentor(true);
		// controle_.activer_sonde_tuio(true);
	}
	
	void update() {
		controle_.update();
		if(controle_.camera_active() || controle_.kinect_active())
			image_cam_.update(controle_.img());
		// if(controle_.store().objets().size() >= 1 && alphi_ == 0 && (*(*(controle_.store().begin()))).id() > 0) {
					// alphi_ = new objet_alpha(*(*(controle_.store().begin())));
				// }
		// static float adpos[3];
		// 		static float apos[3];
		// 		static float asigne = 0;
		// 		static int nboppose = 0;
		// 		static double tps_act = std::clock();
		// 		float dpos[3];
		// 		if(controle_.store().objets().size() >= 1) {
		// 			objet& o = *(*(controle_.store().begin()));
		// 			
		// 			dpos[0] = o.x() - apos[0];
		// 			dpos[1] = o.y() - apos[1];
		// 			dpos[2] = o.z() - apos[2];
		// 			
		// 			float distance = sqrt(dpos[0]*dpos[0] + dpos[1]*dpos[1] + dpos[2]*dpos[2]);
		// 			
		// 			// std::cout << "d=" << distance << " pour (" << dpos[0] << "," << dpos[1] << "," << dpos[2] << ")" << std::endl;
		// 			bool ok = false;
		// 			if(nboppose > 8) {
		// 				ok = true;
		// 				std::cout << "PROUUUUUUUUUUUUUUUUUT." << std::endl;
		// 			}
		// 			if(ok || std::clock() - tps_act > 1.5 * CLOCKS_PER_SEC) {
		// 				tps_act = std::clock();
		// 				nboppose = 0;
		// 				asigne = 0;
		// 				ok = false;
		// 				std::cout << "reset" << std::endl;
		// 			}
		// 			
		// 			// Seuille
		// 			if(distance > 0.01) {
		// 				double prodscal = dpos[0] * adpos[0] + dpos[1] * adpos[1] + dpos[2] * adpos[2];//std::inner_product(dpos, dpos + 3, adpos, 0);
		// 
		// 				double signe = (prodscal>=0 ? 1:-1);
		// 
		// 				if(asigne == 0)
		// 					asigne = signe;
		// 
		// 				if(asigne != signe)
		// 					std::cout << "mouvement oppose " << (++nboppose) << std::endl;
		// 				
		// 				asigne = signe;
		// 			}
		// 			
		// 			// Maj positions
		// 			apos[0] = o.x();
		// 			apos[1] = o.y();
		// 			apos[2] = o.z();
		// 			adpos[0] = dpos[0];
		// 			adpos[1] = dpos[1];
		// 			adpos[2] = dpos[2];
		// 		}
		
		for(objet_store::iterator it = controle_.store().begin(); !alphi_ && it != controle_.store().end(); ++it) {
			// std::cout << (*it)->id() << std::endl;
			// if((*it)->id() == 630/*774*//*1208*//*35*//**/)
				// alphi_ = new objet_alpha2(*(*it));
			if(!(*it)->est_vraiment_attache())
				new shake_comportement(*(*it));
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

		ctx.setFont(fnt_);
		
		for(objet_store::iterator it = controle_.store().begin(); it != controle_.store().end(); ++it) {
			objet& o = *(*it);
			Vec2f position = /*world::to_screen(o);*/Vec2f(o.x() * getWindowWidth(), o.y() * getWindowHeight());
			Vec2f position_c = /*world::to_screen(o, true);*/Vec2f(o.x_c() * getWindowWidth(), o.y_c() * getWindowHeight());
			
			// ctx.setSource(ColorA(1.0 * (o.id() % 2 == 0), 1.0 * (o.id() % 3 == 0), 1.0 * (o.id() % 2 == 1), 0.3));
			ctx.setSourceRgba(
				o.couleur().r * (1-o.zombie_percent()),
				o.couleur().g * (1-o.zombie_percent()),
				o.couleur().b * (1-o.zombie_percent()),
				0.3 + 0.4 * (1-o.absent_percent())
				);
			// if(o.zombie())
				// ctx.setSourceRgba(0.2, 0.2, 0.2, 0.2);
			// else
				// ctx.setSource(ColorA(o.couleur(), (o.present() ? 0.7 : 0.3)));
			ctx.circle(position_c, 2);
			ctx.fill();
			ctx.setLineWidth(1);
			ctx.line(position, position_c);
			ctx.stroke();
			// ctx.setSourceRgba(1, 1, 1, 0.7);
			// ctx.setLineWidth(3);
			// ctx.line(position, Vec2f(position.x + 20*cos(o.r()), position.y + 20* sin(o.r())));
			// ctx.stroke();
			
			// ctx.setSource(ColorA(1.0 * (o.id() % 2 == 0), 1.0 * (o.id() % 3 == 0), 1.0 *(o.id() % 2 == 1), (o.present() ? 0.9 : 0.3)));
			// if(o.present()) {
			Rectf r = o.rect_abs_scaled(Vec2f(getWindowWidth(), getWindowHeight()));
			ctx.setLineWidth(1);
			ctx.rectangle(r);
			ctx.stroke();
			// }
			ctx.circle(position, 5);
			ctx.fill();
			
			ctx.setSource(Color(1.0, 1.0, 1.0));
			if(o.est_vraiment_attache()) {
				ctx.save();
				o.comportement_attache()->draw(ctx, getWindowWidth(), getWindowHeight());
				ctx.restore();
			}
		}
		ctx.setFontSize(12);				
		ctx.setSource(Color(1.0, 1.0, 1.0));
		ctx.moveTo(Vec2f(10, 10));
		ctx.showText(oss.str());
		ctx.stroke();
		
		rndring_.update(render_to_.getSurface());
		gl::clear(Color(0.f, 0.f, 0.f));
		if(image_)
			gl::draw(image_cam_, Vec2f(0,0));
		gl::draw(rndring_, Vec2f(0,0));
	}
	
	void prepareSettings(Settings* settings) {
		settings->setWindowSize(640, 480);
		settings->setFrameRate(30.f);
		settings->setTitle("Tritrilipompon");
	}
	void keyUp(KeyEvent e) {
		if(e.getChar()=='k')
			image_ = !image_;
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
	ci::Font fnt_;
	bool image_;
};

CINDER_APP_BASIC(affiche_objets, RendererGl(0))