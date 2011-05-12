#include "comportements/corde_guitare.h"

#include <ctime>
#include <iostream>

corde_guitare::corde_guitare(objet& o) : comportement_source(o) {
	set_source(&karplus_);
}

void corde_guitare::draw(ci::cairo::Context ctx, int w, int h) {
	comportement_source::draw(ctx, w, h);
	
	// ctx.setFont(fnt_param_);
	
	
	// ctx.setLineWidth(1);
	// ctx.setSourceRgba(1,1,1,0.5);
	// ctx.line(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h), ci::Vec2f(objet_attache().x() * w - 15, objet_attache().y() * h));
	// ctx.stroke();
}

void corde_guitare::operator()() {
	// std::cout << "caca" << std::endl;
	double note = karplus_.note().min() + objet_attache().x() * (karplus_.note().max() - karplus_.note().min());
	karplus_.note().set(note);
	
	// if(!objet_attache().store())
	// 		return;
	// 	
	// 	// std::cout << "caca2" << std::endl;
	// 	static objet* deja_en_collision = 0;
	// 	
	// 	objet* collision = 0;
	// 	for(objet_store::iterator it = objet_attache().store()->begin(); it != objet_attache().store()->end() && !collision; ++it) {
	// 		if(objet_attache().collision(*(*it)))
	// 			collision = *it;
	// 	}
	// 	
	// 	if(collision != deja_en_collision) {
	// 		if(deja_en_collision && collision)
	// 			karplus_.bang();
	// 		deja_en_collision = collision;
	// 	}
}