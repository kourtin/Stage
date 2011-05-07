#include "objet.h"
#include "couplage_virtuel.h"
#include <cmath>
#include <cstdlib>

objet::objet(objet_store* store, int id, float x, float y, float r, float v, float a, bool present) : id_(id), x_(x), y_(y), r_(r), v_(v), a_(a), z_(-1), present_(present), rayon_(0.01f + 0.001f*(rand() % 30)), store_(store), couplage_(0), image_(cv::Size(640,480), CV_8UC1) {
	couplage_ = new couplage_virtuel(*this);
}

objet::~objet() {
	if(couplage_)
		delete couplage_;
}

float objet::x() { if(couplage_) return couplage_->x(); else return x_; }
float objet::y() { if(couplage_) return couplage_->y(); else return y_; }
float objet::z() { if(couplage_) return couplage_->z(); else return z_; }