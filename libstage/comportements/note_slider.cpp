#include "comportements/note_slider.h"

#include <ctime>
#include <iostream>

note_slider::note_slider(objet& o) : comportement_ctrl(o) {
	// set_source(&karplus_);
}

void note_slider::draw(ci::cairo::Context ctx, int w, int h) {
	comportement::draw(ctx, w, h);
}

void note_slider::operator()() {
	// // std::cout << "caca" << std::endl;
	// double note = karplus_.note().min() + objet_attache().x() * (karplus_.note().max() - karplus_.note().min());
	// karplus_.note().set(note);
	// 
	// if(!objet_attache().store())
	// 	return;
	// 
	// // std::cout << "caca2" << std::endl;
	// static objet* deja_en_collision = 0;
	// 
	// objet* collision = 0;
	// for(objet_store::iterator it = objet_attache().store()->begin(); it != objet_attache().store()->end() && !collision; ++it) {
	// 	if(objet_attache().collision(*(*it)))
	// 		collision = *it;
	// }
	// 
	// if(collision != deja_en_collision) {
	// 	if(deja_en_collision && collision)
	// 		karplus_.bang();
	// 	deja_en_collision = collision;
	// }
}