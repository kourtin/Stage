#include "comportements/marteau.h"

struct pred_pas_collision {
	pred_pas_collision(objet& o) : o_(o) {}
	bool operator()(objet* x) {
		return !x || !o_.collision(*x);
	}
private:
	objet& o_;
};

marteau::marteau(objet& o) : comportement_tool(o) {}

void marteau::draw(ci::cairo::Context ctx, int w, int h) {
	comportement_tool::draw(ctx, w, h);
}
#include <iostream>
void marteau::operator()() {
	if(!objet_attache().store())
		return;
	
	// S'il n'y a plus de collision pour certains objets, le signaler
	deja_en_collision_.remove_if(pred_pas_collision(objet_attache()));
	// for(liste_objets::iterator it = deja_en_collision_.begin(); it != deja_en_collision_.end();) {
	// 	if(!objet_attache().collision(*(*it)))
	// 		deja_en_collision_.remove(it);
	// 	else
	// 		++it;
	// }
	
	for(objet_store::iterator it = objet_attache().store()->begin(); it != objet_attache().store()->end(); ++it) {
		// Est-on en collision avec l'objet ?
		if((*it) != &objet_attache() && objet_attache().collision(*(*it))) {
			// L'objet est-il associé à un comportement ?
			if((*it)->est_attache()) {
				comportement* c = (*it)->comportement_attache();
					std::cout << "comportementattache" << c->what() << std::endl;
				// Ce comportement est-il de type source ?
				if(c->what() == comportement::type_source) {
					std::cout << "typesource" << std::endl;
					comportement_source* cs = dynamic_cast<comportement_source*>(c);
					// N'est-on pas déjà en collision avec cet objet ?
					if(cs && std::find(deja_en_collision_.begin(), deja_en_collision_.end(), *it) == deja_en_collision_.end()) {
						std::cout << "BANG" << std::endl;
						// Nouvelle collision
						deja_en_collision_.push_back(*it);
						// BANG ! (s'il n'y a pas de bang, ça ne fera rien)
						cs->parametres()->set("bang", 1);
					}
				}
			}
		}
	}
}