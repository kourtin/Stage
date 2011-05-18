#include "comportements/note_slider.h"

#include <ctime>
#include <iostream>

note_slider::note_slider(objet& o) : comportement_ctrl(o) {
	// set_source(&karplus_);
}

void note_slider::draw(ci::cairo::Context ctx, int w, int h) {
	comportement_ctrl::draw(ctx, w, h);
}

void note_slider::operator()() {
	comportement_ctrl::operator()();
	if(objet_associe() && objet_associe()->est_attache()) {
		parametre* p = objet_associe()->comportement_attache()->parametres()->get_param("note");
		if(p) {
			double note = p->min() + objet_attache().y() * (p->max() - p->min());
			p->set(note);
		}  
	}
}

void note_slider::maj_objet_associe(objet* ancien) {
	if(!parametres()) return;
	parametre* p = 0;
	parametre* pancien = 0;
	if(objet_associe() && objet_associe()->est_attache())
		p = objet_associe()->comportement_attache()->parametres()->get_param("note");
	if(ancien && ancien->est_attache())
		pancien = ancien->comportement_attache()->parametres()->get_param("note");
		
	if(pancien)
		parametres()->remove(pancien);
	if(p)
		parametres()->add(p);
}