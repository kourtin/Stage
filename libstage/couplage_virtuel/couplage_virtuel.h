#if !defined(COUPLAGE_VIRTUEL_H_SUNSRRBK)
#define COUPLAGE_VIRTUEL_H_SUNSRRBK

#include "objets/objet.h"

class calculateur_couplage;

struct couplage_virtuel {
	couplage_virtuel(objet& o);
	virtual ~couplage_virtuel();
	void operator()();
	float x() { return px_; }
	float y() { return py_; }
	float z() { return pz_; }
	float r() { return pr_; }
private:
	void maj_position();
	void maj_forces();
private:
	objet& objet_;
	float k_, z_, zmilieu_, invm_;
	float px_, py_, pz_, pr_;
	float ax_, ay_, az_, ar_;
	float fx_, fy_, fz_, fr_;
	float ox_, oy_, oz_, or_;
	float dist_;
};

#endif /* end of include guard: COUPLAGE_VIRTUEL_H_SUNSRRBK */
