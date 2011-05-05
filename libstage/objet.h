#if !defined(OBJET_H_I70OZNCH)
#define OBJET_H_I70OZNCH

#include <cmath>

struct comportement;
struct objet_store;

// struct couplage_virtuel {
// 	couplage_virtuel(float k, float z, float x = 0.f) : k_(k), z_(z) {}
// 	virtual ~couplage_virtuel() {}
// 	void reset(float i = 0.f) { x_ = i; }
// 	void k(float i) { k_ = i; }
// 	void z(float i) { z_ = i; }
// 	float k() { return k_; }
// 	float z() { return z_; }
// 	void update() { /* formule Julien */ }
// 	double& operator()() { return x_; }
// private:
// 	float k_, z_, x_;
// };

struct floatrect {
	floatrect() {}
	floatrect(float cx1, float cy1, float cx2, float cy2) : x1(cx1), y1(cy1), x2(cx2), y2(cy2) {}
	bool contains(float x, float y) {
		return x1 <= x && x <= x2 && y1 <= y && y <= y2;
	}
	float x1, y1, x2, y2;
	int id;
};

struct objet {
	objet(objet_store* store, int id, float x = 0.0, float y = 0.0, float r = 0.0, float v = 0.0 , float a = 0.0, bool present = true);
	void id(int i) { id_ = i; }
	void x(float i) { x_ = i; }
	void y(float i) { y_ = i; }
	void r(float i) { r_ = i; }
	void v(float i) { v_ = i; }
	void a(float i) { a_ = i; }
	void z(float i) { z_ = i; }
	void present(bool i) { present_ = i; }
	void rayon(float i) { rayon_ = i; }
	int id() { return id_; }
	float x() { return x_; }
	float y() { return y_; }
	float r() { return r_; }
	float v() { return v_; }
	float a() { return a_; }
	float z() { return z_; }
	floatrect rect() { if(!rect_.contains(x(), y())) rect_ = floatrect(); return rect_; }
	void rect(floatrect r) { rect_ = r; }
	bool present() { return present_; }
	float rayon() { return rayon_; }
	void attacher(comportement& c) { comportement_ = &c; }
	void detacher(comportement& c) { comportement_ = 0; }
	bool est_attache() { return comportement_ != 0; }
	comportement* comportement_attache() { return comportement_; }
	objet_store& store() { return *store_; }
	
	float distance(objet& o) { return sqrt(pow(x() - o.x(), 2) + pow(y() - o.y(), 2)); }
	bool collision(objet& o) { return distance(o) <= rayon() + o.rayon(); }
	bool contient(float x, float y) { return false; } // TODO: remplir correctement
	bool est_dans(floatrect r) { return r.contains(x(), y()); }
private:
	int id_;
	float x_, y_, r_, v_, a_, z_;
	bool present_;
	float rayon_;
	floatrect rect_; 
	comportement* comportement_;
	objet_store* store_;
};

#endif /* end of include guard: OBJET_H_I70OZNCH */
