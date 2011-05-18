#include "comportements/comportement_ctrl.h"

comportement_ctrl::comportement_ctrl(objet& o) : comportement(o), o_(0), o_promis_(0), detacher_(false), temporisation_(-1) {}
comportement_ctrl::~comportement_ctrl() {}

void comportement_ctrl::draw(ci::cairo::Context ctx, int w, int h) {
	comportement::draw(ctx, w, h);
	
	// Retour visuel pour le mécanisme d'attachement/détachement
	ctx.moveTo(objet_attache().x()*w,objet_attache().y()*h);
	ctx.setSourceRgb(1,1,1);
	if(!o_ && !o_promis_) {
		couleur(ci::Color(1.0, 0, 0));
		// Affiche les objets pouvant représenter un processus sonore acceptable
		ctx.showText("attend attache");
	} else if(!o_ && o_promis_) {
		couleur(ci::Color(0.5, 0.5, 0));
		// Met en surbillance la temporisation pour l'attachement
		ctx.showText("attachement proche...");
	} else if(o_ || (o_ && o_promis_ == o_)) {
		// Affiche des trucs pour dire que c'est actuellement attaché
		ctx.showText("attaché");
		ci::Color vert(0, 1.0, 0);
		couleur(vert);
		// Fait une flèche vers l'objet sur lequel on est attaché
		ctx.save();
		ctx.moveTo(objet_attache().x()*w,objet_attache().y()*h);
		ctx.lineTo(o_->x()*w, o_->y()*h);
		ctx.setSource(vert);
		ctx.stroke();
		ctx.restore();
	} else if(o_ && o_promis_) {
		couleur(ci::Color(0.5, 0.5, 0));
		// Met en surbillance la temporisation pour le détachement
		ctx.showText("detachement proche...");
	} else return;
	ctx.stroke();
}

bool comportement_ctrl::accepte(objet& o) {
	return o.est_attache() && (o.comportement_attache()->what() == comportement::type_source || o.comportement_attache()->what() == comportement::type_fx);
}

void comportement_ctrl::operator()() {
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