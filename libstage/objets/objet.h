#if !defined(OBJET_H_I70OZNCH)
#define OBJET_H_I70OZNCH

#include <cmath>
#include <cinder/Rect.h>
#include <cinder/Vector.h>

struct comportement;
struct objet_store;

class couplage_virtuel;

struct objet {
	objet(objet_store* store, int id);
	virtual ~objet();
	void id(int i) { id_ = i; }
	void x(float i) { x_ = i; }
	void y(float i) { y_ = i; }
	void z(float i) { z_ = i; }
	void r(float i) { r_ = i; }
	void present(bool i) { present_ = i; }
	int id() { return id_; }
	float x();
	float y();
	float z();
	float r();
	float x_c() { return x_; }
	float y_c() { return y_; }
	float z_c() { return z_; }
	float r_c() { return r_; }
	ci::Rectf rect() { return rect_; }
	ci::Rectf rect_abs() { return rect() + ci::Vec2f(x(),y()); }
	ci::Rectf rect_abs_scaled(ci::Vec2f scl) { ci::Rectf r = rect_abs(); return ci::Rectf(r.getX1() * scl.x, r.getY1() * scl.y, r.getX2() * scl.x, r.getY2() * scl.y); }
	void rect(ci::Rectf r) { rect_ = r; }
	bool present() { return present_; }
	void attacher(comportement* c) { comportement_ = c; }
	void detacher() { comportement_ = 0; }
	bool est_attache() { return comportement_ != 0; }
	comportement* comportement_attache() { return comportement_; }
	objet_store* store() { return store_; }
	
	float distance(objet& o) { return ci::Vec2f(x(), y()).distance(ci::Vec2f(o.x(), o.y())); }
	bool collision(objet& o) { return rect_abs().intersects(o.rect_abs()); }
	bool contient(float x, float y) { return rect_abs().contains(ci::Vec2f(x,y)); }
	bool contient(ci::Vec2f p) { return rect_abs().contains(p); }
	bool est_dans(ci::Rectf r) { return r.intersects(rect_abs()); }
private:
	friend class couplage_virtuel;
	int id_;
	float x_, y_, z_, r_;
	bool present_;
	ci::Rectf rect_; 
	comportement* comportement_;
	objet_store* store_;
	couplage_virtuel* couplage_;
};

#endif /* end of include guard: OBJET_H_I70OZNCH */
