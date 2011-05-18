#include "comportements/tete_lecture.h"

struct pred_pas_x {
	pred_pas_x(objet& o) : o_(o) {}
	bool operator()(objet* x) {
		return !x || std::abs(o_.x() - (*x).x()) >= (*x).rect().getWidth() / 2;
	}
private:
	objet& o_;
};

tete_lecture::tete_lecture(objet& o) : comportement_tool(o) {}

void tete_lecture::draw(ci::cairo::Context ctx, int w, int h) {
	comportement_tool::draw(ctx, w, h);
	
	// Dessine la ligne de la tete de lecture
	ctx.setSourceRgba(0.5, 1.0, 0.5, 0.7);
	ctx.moveTo(objet_attache().x() * w, 0);
	ctx.lineTo(objet_attache().x() * w, h);
	ctx.stroke();
}
#include <iostream>
void tete_lecture::operator()() {
	if(!objet_attache().store())
		return;
	
	// S'il n'y a plus de collision pour certains objets, le signaler
	deja_en_collision_.remove_if(pred_pas_x(objet_attache()));
	// for(liste_objets::iterator it = deja_en_collision_.begin(); it != deja_en_collision_.end();) {
	// 	if(!objet_attache().collision(*(*it)))
	// 		deja_en_collision_.remove(it);
	// 	else
	// 		++it;
	// }
	
	for(objet_store::iterator it = objet_attache().store()->begin(); it != objet_attache().store()->end(); ++it) {
		// Est-on sur la même colonne que l'objet ?
		if((*it) != &objet_attache() && std::abs((*it)->x() - objet_attache().x()) <= (*it)->rect().getWidth() / 2) {
			// L'objet est-il associé à un comportement ?
			if((*it)->est_attache()) {
				comportement* c = (*it)->comportement_attache();
				// N'est-on pas déjà en collision avec cet objet ?
				if(c->parametres() && std::find(deja_en_collision_.begin(), deja_en_collision_.end(), *it) == deja_en_collision_.end()) {
				// Nouvelle collision
				deja_en_collision_.push_back(*it);
				// BANG ! (s'il n'y a pas de bang, ça ne fera rien)
				c->parametres()->set("bang", 1);
				}
			}
		}
	}
}