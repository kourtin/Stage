#include "comportements/comportement_fx.h"
#include "son/fx.h"
#include "temps.h"

comportement_fx::comportement_fx(objet& o) : comportement(o), fx_(0), o_(0), o_promis_(0), detacher_(false), temporisation_(-1) {}
comportement_fx::~comportement_fx() {}

liste_parametres* comportement_fx::parametres() { 
	if(fx_)
		return &fx_->parametres();
	return 0;
}

void comportement_fx::draw(ci::cairo::Context ctx, int w, int h) {
	comportement::draw(ctx, w, h);
	
	// Retour visuel pour le mécanisme d'attachement/détachement
	ctx.moveTo(objet_attache().x()*w,objet_attache().y()*h);
	ctx.setSourceRgb(1,1,1);
	if(!o_ && !o_promis_) {
		couleur(ci::Color(1.0, 0, 0));
		// Affiche les objets pouvant représenter un processus sonore acceptable
		ctx.showText("attend attache");
	} else if(!o_ && o_promis_) {
		couleur(ci::Color(0.0, 1.0, 0));
		// Met en surbillance la temporisation pour l'attachement
		ctx.showText("attachement proche...");
	} else if(o_ || (o_ && o_promis_ == o_)) {
		// Affiche des trucs pour dire que c'est actuellement attaché
		// ctx.showText("attaché");
		// ci::Color vert(0, 1.0, 0);
		// couleur(vert);
		// Fait une flèche vers l'objet sur lequel on est attaché
		ctx.save();
		ctx.moveTo(objet_attache().x()*w,objet_attache().y()*h);
		ctx.lineTo(o_->x()*w, o_->y()*h);
		// ctx.setSource(vert);
		ci::cairo::GradientLinear g(objet_attache().x()*w,objet_attache().y()*h, o_->x()*w, o_->y()*h);
		g.addColorStop(0, ci::ColorA(objet_attache().couleur(), 0.5));
		g.addColorStop(1, ci::ColorA(o_->couleur(), 0.5));
		g.setExtendPad();
		// ctx.setSource(ci::ColorA(o_->couleur(), 0.5));
		ctx.setSource(g);
		ctx.stroke();
		ctx.restore();
		
		// Affichage des paramètres
		// if(fx_) {
			// draw_params(ctx, w, h, &fx_->parametres());
		// }
	} else if(o_ && o_promis_) {
		couleur(ci::Color(0.5, 0.5, 0));
		// Met en surbillance la temporisation pour le détachement
		ctx.showText("detachement proche...");
	} else return;
	ctx.stroke();
}

bool comportement_fx::accepte(objet& o) {
	return o.est_attache() && o.comportement_attache()->what() == comportement::type_source;
}

void comportement_fx::operator()() {
	if(!o_ && !o_promis_) {
		// Cherche si on est pas sur un objet représentant un processus sonore
		objet* collision = objet_attache().objet_en_collision();
		if(collision && accepte(*collision)) {
			o_promis_ = collision;
			temporisation_ = temps_actuel();
		}
	} else if(!o_ && o_promis_) {
		// Regarde si l'objet est toujours sélectionné
		if(!objet_attache().collision(*o_promis_)) {
			o_promis_ = 0;
			temporisation_ = -1;
		} else {
			// Attend que la tempo soit terminée, qu'on puisse attacher le truc
			if(temps_actuel() - temporisation_ >= 900) {
				o_ = o_promis_;
				maj_objet_associe(0);
			}
		}
	} else if(o_ && o_promis_ == o_) {
		// Attend que l'on ne soit plus en collision avec l'objet
		if(!objet_attache().collision(*o_)) {
			o_promis_ = 0;
		}
	} else if(!detacher_ && o_ && !o_promis_) {
		// Regarde si on est pas revenu sur l'objet en attendant un peu longtemps (pour détacher)
		if(objet_attache().collision(*o_)) {
			o_promis_ = o_+1;
			temporisation_ = temps_actuel();
		}
		// Fera des trucs ?
	} else if(o_ && o_promis_) {
		// Regarde si l'objet est toujours sélectionné
		if(!objet_attache().collision(*o_)) {
			o_promis_ = 0;
			temporisation_ = -1;
		} else {
			// Attend que la tempo soit terminée, qu'on puisse confirmer le détachement
			if(temps_actuel() - temporisation_ >= 900) {
				o_promis_ = 0;
				temporisation_ = -1;
				detacher_ = true;
			}
		}
	} else if(detacher_) {
		if(!objet_attache().collision(*o_)) {
			detacher_ = false;
			objet* p = o_;
			o_ = 0;
			maj_objet_associe(p);
		}
	}
}