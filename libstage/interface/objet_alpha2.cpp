#include "interface/objet_alpha2.h"
#include "temps.h"

float entre0et1(float x) {
	while(x < 0.0f) x += 1.0f;
	while(x > 1.0f) x -= 1.0f;
	return x;
}

objet_alpha2::objet_alpha2(objet& o) : comportement(o), etat_(etat_initial), objet_selectionne_(0), temporisation_(-1), menu_selectionne_(0), pourcentage_initial_(-1) {
	// std::cout << "objetalpha: " << o.id() << std::endl;
}

objet_alpha2::~objet_alpha2() {}

void objet_alpha2::draw(ci::cairo::Context ctx, int w, int h) {
	comportement::draw(ctx, w, h);
	if(etat_ == etat_selection_possible) {
		double r = (temps_actuel() - temporisation_) * 1.0 / 900;
		ctx.setSource(ci::ColorA(objet_attache().couleur(), 0.3));
		ctx.circle(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h), 50 * r);
		ctx.fill();
	}
	if(etat_ == etat_selection_confirmee) {
		ctx.setSource(ci::ColorA(objet_attache().couleur(), 0.3));
		ctx.circle(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h), 50);
		ctx.fill();
	}
	if(etat_ == etat_selection_confirmee || etat_ == etat_presque_accepte) {
		ctx.save();
		ctx.setSourceRgb(1,1,1);
		ctx.moveTo(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h));
		ctx.setFontSize(9);
		ctx.showText("DEPLACER");
		ctx.stroke();
		ctx.restore();
	}
	if(etat_ == etat_pourrait_accepter && objet_selectionne_ && menu_selectionne_) {
		ctx.save();
		ctx.setSource(ci::ColorA(objet_selectionne_->couleur(), 0.3));
		ctx.setLineWidth(3);
		ctx.moveTo(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h));	
		ctx.lineTo(ci::Vec2f(objet_selectionne_->x() * w, objet_selectionne_->y() * h));
		ctx.stroke();
		ctx.restore();
		menu_draw(ctx, w, h);
	}
}

void objet_alpha2::menu_draw(ci::cairo::Context ctx, int w, int h) {
	float x = objet_attache().x() * w;
	float y = objet_attache().y() * h;
	ci::Color couleur(objet_attache().couleur());
	
	static const float rayon = 50;
	
	// Affiche le titre bien au-dessus
	std::string titre = menu_selectionne_->titre();
	if(menu_selectionne_->selection() != menu_selectionne_->end()) {
		if(!(*menu_selectionne_->selection())->commande()) {
			titre += " ";
			titre += (*menu_selectionne_->selection())->titre();
		} else {
			titre = (*menu_selectionne_->selection())->titre();
		}
	}
	ci::cairo::TextExtents e = ctx.textExtents(titre);
	ctx.moveTo(x - e.width() * 0.5, y - e.height() * 0.5 - rayon - 20);
	ctx.setSourceRgb(1, 1, 1);
	ctx.setFontSize(15);
	ctx.showText(titre);
	ctx.stroke();
	
	// Calcule le nombre d'éléments du menu et l'arc qu'occupe un élément
	int nb_elts = menu_selectionne_->size();
	if(nb_elts == 0)
		return;
	double angle_elt = 2*M_PI / nb_elts;
	// std::cout << angle_elt << std::endl;
	
	// Affiche chaque élément
	ctx.setFontSize(12);
	int i = 0;
	// int decalage = std::distance(menu_->begin(), menu_->selection());
	for(menu::iterator it = menu_selectionne_->begin(); it != menu_selectionne_->end(); ++it) {
		if(*it) {
			ctx.save();
			ctx.setLineWidth(2);
			ctx.translate(x,y);
			ctx.rotate(angle_elt * i);
			ctx.setSourceRgba(0,0,0,0.5);
			ctx.moveTo(0,0);
			ctx.lineTo(rayon,0);
			ctx.stroke();
			if(it == menu_selectionne_->selection())
				ctx.setSource(ci::ColorA(couleur, 0.8));
			else
				ctx.setSource(ci::ColorA(couleur, 0.3));
			
			ctx.moveTo(0,0);
			ctx.arc(0, 0, rayon, 0, angle_elt);
			// ctx.moveTo(0,0);
			ctx.closePath();
			ctx.fill();
			ctx.save();
			ci::cairo::TextExtents e2 = ctx.textExtents((*it)->titre());
			ctx.rotate(angle_elt * 0.5);
			// ctx.translate(cos(angle_elt * 0.5) * 50 + e2.width() *0.5, sin(angle_elt * 0.5) * 50 - e2.height() *0.5);
			ctx.translate(rayon + 5, e2.width() * -0.5);
			if(it == menu_selectionne_->selection())
				ctx.setSourceRgb(1,1,1);
			else
				ctx.setSourceRgba(1,1,1,0.4);
			ctx.save();
			ctx.rotate(M_PI * 0.5);
			ctx.showText((*it)->titre());
			ctx.stroke();
			ctx.restore();
			ctx.restore();
			ctx.restore();
		} else {
			std::cout << "erreur objet_alpha2::menu_draw" << std::endl;
		}
		++i;
	}
}

