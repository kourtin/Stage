#if !defined(COMPORTEMENT_H_U8D66Y9P)
#define COMPORTEMENT_H_U8D66Y9P

#include "objet.h"
#include "objet_store.h"
#include <ctime>
#include <iostream>

double temps_actuel() { 
	return std::clock() * 1000.0 / CLOCKS_PER_SEC;
}

struct comportement {
	comportement(objet& o) : objet_(o) {
		o.attacher(*this);
	}
	virtual ~comportement() {
		objet_.detacher(*this);
	}
	virtual void operator()() = 0; // action
	objet& objet_attache() { return objet_; }
private:
	objet& objet_;
};

struct objet_alpha : public comportement {
	objet_alpha(objet& o) : comportement(o), etat_(etat_initial), objet_selectionne_(0), temporisation_(-1) {}
	virtual void operator()() {
		if(etat_ == etat_initial) {
			// objet_selectionne_ = 0;
			// Vérifie si une sélection est possible
			for(objet_store::iterator it = objet_attache().store().begin(); it != objet_attache().store().end(); ++it)
				if(*it != &objet_attache() && (*it)->collision(objet_attache())) {
					objet_selectionne_ = *it;
					etat_ = etat_selection_possible;
					temporisation_ = temps_actuel();
					// std::cout << "0 initial >> 1 selection possible " << it->distance(objet_attache()) << std::endl;
					break;
				}
		} else if(etat_ == etat_selection_possible) {
			// Vérifie que l'objet alpha est toujours sur la sélection pressentie
			// Sinon, on annule
			if(!objet_selectionne_ || !objet_selectionne_->collision(objet_attache())) {
				objet_selectionne_ = 0;
				etat_ = etat_initial;
				temporisation_ = -1;
				// std::cout << "1 selection possible >> 0 initial" << std::endl;
			}
			// Une attente de X ms est suffisante pour confirmer la sélection
			else if(temps_actuel() - temporisation_ >= 150) {
				etat_ = etat_selection_confirmee;
				temporisation_ = -1;
				// std::cout << "1 selection possible >> 2 selection confirmee" << std::endl;
			}
		} else if(etat_ == etat_selection_confirmee) {
			// Vérifie que l'objet alpha est toujours sur la sélection pressentie
			// Sinon, on passe à l'état "pourrait accepter"
			if(!objet_selectionne_ || !objet_selectionne_->collision(objet_attache())) {
				etat_ = etat_pourrait_accepter;
				temporisation_ = temps_actuel();
				// std::cout << "2 selection confirmee >> 3 pourrait accepter" << std::endl;
			}
			// La rotation de l'objet alpha définit ce qui est sélectionné dans le menu
			// TODO: rotation
		} else if(etat_ == etat_pourrait_accepter) {	
			// Si l'attente est supérieure à X ms, on annule
			if(temps_actuel() - temporisation_ >= 100) {
				etat_ = etat_initial;
				temporisation_ = -1;
				// std::cout << "3 pourrait accepter >> 0 initial" << std::endl;
				return;
			}
			// Attend une nouvelle collision avec l'objet sélectionné
			else if(objet_selectionne_ && objet_selectionne_->collision(objet_attache())) {
				etat_ = etat_presque_accepte;
				temporisation_ = -1;
				// std::cout << "3 pourrait accepter >> 4 presque accepte" << std::endl;
			}
		} else if(etat_ == etat_presque_accepte) {
			// Attend que l'objet alpha s'en aille de l'objet sélectionné
			if(objet_selectionne_ && !objet_selectionne_->collision(objet_attache())) {
				etat_ = etat_accepte;
				// std::cout << "5 presque accepte >> 6 accepte" << std::endl;
			}
		} else if(etat_ == etat_accepte) {
			etat_ = etat_initial;
			// Fait un truc
			// std::cout << "Bip bip" << std::endl;
			// std::cout << "6 accepte >> 0 initial" << std::endl;
		}
	}
private:
	enum { etat_initial, etat_selection_possible, etat_selection_confirmee, etat_pourrait_accepter, etat_presque_accepte, etat_accepte };
	unsigned int etat_;
	objet* objet_selectionne_;
	double temporisation_;
	
};

// struct source : public comportement {
// 	source(objet& o) : comportement(o) {}
// 	virtual ~source() {}
// 	virtual void operator()() = 0;
// 	
// };
// 
// struct fx : public source {
// 	
// };
// 
// struct controleur : public comportement {
// 	controleur(source* s) : 
// };
// 
// 
// 
// struct sampler : public comportement {
// 	sampler(objet& o) : comportement(o) {}
// 	virtual ~sampler() {}
// 	
// private:
// 	
// };

#endif /* end of include guard: COMPORTEMENT_H_U8D66Y9P */
