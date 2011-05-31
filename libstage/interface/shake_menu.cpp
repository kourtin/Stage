#include "interface/shake_menu.h"
#include "temps.h"
#include <cmath>
#include <iostream>

float entre0et2pi(float x) {
	while(x < 0.0f) x += 2 * M_PI;
	while(x > 2 * M_PI) x -= 2 * M_PI;
	return x;
}

shake_comportement::shake_comportement(objet& o) : comportement(o), etat_(etat_initial), temporisation_(-1), menu_selectionne_(0), maracas_(), numero_page_(0), x_(0.5), y_(0.5), rayon_(0), rayon_int_(0), rayon_page_(0), page_moins_sel_(false), page_plus_sel_(false) {}

shake_comportement::~shake_comportement() {}

unsigned int shake_comportement::nb_pages() {
	if(!menu_selectionne_) return 0;
	return static_cast<unsigned int>(ceil(menu_selectionne_->size() * 1.f / 8));
}

unsigned int shake_comportement::idx_premier_element_page_actuelle() {
	return numero_page_ * 8;
}

unsigned int shake_comportement::nb_elements_page_actuelle() {
	if(!menu_selectionne_) return 0;
	if(numero_page_ < nb_pages() - 1)
		return 8;
	else
		return menu_selectionne_->size() - idx_premier_element_page_actuelle();
}

float shake_comportement::angle_1_element_page_actuelle() {
	return 2 * M_PI / nb_elements_page_actuelle();
}

// enum { etat_initial, etat_secoue, etat_affiche_menu, etat_secoue_confirme, etat_accepte };

void shake_comportement::draw(ci::cairo::Context ctx, int w, int h) {
	// comportement::draw(ctx, w, h);
	if(etat_ == etat_secoue) {
		// Un disque qui grandit
		double r = (temps_actuel() - temporisation_) * 1.0 / 1000;
		ctx.setSource(ci::ColorA(objet_attache().couleur(), 0.3 + (1-r) * 0.7));
		ctx.circle(ci::Vec2f(x_ * w, y_ * h), rayon_ * w * r);
		ctx.setFillRule(1);
		ctx.circle(ci::Vec2f(x_ * w, y_ * h), rayon_int_ * w * r);
		ctx.fill();
		ctx.setFillRule(0);
	}
	if(etat_ == etat_affiche_menu) {
		menu_draw(ctx, w, h);
	}
	if(etat_ == etat_secoue_confirme) {
		// Le disque qui rétrécit
		double r = 1 - ((temps_actuel() - temporisation_) * 1.0 / 100);
		
		ctx.setSource(ci::ColorA(objet_attache().couleur(), 0.3 + r * 0.7));
		ctx.circle(ci::Vec2f(x_ * w, y_ * h), rayon_ * w * r);
		ctx.setFillRule(1);
		ctx.circle(ci::Vec2f(x_ * w, y_ * h), rayon_int_ * w * r);
		ctx.fill();
		ctx.setFillRule(0);
	}
}

