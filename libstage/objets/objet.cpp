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