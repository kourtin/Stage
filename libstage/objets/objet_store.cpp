#include "objets/objet_store.h"
#include "deleter.h"
#include <algorithm>

struct pred_id {
	pred_id(int id) : id_(id) {}
	bool operator()(objet* o) {
		return o && o->id() == id_;
	}
private:
	int id_;
};

struct pred_at {
	pred_at(ci::Vec2f p) : p_(p) {}
	bool operator()(objet* o) {
		return o && o->contient(p_);
	}
private:
	ci::Vec2f p_;
};

struct pred_in {
	pred_in(ci::Rectf r) : r_(r) {}
	bool operator()(objet* o) {
		return o && o->est_dans(r_);
	}
private:
	ci::Rectf r_;
};

objet_store::objet_store() {}

objet_store::~objet_store() {
	std::for_each(objets_.begin(), objets_.end(), deleter<objet>());
}

objet* objet_store::get(int id) {
	iterator it = std::find_if(objets_.begin(), objets_.end(), pred_id(id));
	if(it == objets_.end()) {
		return 0;
	} else
		return *it;
}

objet& objet_store::get_or_create(int id) {
	objet* op = get(id);
	if(!op) {
		op = new objet(this, id);
		objets_.push_back(op);
	}
	return *op;
}
objet* objet_store::get_at(ci::Vec2f v) {
	iterator it = std::find_if(objets_.begin(), objets_.end(), pred_at(v));
	if(it == objets_.end()) {
		return 0;
	} else
		return *it;
}

objet* objet_store::get_in(ci::Rectf r) {
	iterator it = std::find_if(objets_.begin(), objets_.end(), pred_in(r));
	if(it == objets_.end()) {
		return 0;
	} else
		return *it;
}