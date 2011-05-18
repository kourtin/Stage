#include "comportements/comportement_fx.h"
#include "son/fx.h"

comportement_fx::comportement_fx(objet& o) : comportement(o), fx_(0) {}
comportement_fx::~comportement_fx() {}

liste_parametres* comportement_fx::parametres() { 
	if(fx_)
		return &fx_->parametres();
	return 0;
}

void comportement_fx::draw(ci::cairo::Context ctx, int w, int h) {
	comportement::draw(ctx, w, h);

	// Affichage des paramètres
	if(fx_) {
		draw_params(ctx, w, h, &fx_->parametres());
	}
}