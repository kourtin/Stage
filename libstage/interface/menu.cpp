#include "interface/menu.h"
#include "comportements/comportement.h"
#include <iostream>

menu::menu(std::string titre) : selection_(liste_.end()), titre_(titre) {}

menu::~menu() {
	for(iterator it = liste_.begin(); it != liste_.end(); ++it) {
		if(*it)
			delete *it;
	}
}

void menu::select(unsigned int x) {
	iterator it = begin();
	while(x > 0 && it != end()) {
		++it;
		--x;
	}
	selection_ = it;
}

menu::iterator menu::iterator_at(unsigned int x) {
	iterator it = begin();
	while(x > 0 && it != end()) {
		++it;
		--x;
	}
	return it;
}

void menu::confirmer(objet& o) {
	if(selection_ != liste_.end()) {
		element_menu* ep = *selection();
		if(ep) {
			ep->operator()(o);
		} else {
			std::cout << "erreur menu::confirmer" << std::endl;
		}
	}
}

void elt_detruire::operator()(objet& o) {
	comportement* c = o.comportement_attache();
	o.detacher();
	delete c;
}

#include "comportements/oscillathor.h"
#include "comportements/corde_guitare.h"
#include "comportements/marteau.h"
#include "comportements/tete_lecture.h"
#include "comportements/variateur.h"
#include "comportements/delayman.h"
#include "comportements/reeverb.h"

menu_affectation_objet::menu_affectation_objet() : menu("Affecter cet objet à") {
	liste_.push_back(new elt_annuler);
	supporter_comportement<corde_guitare>();
	// liste_.push_back(new elt_annuler);
	supporter_comportement<marteau>();
	// liste_.push_back(new elt_annuler);
	supporter_comportement<oscillathor>();
	// liste_.push_back(new elt_annuler);
	supporter_comportement<micro>();
	supporter_comportement<tete_lecture>();
	supporter_comportement<delayman>();
	supporter_comportement<reeverb>();
	
	supporter_comportement<variateur<parametre_note> >();
	supporter_comportement<variateur<parametre_frequence> >();
	selection_ = liste_.end();
}

menu_edition_objet::menu_edition_objet() : menu("Actions sur cet objet") {
	liste_.push_back(new elt_annuler);
	liste_.push_back(new elt_detruire);
	selection_ = liste_.end();
}