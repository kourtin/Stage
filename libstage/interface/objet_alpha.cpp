#include "interface/objet_alpha.h"
#include "comportements/oscillathor.h"
#include "comportements/corde_guitare.h"
#include "comportements/marteau.h"
#include "comportements/tete_lecture.h"
#include "comportements/note_slider.h"

menu_affectation_objet::menu_affectation_objet() : menu("Affecter cet objet à") {
	// supporter_comportement<oscillathor>();
	// supporter_comportement<micro>();
	supporter_comportement<corde_guitare>();
	supporter_comportement<marteau>();
	// supporter_comportement<tete_lecture>();
	supporter_comportement<note_slider>();
}

struct elt_detruire : public element_menu {
	std::string titre() { return "Detruire"; }
	void operator()(objet& o) {
		comportement* c = o.comportement_attache();
		o.detacher();
		delete c;
	}
};

menu_edition_objet::menu_edition_objet() : menu("Actions sur cet objet") {
	liste_.push_back(new elt_detruire);
}

objet_alpha::objet_alpha(objet& o) : comportement(o), etat_(etat_initial), objet_selectionne_(0), temporisation_(-1), menu_selectionne_(0), mnu_drw_(new menu_draw_circulaire(menu_selectionne_)) {
	dynamic_cast<menu_draw_circulaire*>(mnu_drw_)->attacher_a(&o);
	std::cout << "objetalpha: " << o.id() << std::endl;
}

objet_alpha::~objet_alpha() {
	if(mnu_drw_)
		delete mnu_drw_;
}

void objet_alpha::draw(ci::cairo::Context ctx, int w, int h) {
	comportement::draw(ctx, w, h);
	if(etat_ != etat_initial) {
		ctx.setSourceRgba(1.0f, 0.7f, 0.3f, 0.5);
		ctx.circle(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h), 10);
		ctx.setLineWidth(4);
		ctx.fill();
	}
	if(etat_ == etat_selection_confirmee && mnu_drw_) {
		mnu_drw_->draw(ctx, w, h);
	}
}
#include <iostream>
void objet_alpha::operator()() {
	if(etat_ == etat_initial) {
		// Vérifie si une sélection est possible
		objet* obj_collision = objet_attache().objet_en_collision();
		if(obj_collision) {
			objet_selectionne_ = obj_collision;
			etat_ = etat_selection_possible;
			temporisation_ = temps_actuel();
			std::cout << "0 initial >> 1 selection possible" << std::endl;
		}
	} else if(etat_ == etat_selection_possible) {
		// Vérifie que l'objet alpha est toujours sur la sélection pressentie
		// Sinon, on annule
		if(!objet_selectionne_ || !objet_attache().collision(*objet_selectionne_)) {
			objet_selectionne_ = 0;
			etat_ = etat_initial;
			temporisation_ = -1;
			std::cout << "1 selection possible >> 0 initial" << std::endl;
		}
		// Une attente de X ms est suffisante pour confirmer la sélection
		else if(temps_actuel() - temporisation_ >= 900) {
			std::cout << "1 selection possible >> 2 selection confirmee" << std::endl;
			etat_ = etat_selection_confirmee;
			temporisation_ = -1;
			if(!objet_selectionne_->est_attache())
				menu_selectionne_ = &menu_affectation_objet::instance();
			else
				menu_selectionne_ = &mnu_ed_;
			
			if(mnu_drw_)
				mnu_drw_->change(menu_selectionne_);
			
			if(menu_affectation_objet::instance().size() > 0)
				menu_affectation_objet::instance().select(menu_affectation_objet::instance().begin());
		}
	} else if(etat_ == etat_selection_confirmee) {
		// Vérifie que l'objet alpha est toujours sur la sélection pressentie
		// Sinon, on passe à l'état "pourrait accepter"
		if(!objet_selectionne_ || !objet_attache().collision(*objet_selectionne_)) {
			etat_ = etat_pourrait_accepter;
			temporisation_ = temps_actuel();
			std::cout << "2 selection confirmee >> 3 pourrait accepter" << std::endl;
		}
		if(menu_selectionne_) {
			// La rotation de l'objet alpha définit ce qui est sélectionné dans le menu
			float r = objet_attache().r();
			float pourcentage = r / 2 / M_PI;
			unsigned int selection = pourcentage * menu_selectionne_->size();
			// std::cout << "pourcentage: " << pourcentage << std::endl;
			// std::cout << "selection: " << selection << std::endl;
			menu::iterator sel = menu_selectionne_->begin();
			while(selection > 0 && sel != menu_selectionne_->end()) {
				++sel;
				--selection;
			}
			// std::cout << "selected: " /*<< (*sel)->titre() << "; "*/ << (sel == menu_selectionne_->end()) << "/ " << (sel == menu_selectionne_->begin()) << std::endl;
			menu_selectionne_->select(sel);
		}
	} else if(etat_ == etat_pourrait_accepter) {	
		// Si l'attente est supérieure à X ms, on annule
		if(temps_actuel() - temporisation_ >= 500) {
			etat_ = etat_initial;
			temporisation_ = -1;
			std::cout << "3 pourrait accepter >> 0 initial" << std::endl;
			return;
		}
		// Attend une nouvelle collision avec l'objet sélectionné
		else if(objet_selectionne_ && objet_attache().collision(*objet_selectionne_)) {
			etat_ = etat_presque_accepte;
			temporisation_ = -1;
			std::cout << "3 pourrait accepter >> 4 presque accepte" << std::endl;
		}
	} else if(etat_ == etat_presque_accepte) {
		// Attend que l'objet alpha s'en aille de l'objet sélectionné
		if(objet_selectionne_ && !objet_attache().collision(*objet_selectionne_)) {
			etat_ = etat_accepte;
			std::cout << "5 presque accepte >> 6 accepte" << std::endl;
		}
	} else if(etat_ == etat_accepte) {
		etat_ = etat_initial;
		// Fait un truc
		if(menu_selectionne_)
			menu_selectionne_->confirmer(*objet_selectionne_);
	}
}