#include "comportements/variateur.h"

#include <ctime>
#include <iostream>

variateur_parametre::variateur_parametre(objet& o, std::string parametre) : comportement_ctrl(o), parametre_(parametre) {}

void variateur_parametre::draw(ci::cairo::Context ctx, int w, int h) {
	comportement_ctrl::draw(ctx, w, h);
}

void variateur_parametre::operator()() {
	comportement_ctrl::operator()();
	if(objet_associe() && objet_associe()->est_attache()) {
		parametre* p = objet_associe()->comportement_attache()->parametres()->get_param(parametre_);
		if(p) {
			double note = p->min() + objet_attache().y() * (p->max() - p->min());
			p->set(note);
		}  
	}
}

void variateur_parametre::maj_objet_associe(objet* ancien) {
	if(!parametres()) return;
	parametre* p = 0;
	parametre* pancien = 0;
	if(objet_associe() && objet_associe()->est_attache())
		p = objet_associe()->comportement_attache()->parametres()->get_param(parametre_);
	if(ancien && ancien->est_attache())
		pancien = ancien->comportement_attache()->parametres()->get_param(parametre_);
		
	if(pancien)
		parametres()->remove(pancien);
	if(p)
		parametres()->add(p);
}