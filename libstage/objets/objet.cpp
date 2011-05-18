#include "objets/objet.h"
#include "objets/objet_store.h"
#include "couplage_virtuel/couplage_virtuel.h"
#include "cindercolor.h"
#include <ctime>

objet::objet(objet_store* store, int id) : id_(id), x_(0), y_(0), z_(-1), r_(0), present_(false), comportement_(0), store_(store), couplage_(0), rect_(-0.05, -0.05, 0.05, 0.05), couleur_(randColor()), compteur_(std::clock()) {
	if(store_)
		couplage_ = new couplage_virtuel(*this);
}

objet::~objet() {
	if(couplage_)
		delete couplage_;
}

float objet::x() { if(couplage_) return couplage_->x(); else return x_; }
float objet::y() { if(couplage_) return couplage_->y(); else return y_; }
float objet::z() { if(couplage_) return couplage_->z(); else return z_; }
float objet::r() { if(couplage_) return couplage_->r(); else return r_; }

bool objet::present() {
	if(std::clock() - compteur_ > 0.2 * CLOCKS_PER_SEC)
		return false;
	else
		return true;
}

void objet::present(bool i) {
	if(i)
		compteur_ = std::clock();
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