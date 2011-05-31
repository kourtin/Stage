#include "objets/objet.h"
#include "objets/objet_store.h"
#include "comportements/comportement.h"
#include "couplage_virtuel/couplage_virtuel.h"
#include "cindercolor.h"
#include "temps.h"

// world::world() : screen_(640, 480) {
// 	proj_.setToIdentity();
// 	world_.setToIdentity();
// 	proj_.at(1,1) = -2;
// 	proj_.at(0,0) = 2;
// 	proj_.at(0,3) = -1;
// 	proj_.at(1,3) = 1;
// }
// #include <iostream>
// ci::Vec2f world::to_screen(objet& o, bool c) {
// 	ci::Matrix44f pw = instance().proj_ * instance().world_;
// 	ci::Vec3f coords;
// 	if(!c)
// 		coords = pw.transformPoint(ci::Vec3f(o.x(), o.y(), o.z()));
// 	else
// 		coords = pw.transformPoint(ci::Vec3f(o.x_c(), o.y_c(), o.z_c()));
// 	
// 	// std::cout << ci::Vec2f((1 + coords.x) * 0.5 * instance().screen_.x, (1 - coords.y) * 0.5 * instance().screen_.y) << std::endl;
// 	return ci::Vec2f((1 + coords.x) * 0.5 * instance().screen_.x, (1 - coords.y) * 0.5 * instance().screen_.y);
// }

objet::objet(objet_store* store, int id) : id_(id), x_(0), y_(0), z_(-1), r_(0), present_(false), comportement_(0), store_(store), couplage_(0), rect_(-0.05, -0.05, 0.05, 0.05), couleur_(randColor()), compteur_(std::clock()) {
	if(store_)
		couplage_ = new couplage_virtuel(*this);
}

objet::~objet() {
	detacher();
	
	if(couplage_)
		delete couplage_;
}

void objet::attacher(comportement* c) {
	detacher();
	comportement_ = c;
}

void objet::detacher() {
	if(comportement_) {
		delete comportement_;
		comportement_ = 0;
	}
}

void objet::detacher(comportement*) {
	comportement_ = 0;
}

bool objet::est_attache() {
	return est_vraiment_attache() && !comportement_->provisoire();
}

bool objet::est_vraiment_attache() {
	return comportement_ != 0;
}

float objet::x() { if(couplage_) return couplage_->x(); else return x_; }
float objet::y() { if(couplage_) return couplage_->y(); else return y_; }
float objet::z() { if(couplage_) return couplage_->z(); else return z_; }
float objet::r() { if(couplage_) return couplage_->r(); else return r_; }


float objet::absent_percent() {
	if(present_) return 0.f;
	return std::max(0.0, std::min(1.0, (temps_actuel()-compteur_) / 200));
}

bool objet::present() {
	if(absent_percent() >= 1)
		return false;
	else
		return true;
}

float objet::zombie_percent() {
	if(present_) return 0.f;
	return std::max(0.0, std::min(1.0, (temps_actuel()-compteur_) / 5000));
}

bool objet::zombie() {
	if(zombie_percent() >= 1)
		return true;
	else
		return false;
}

void objet::present(bool i) {
	present_ = i;
	if(i)
		compteur_ = temps_actuel();
}

ci::Rectf objet::rect_abs() { 
	return rect() + ci::Vec2f(x(),y());
}

ci::Rectf objet::rect_abs_scaled(ci::Vec2f scl) {
	ci::Rectf r = rect_abs();
	return ci::Rectf(r.getX1() * scl.x, r.getY1() * scl.y, r.getX2() * scl.x, r.getY2() * scl.y);
}

float objet::distance(objet& o) { return ci::Vec2f(x(), y()).distance(ci::Vec2f(o.x(), o.y())); }

bool objet::collision(objet& o) { 
	//return present() && o.present() && rect_abs().scaledCentered(2).intersects(o.rect_abs().scaledCentered(2));
	return (present() && std::abs(z() - o.z()) <= 0.1 && rect_abs().scaledCentered(1.1).intersects(o.rect_abs().scaledCentered(1.1)));
}
bool objet::contient(float x, float y) { return rect_abs().contains(ci::Vec2f(x,y)); }
bool objet::contient(ci::Vec2f p) { return rect_abs().contains(p); }
bool objet::est_dans(ci::Rectf r) { return r.intersects(rect_abs()); }

objet* objet::objet_en_collision() {
	if(store_) {
		for(objet_store::iterator it = store_->begin(); it != store_->end(); ++it)
			if((*it) != this && collision(*(*it)))
				return *it;
	}
	return 0;
}