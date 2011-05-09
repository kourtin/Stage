#include "oscillathor.h"
#include <ctime>
#include <iostream>

oscillathor::oscillathor(objet& o) : comportement(o), frequence_(440), volume_(0.0)/*, d1_(&line)*/ {
	// d1_.delai(350); d1_.wet(0.3); d1_.feedback(0.4);
}
void oscillathor::draw(ci::cairo::Context ctx, int w, int h) {
	comportement::draw(ctx, w, h);
	
	ctx.setFont(fnt_param_);
	ctx.setFontSize(10);
	std::ostringstream oss;
	oss.precision(4);
	oss << oscil_.frequence() << " Hz";
	
	ci::cairo::TextExtents e = ctx.textExtents(classe());
	ctx.moveTo(ci::Vec2f(objet_attache().x() * w + 20 * cos(objet_attache().r()), objet_attache().y() * h + 20 * sin(objet_attache().r())));
	ctx.showText(oss.str());
	ctx.stroke();
	ctx.setLineWidth(1);
	ctx.setSourceRgba(1,1,1,0.5);
	ctx.line(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h), ci::Vec2f(objet_attache().x() * w - 15, objet_attache().y() * h));
	ctx.stroke();
	// ci::gl::drawStringCentered(oss.str(), ci::Vec2f(objet_attache().x() * w  + 20 * cos(objet_attache().r()), objet_attache().y() * h  + 20 * sin(objet_attache().r())), ci::Color(1,1,1), fnt_param_);
	// ci::gl::drawLine(ci::Vec2f(objet_attache().x() * w, objet_attache().y() * h), ci::Vec2f(objet_attache().x() * w - 15, objet_attache().y() * h - 20));
}

void oscillathor::operator()() {
	static float x_a = objet_attache().x();
	static float y_a = objet_attache().y();
	static float dx_a = 0;
	static float dy_a = 0;
	float x = objet_attache().x();
	float y = objet_attache().y();
	float dx = x - x_a;
	float dy = y - y_a;
	float ax = dx - dx_a;
	float ay = dy - dy_a;
	frequence_ += dx * 440;
	volume_ += std::abs(ay);
	x_a = x;
	y_a = y;
	dx_a = dx;
	dy_a = dy;
	// std::cout << "f=" << frequence_ << ", v=" << volume_ << std::endl;
	// oscil_.frequence(440 + objet_attache().r() * 440 / (2*M_PI));
	oscil_.frequence(objet_attache().x()*440);
	// oscil_.volume(volume_);
	// d1_.delai(objet_attache().x() * 1000 + 100);
	// d1_.feedback(objet_attache().y());
}