void shake_comportement::menu_draw(ci::cairo::Context ctx, int w, int h) {
	float x = x_ * w;
	float y = y_ * h;
	ci::Color couleur(objet_attache().couleur());
	
	// Met à jour la sélection
	// Calcule l'angle formé par l'objet et le centre du menu (entre 0 et 2π)
	float angle = entre0et2pi(-atan2(objet_attache().x() * w - x, objet_attache().y() * h - y) + M_PI/2);

	// Calcule la distance entre ces deux trucs
	float distance = sqrt(pow(objet_attache().x() * w - x, 2) + pow(objet_attache().y() * h - y, 2));
	
	// std::cout << "angle: " << (angle*180/M_PI) << std::endl;
	// std::cout << "dist : " << (distance) << std::endl;

	// Regarde si l'objet est au bon endroit
	if(rayon_int_ * w <= distance && distance <= rayon_ * w) {
		unsigned int num_section_selectionnee = static_cast<unsigned int>(std::floor(angle / angle_1_element_page_actuelle()));
		unsigned int selection = num_section_selectionnee + idx_premier_element_page_actuelle();
		menu_selectionne_->select(selection);
		page_moins_sel_ = false;
		page_plus_sel_ = false;
	} else if(rayon_int_ * w > distance) {
		menu_selectionne_->select(menu_selectionne_->end());
		page_moins_sel_ = false;
		page_plus_sel_ = false;
	}
	
	// Regarde si l'objet est sur le bouton moins
	if(numero_page_ > 0) {
		float distance_bouton_moins = sqrt(pow(objet_attache().x() * w - (x - rayon_ * w - rayon_page_ * w), 2) + pow(objet_attache().y() * h - y, 2));
		if(distance_bouton_moins <= rayon_page_ * w) {
			menu_selectionne_->select(menu_selectionne_->end());
			page_moins_sel_ = true;
			page_plus_sel_ = false;
		}
	}
	
	// Regarde si l'objet est sur le bouton plus
	if(numero_page_ < nb_pages()-1) {
		float distance_bouton_plus = sqrt(pow(objet_attache().x() * w - (x + rayon_ * w + rayon_page_ * w), 2) + pow(objet_attache().y() * h - y, 2));
		if(distance_bouton_plus <= rayon_page_ * w) {
			menu_selectionne_->select(menu_selectionne_->end());
			page_plus_sel_ = true;
			page_moins_sel_ = false;
		}
	}
	// /Met à jour la sélection
	
	// Affiche le titre bien au-dessus
	std::string titre = menu_selectionne_->titre();
	if(page_plus_sel_ || page_moins_sel_) {
		titre = "Voir d'autres choix...";
	} else if(menu_selectionne_->selection() != menu_selectionne_->end()) {
		if(!(*menu_selectionne_->selection())->commande()) {
			titre += " ";
			titre += (*menu_selectionne_->selection())->titre();
		} else {
			titre = (*menu_selectionne_->selection())->titre();
		}
	} else {
		titre = "Annuler";
	}
	ci::cairo::TextExtents e = ctx.textExtents(titre);
	ctx.moveTo(x - e.width() * 0.5, y - e.height() * 0.5 - rayon_ * w - 20);
	ctx.setSourceRgb(1, 1, 1);
	ctx.setFontSize(15);
	ctx.showText(titre);
	ctx.stroke();
	
	// Calcule le nombre d'éléments du menu et l'arc qu'occupe un élément
	unsigned int nb_elts = nb_elements_page_actuelle();
	if(nb_elts == 0)
		return;
	double angle_elt = angle_1_element_page_actuelle();
	
	// Affiche chaque élément
	ctx.setFontSize(12);
	for(unsigned int i = 0; i < nb_elts; ++i) {
		element_menu* actuel = menu_selectionne_->at(idx_premier_element_page_actuelle() + i);
		bool est_la_selection = idx_premier_element_page_actuelle() + i == menu_selectionne_->index_selection();
		ctx.save();
		
		// Place le repère où il faut pour dessiner
		ctx.translate(x,y);
		ctx.rotate(angle_elt * i);
		
		// Fait une belle ligne bien noire
		ctx.setLineWidth(2);
		ctx.setSourceRgba(0,0,0,0.5);
		ctx.moveTo(rayon_int_ * w,0);
		ctx.lineTo(rayon_ * w,0);
		ctx.stroke();
		
		// Met la bonne couleur
		if(est_la_selection)
			ctx.setSource(ci::ColorA(couleur, 0.8));
		else
			ctx.setSource(ci::ColorA(couleur, 0.3));
		
		// Fait une partie du cercle
		ctx.moveTo(rayon_int_ * w,0);
		ctx.arc(0, 0, rayon_ * w, 0, angle_elt);
		// ctx.moveTo()
		ctx.lineTo(rayon_int_ * w * cos(angle_elt), sin(angle_elt) * rayon_int_ * w);
		ctx.arcNegative(0, 0, rayon_int_ * w, angle_elt, 0);
		// ctx.closePath();
		ctx.fill();
		
		// Ecrit le texte qui va avec
		ctx.save();
		ci::cairo::TextExtents e2 = ctx.textExtents(actuel->titre());
		ctx.rotate(angle_elt * 0.5);
		ctx.translate((rayon_ + rayon_int_) * 0.5 * w, e2.width() * -0.5);
		
		if(est_la_selection)
			ctx.setSourceRgb(1,1,1);
		else
			ctx.setSourceRgba(1,1,1,0.4);
		
		ctx.save();
		ctx.rotate(M_PI * 0.5);
		ctx.showText(actuel->titre());
		ctx.stroke();
		ctx.restore();
		
		ctx.restore();
		// /Ecrit le texte qui va avec
		ctx.restore();
	}
	
	// Ecrit le bouton -
	if(numero_page_ > 0) {
		bool est_la_selection = page_moins_sel_;
		ctx.save();
		ctx.translate(x - rayon_ * w - 10, y);
		if(est_la_selection)
			ctx.setSource(ci::ColorA(couleur, 0.8));
		else
			ctx.setSource(ci::ColorA(couleur, 0.3));
		ctx.circle(-rayon_page_ * w, 0, rayon_page_ * w);
		ctx.fill();
		if(est_la_selection)
			ctx.setSourceRgb(1,1,1);
		else
			ctx.setSourceRgba(1,1,1,0.4);
		ctx.setLineWidth(4);
		ctx.moveTo(rayon_page_ * w / -2 - rayon_page_*w*0.25, 0);
		ctx.lineTo(rayon_page_ * w / -2 - rayon_page_*w*0.75, 0);
		ctx.stroke();
		ctx.restore();
	}
	
	// Ecrit le bouton +
	if(numero_page_ < nb_pages() - 1) {
		bool est_la_selection = page_plus_sel_;
		ctx.save();
		ctx.translate(x + rayon_ * w + 10, y);
		if(est_la_selection)
			ctx.setSource(ci::ColorA(couleur, 0.8));
		else
			ctx.setSource(ci::ColorA(couleur, 0.3));
		ctx.circle(rayon_page_ * w, 0, rayon_page_ * w);
		ctx.fill();
		if(est_la_selection)
			ctx.setSourceRgb(1,1,1);
		else
			ctx.setSourceRgba(1,1,1,0.4);
		ctx.setLineWidth(4);
		ctx.moveTo(rayon_page_ * w / 2 + rayon_page_*w*0.25, 0);
		ctx.lineTo(rayon_page_ * w / 2 + rayon_page_*w*0.75, 0);
		ctx.stroke();
		ctx.moveTo(rayon_page_ * w, rayon_page_*w*-0.25);
		ctx.lineTo(rayon_page_ * w, rayon_page_*w*0.25);
		ctx.stroke();
		ctx.restore();
	}
	
	// Peint un petit cercle pour repérer l'objet
	ctx.setSource(couleur);
	ctx.circle(objet_attache().x() * w, objet_attache().y() * h, rayon_/10 * w);
	ctx.fill();
}

