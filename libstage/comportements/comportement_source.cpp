#include "comportements/comportement_source.h"
#include "son/source.h"

comportement_source::comportement_source(objet& o) : comportement(o), source_(0) {}
comportement_source::~comportement_source() {}

liste_parametres* comportement_source::parametres() { 
	if(source_)
		return &source_->parametres();
	return 0;
}

void comportement_source::draw(ci::cairo::Context ctx, int w, int h) {
	comportement::draw(ctx, w, h);
}