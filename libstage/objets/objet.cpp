#include "objets/objet.h"
#include "couplage_virtuel/couplage_virtuel.h"

objet::objet(objet_store* store, int id) : id_(id), x_(0), y_(0), z_(-1), r_(0), present_(false), comportement_(0), store_(store), couplage_(0) {
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
	return rect_abs().intersects(o.rect_abs());
}
bool objet::contient(float x, float y) { return rect_abs().contains(ci::Vec2f(x,y)); }
bool objet::contient(ci::Vec2f p) { return rect_abs().contains(p); }
bool objet::est_dans(ci::Rectf r) { return r.intersects(rect_abs()); }