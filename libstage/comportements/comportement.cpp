#include "comportements/comportement.h"
#include "temps.h"
#include <sstream>

comportement::comportement(objet& o) : objet_(o), afficher_nom_(true) {
	couleur(true);
	o.attacher(this);
}

comportement::~comportement() {
	objet_.detacher(this);
}

void comportement::draw(ci::cairo::Context ctx, int w, int h) {
	static std::clock_t c = std::clock();
	float coef = (std::clock() - c) * 1.0 / CLOCKS_PER_SEC;
	if(std::clock()-c > 2 * CLOCKS_PER_SEC)
		c = std::clock();
	coef = std::abs(1 - coef) * (1-objet_attache().zombie_percent());
	// ci::gl::color(ci::ColorA(0.5f, 0.f, 0.5f, (float)coef));
	ctx.setSource(ci::ColorA(couleur_, coef));
	ctx.circle(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h), 10);
	ctx.setLineWidth(4);
	ctx.stroke();
	
	if(afficher_nom_) {
		ci::cairo::TextExtents e = ctx.textExtents(classe());
		ctx.moveTo(ci::Vec2f(objet_attache().x() * w - e.width() * 0.5, objet_attache().y() * h - 20 - e.height() * 0.5));
		ctx.setSourceRgba(1, 1, 1, 0.2 + 0.8 * (1-objet_attache().zombie_percent()));
		// ctx.setFont(fnt_ttl_);
		ctx.setFontSize(15);
		ctx.showText(classe());
		ctx.stroke();
	}
	
	draw_params(ctx, w, h, parametres());
}

void comportement::draw_params(ci::cairo::Context ctx, int w, int h, liste_parametres* l) {
	if(l) {
		ctx.setFontSize(10);
		float y = 0;
		for(liste_parametres::iterator it = l->begin(); it != l->end(); ++it) {
			parametre* p = *it;
			if(p) {
				ctx.moveTo(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h + y));
				std::ostringstream oss;
				oss << p->nom() << ": " << p->get_str();
				ctx.showText(oss.str());
				ctx.stroke();	
				y += 10;
			}
		}
	}
}