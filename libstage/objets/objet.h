#if !defined(OBJET_H_I70OZNCH)
#define OBJET_H_I70OZNCH

#include <cmath>
#include <cinder/Rect.h>
#include <cinder/Vector.h>
#include <cinder/Matrix.h>
#include <cinder/Color.h>

struct comportement;
struct objet_store;

class couplage_virtuel;

class objet;

// struct world {
// 	~world() {}
// 	void set_projection(ci::Matrix44f proj) { proj_ = proj; }
// 	void set_screen(ci::Vec2f s) { screen_ = s; }
// 	void set_world(ci::Matrix44f w) { world_ = w; }
// 	ci::Matrix44f get_world() { return world_; }
// 	static ci::Vec2f to_screen(objet&, bool c = false);
// 	static world& instance() { static world inst; return inst; }
// private:
// 	world();
// 	ci::Matrix44f world_;
// 	ci::Matrix44f proj_;
// 	ci::Vec2f screen_;
// };

struct objet {
	objet(objet_store* store, int id);
	virtual ~objet();
	void id(int i) { id_ = i; }
	void x(float i) { x_ = i; }
	void y(float i) { y_ = i; }
	void z(float i) { z_ = i; }
	void r(float i) { r_ = i; }
	void present(bool i);
	void couleur(ci::Color c) { couleur_ = c; }
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
	ci::Rectf rect_abs();
	ci::Rectf rect_abs_scaled(ci::Vec2f scl);
	void rect(ci::Rectf r) { rect_ = r; }
	bool present();
	bool present_abs() { return present_; }
	bool zombie();
	float zombie_percent();
	float absent_percent();
	void attacher(comportement* c);
	void detacher();
	void detacher(comportement* c);
	bool est_attache();
	bool est_vraiment_attache();
	comportement* comportement_attache() { return comportement_; }
	objet_store* store() { return store_; }
	ci::Color couleur() { return couleur_; }
	float distance(objet& o);
	bool collision(objet& o);
	bool contient(float x, float y);
	bool contient(ci::Vec2f p);
	bool est_dans(ci::Rectf r);
	objet* objet_en_collision();
private:
	friend class couplage_virtuel;
	int id_;
	float x_, y_, z_, r_;
	bool present_;
	ci::Rectf rect_; 
	comportement* comportement_;
	objet_store* store_;
	couplage_virtuel* couplage_;
	ci::Color couleur_;
	double compteur_;
	bool zombie_;
};

#endif /* end of include guard: OBJET_H_I70OZNCH */
