#include "comportements/oscillathor.h"
#include <ctime>
#include <iostream>

oscillathor::oscillathor(objet& o) : comportement_source(o) {
	set_source(&oscil_);
}

void oscillathor::draw(ci::cairo::Context ctx, int w, int h) {
	comportement_source::draw(ctx, w, h);
	
	// // ctx.setFont(fnt_param_);
	// ctx.setFontSize(10);
	// std::ostringstream oss;
	// oss.precision(4);
	// oss << oscil_.frequence() << " Hz";
	// 
	// ctx.moveTo(ci::Vec2f(objet_attache().x() * w + 20 * cos(objet_attache().r()), objet_attache().y() * h + 20 * sin(objet_attache().r())));
	// ctx.showText(oss.str());
	// ctx.stroke();
	// ctx.setLineWidth(1);
	// ctx.setSourceRgba(1,1,1,0.5);
	// ctx.line(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h), ci::Vec2f(objet_attache().x() * w - 15, objet_attache().y() * h));
	// ctx.stroke();
}

void oscillathor::operator()() {
	// oscil_.frequence().set(objet_attache().x()*440);
}

micro::micro(objet& o) : comportement_source(o) {
	set_source(&line_);
}

void micro::draw(ci::cairo::Context ctx, int w, int h) {
	comportement_source::draw(ctx, w, h);
}

void micro::operator()() {}
