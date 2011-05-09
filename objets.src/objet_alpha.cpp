#include "objet_alpha.h"
#include "oscillathor.h"

objet_alpha::objet_alpha(objet& o) : comportement(o), etat_(etat_initial), objet_selectionne_(0), temporisation_(-1) {}
void objet_alpha::draw(ci::cairo::Context ctx, int w, int h) {
	comportement::draw(ctx, w, h);
	if(etat_ == etat_selection_confirmee && !objet_selectionne_->est_attache()) {
		ctx.setSourceRgba(0, 0, 0, 0.5);
		ctx.rectangle(0, 50, w, 200);
		ctx.setLineWidth(0);
		ctx.fill();
		ctx.setFont(fnt_ttl_);
		ctx.setFontSize(13);
		ctx.setSourceRgb(1, 1, 1);
		ctx.moveTo(5, 60);
		ctx.showText("Cet objet sera :");
		ctx.stroke();
		
		ctx.setFontSize(10);
		ctx.setSourceRgb(1, 1, 0.6);
		ctx.moveTo(15, 80);
		ctx.showText("oscillathor");
		ctx.stroke();
		ctx.setLineWidth(2);
		ctx.rectangle(5, 70, w - 10, 10);
		ctx.stroke();
	} else if(etat_ == etat_selection_confirmee && objet_selectionne_->est_attache()) {
		ctx.setSourceRgba(0, 0, 0, 0.5);
		ctx.rectangle(0, 50, w, 200);
		ctx.setLineWidth(0);
		ctx.fill();
		ctx.setFont(fnt_ttl_);
		ctx.setFontSize(13);
		ctx.setSourceRgb(1, 1, 1);
		ctx.moveTo(5, 60);
		ctx.showText("Actions sur cet objet :");
		ctx.stroke();

		ctx.setFontSize(10);
		ctx.setSourceRgb(1, 1, 0.6);
		ctx.moveTo(15, 80);
		ctx.showText("Detruire");
		ctx.stroke();
		ctx.setLineWidth(2);
		ctx.rectangle(5, 70, w - 10, 10);
		ctx.stroke();
	}
}

void objet_alpha::operator()() {
	if(etat_ == etat_initial) {
		// objet_selectionne_ = 0;
		// Vérifie si une sélection est possible
		for(objet_store::iterator it = objet_attache().store().begin(); it != objet_attache().store().end(); ++it)
			if(*it != &objet_attache() && (*it)->collision(objet_attache())) {
				objet_selectionne_ = *it;
				etat_ = etat_selection_possible;
				temporisation_ = temps_actuel();
				std::cout << "0 initial >> 1 selection possible " << (*it)->distance(objet_attache()) << std::endl;
				break;
			}
	} else if(etat_ == etat_selection_possible) {
		// Vérifie que l'objet alpha est toujours sur la sélection pressentie
		// Sinon, on annule
		if(!objet_selectionne_ || !objet_selectionne_->collision(objet_attache())) {
			objet_selectionne_ = 0;
			etat_ = etat_initial;
			temporisation_ = -1;
			std::cout << "1 selection possible >> 0 initial" << std::endl;
		}
		// Une attente de X ms est suffisante pour confirmer la sélection
		else if(temps_actuel() - temporisation_ >= 1200) {
			etat_ = etat_selection_confirmee;
			temporisation_ = -1;
			std::cout << "1 selection possible >> 2 selection confirmee" << std::endl;
		}
	} else if(etat_ == etat_selection_confirmee) {
		// Vérifie que l'objet alpha est toujours sur la sélection pressentie
		// Sinon, on passe à l'état "pourrait accepter"
		if(!objet_selectionne_ || !objet_selectionne_->collision(objet_attache())) {
			etat_ = etat_pourrait_accepter;
			temporisation_ = temps_actuel();
			std::cout << "2 selection confirmee >> 3 pourrait accepter" << std::endl;
		}
		// La rotation de l'objet alpha définit ce qui est sélectionné dans le menu
		// TODO: rotation
	} else if(etat_ == etat_pourrait_accepter) {	
		// Si l'attente est supérieure à X ms, on annule
		if(temps_actuel() - temporisation_ >= 700) {
			etat_ = etat_initial;
			temporisation_ = -1;
			std::cout << "3 pourrait accepter >> 0 initial" << std::endl;
			return;
		}
		// Attend une nouvelle collision avec l'objet sélectionné
		else if(objet_selectionne_ && objet_selectionne_->collision(objet_attache())) {
			etat_ = etat_presque_accepte;
			temporisation_ = -1;
			std::cout << "3 pourrait accepter >> 4 presque accepte" << std::endl;
		}
	} else if(etat_ == etat_presque_accepte) {
		// Attend que l'objet alpha s'en aille de l'objet sélectionné
		if(objet_selectionne_ && !objet_selectionne_->collision(objet_attache())) {
			etat_ = etat_accepte;
			std::cout << "5 presque accepte >> 6 accepte" << std::endl;
		}
	} else if(etat_ == etat_accepte) {
		etat_ = etat_initial;
		// Fait un truc
		std::cout << "Bip bip" << std::endl;
		std::cout << "6 accepte >> 0 initial" << std::endl;
		if(!objet_selectionne_->est_attache()) {
			new oscillathor(*objet_selectionne_);
		} else {
			comportement* c = objet_selectionne_->comportement_attache();
			objet_selectionne_->detacher();
			delete c;
		}
		// objet_selectionne_->attacher(*c);
	}
}