// #include <iostream>
void objet_alpha2::operator()() {
	if(etat_ == etat_initial) {
		// Vérifie si une sélection est possible
		objet* obj_collision = objet_attache().objet_en_collision();
		if(objet_attache().present() && obj_collision && !obj_collision->zombie()) {
			objet_selectionne_ = obj_collision;
			etat_ = etat_selection_possible;
			temporisation_ = temps_actuel();
			// std::cout << "0 initial >> 1 selection possible" << std::endl;
		}
	} else if(etat_ == etat_selection_possible) {
		// Vérifie que l'objet alpha est toujours sur la sélection pressentie
		// Sinon, on annule
		if(objet_attache().present() && (!objet_selectionne_ || !objet_attache().collision(*objet_selectionne_))) {
			objet_selectionne_ = 0;
			etat_ = etat_initial;
			temporisation_ = -1;
			// std::cout << "1 selection possible >> 0 initial" << std::endl;
		}
		// Une attente de X ms est suffisante pour confirmer la sélection
		else if(temps_actuel() - temporisation_ >= 900) {
			// std::cout << "1 selection possible >> 2 selection confirmee" << std::endl;
			etat_ = etat_selection_confirmee;
			temporisation_ = -1;
			afficher_nom_ = false;
			if(!objet_selectionne_->est_attache())
				menu_selectionne_ = &menu_affectation_objet::instance();
			else
				menu_selectionne_ = &mnu_ed_;
			
			if(menu_affectation_objet::instance().size() > 0)
				menu_affectation_objet::instance().select(menu_affectation_objet::instance().begin());
		}
	} else if(etat_ == etat_selection_confirmee) {
		// Attend qu'on sorte de l'objet
		if(objet_attache().present() && !objet_attache().collision(*objet_selectionne_)) {
			etat_ = etat_pourrait_accepter;
			pourcentage_initial_ = -1;
			// std::cout << "2 selection confirmee >> 3 pourrait accepter" << std::endl;
		}
	} else if(etat_ == etat_pourrait_accepter) {	
		// Vérifie que l'objet alpha est toujours en dehors de la sélection
		// Sinon, on passe à l'état "presque accepte"
		if(objet_attache().present() && objet_attache().collision(*objet_selectionne_)) {
			etat_ = etat_presque_accepte;
			// std::cout << "3 pourrait accepter >> 4 presque accepte" << std::endl;
		}
		if(menu_selectionne_) {
			// La rotation de l'objet alpha définit ce qui est sélectionné dans le menu
			float r = objet_attache().r();
			float pourcentage = entre0et1(r / 2 / M_PI);
			
			if(pourcentage_initial_ == -1)
				pourcentage_initial_ = entre0et1(pourcentage);
			// std::cout << pourcentage_initial_ << "prcent initial" << std::endl;
			// std::cout << pourcentage << "prcent" << std::endl;
			unsigned int selection = entre0et1(pourcentage - pourcentage_initial_) * menu_selectionne_->size();
			menu::iterator sel = menu_selectionne_->begin();
			while(selection > 0 && sel != menu_selectionne_->end()) {
				++sel;
				--selection;
			}
			menu_selectionne_->select(sel);
		}
	} else if(etat_ == etat_presque_accepte) {
		// Attend que l'objet alpha s'en aille de l'objet sélectionné
		if(objet_attache().present() && !objet_attache().collision(*objet_selectionne_)) {
			etat_ = etat_accepte;
			pourcentage_initial_ = -1;
			// std::cout << "5 presque accepte >> 6 accepte" << std::endl;
		}
	} else if(etat_ == etat_accepte) {
		etat_ = etat_initial;
		afficher_nom_ = true;
		// Fait un truc
		if(menu_selectionne_)
			menu_selectionne_->confirmer(*objet_selectionne_);
	}
}