// #include <iostream>
void shake_comportement::operator()() {
	// std::cout << "coucou" << std::endl;
	if(etat_ == etat_initial) {
		// Regarde si on a secoué
		if(maracas_(objet_attache().x(), objet_attache().y(), objet_attache().z())) {
			etat_ = etat_secoue;
			maracas_.reset();
			temporisation_ = temps_actuel();
			// std::cout << "etat_secoue" << std::endl;
		}
	} else if(etat_ == etat_secoue) {
		// Met à jour la position x et y du futur menu
		x_ = objet_attache().x();
		y_ = objet_attache().y();
		rayon_ = 0.14;//exp((1 - objet_attache().z())) * 0.09;
		rayon_int_ = rayon_ * 0.6;
		rayon_page_ = rayon_ * 0.3;
		
		// // Si on a re-secoué, on annule
		// if(maracas_(objet_attache().x(), objet_attache().y(), objet_attache().z())) {
		// 	etat_ = etat_initial;
		// 	maracas_.reset();
		// 	// std::cout << "etat_initial" << std::endl;
		// }
		// Si on a arrêté de secouer tout ce temps, on affiche le menu
		if(temps_actuel() - temporisation_ > 1000) {
			etat_ = etat_affiche_menu;
			menu_selectionne_ = &menu_affectation_objet::instance();
			maracas_.reset();
			// std::cout << "etat_affiche_menu" << std::endl;
		}
	} else if(etat_ == etat_affiche_menu) {
		// Regarde si la sélection a changé
		if(menu_selectionne_->index_selection() != selection_avant_ || page_moins_sel_ != page_moins_sel_avant_ || page_plus_sel_ != page_plus_sel_avant_) {
			selection_avant_ = menu_selectionne_->index_selection();
			page_moins_sel_avant_ = page_moins_sel_;
			page_plus_sel_avant_ = page_plus_sel_;
			maracas_.reset();
		}
		// Attend un shake pour confirmer
		if(maracas_(objet_attache().x(), objet_attache().y(), objet_attache().z())) {
			maracas_.reset();
			if(page_moins_sel_) {
				--numero_page_;
				page_moins_sel_ = false;
			} else if(page_plus_sel_) {
				++numero_page_;
				page_plus_sel_ = false;
			} else {
				etat_ = etat_secoue_confirme;
				temporisation_ = temps_actuel();
				// std::cout << "etat_secoue_confirme" << std::endl;
			}
		}
	} else if(etat_ == etat_secoue_confirme) {
		// Si on a arrêté de secouer tout ce temps, on affiche le menu
		if(temps_actuel() - temporisation_ > 100) {
			etat_ = etat_accepte;
			maracas_.reset();
			// std::cout << "etat_accepte" << std::endl;
		}
	} else if(etat_ == etat_accepte) {	
		// std::cout << "etat_initial" << std::endl;
		etat_ = etat_initial;
		// Fait un truc
		if(menu_selectionne_)
			menu_selectionne_->confirmer(objet_attache());
	}
}