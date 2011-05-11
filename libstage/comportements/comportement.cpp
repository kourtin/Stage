#include "comportements/comportement.h"
#include <ctime>

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
	// ci::gl::drawSolidCircle(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h), 10);
	// ci::gl::drawStringCentered(classe(), , ci::Color(1,1,1), fnt_ttl_);
	// ci::gl::drawSolidRect(ci::Rectf(objet_attache().x() * w - 10, objet_attache().y() * h - 10, objet_attache().x() * w + 10, objet_attache().y() * h + 10));
}