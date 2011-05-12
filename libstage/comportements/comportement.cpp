#include "comportements/comportement.h"
#include "son/source.h"
#include "son/fx.h"
#include <ctime>
#include <sstream>

double temps_actuel() { 
	return std::clock() * 1000.0 / CLOCKS_PER_SEC;
}

comportement::comportement(objet& o) : objet_(o) {
	o.attacher(this);
}

comportement::~comportement() {
	objet_.detacher();
}

void comportement::draw(ci::cairo::Context ctx, int w, int h) {
	static std::clock_t c = std::clock();
	float coef = (std::clock() - c) * 1.0 / CLOCKS_PER_SEC;
	if(std::clock()-c > 2 * CLOCKS_PER_SEC)
		c = std::clock();
	coef = std::abs(1 - coef);
	// ci::gl::color(ci::ColorA(0.5f, 0.f, 0.5f, (float)coef));
	ctx.setSourceRgba(0.5f, 0.5f, 0.5f, coef);
	ctx.circle(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h), 10);
	ctx.setLineWidth(4);
	ctx.stroke();
	ci::cairo::TextExtents e = ctx.textExtents(classe());
	ctx.moveTo(ci::Vec2f(objet_attache().x() * w - e.width() * 0.5, objet_attache().y() * h - 40 - e.height() * 0.5));
	ctx.setSourceRgb(1, 1, 1);
	// ctx.setFont(fnt_ttl_);
	ctx.setFontSize(15);
	ctx.showText(classe());
}

void comportement::draw_params(ci::cairo::Context ctx, int w, int h, liste_parametres* l) {
	if(l) {
		ctx.setFontSize(10);
		float y = 0;
		for(liste_parametres::iterator it = l->begin(); it != l->end(); ++it) {
			parametre* p = *it;
			ctx.moveTo(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h + y));
			std::ostringstream oss;
			oss << p->nom() << ": " << p->get_str();
			ctx.showText(oss.str());
			ctx.stroke();	
			y += 10;
		}
	}
}

comportement_source::comportement_source(objet& o) : comportement(o), source_(0) {}
comportement_source::~comportement_source() {}

liste_parametres* comportement_source::parametres() { 
	if(source_)
		return &source_->parametres();
	return 0;
}

void comportement_source::draw(ci::cairo::Context ctx, int w, int h) {
	comportement::draw(ctx, w, h);
	
	// Affichage des paramètres
	if(source_) {
		draw_params(ctx, w, h, &source_->parametres());
	}
}

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

comportement_ctrl::comportement_ctrl(objet& o) : comportement(o) {}
comportement_ctrl::~comportement_ctrl() {}

comportement_tool::comportement_tool(objet& o) : comportement(o) {}
comportement_tool::~comportement_tool() {}