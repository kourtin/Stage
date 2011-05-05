#if !defined(OBJET_STORE_H_PQ7R7H3Q)
#define OBJET_STORE_H_PQ7R7H3Q

#include "objet.h"
#include "deleter.h"
#include <list>
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
	pred_at(float x, float y) : x_(x), y_(y) {}
	bool operator()(objet* o) {
		return o && o->contient(x_,y_);
	}
private:
	float x_, y_;
};

struct pred_in {
	pred_in(float x1, float y1, float x2, float y2) : x1_(x1), y1_(y1), x2_(x2), y2_(y2) {}
	bool operator()(objet* o) {
		return o && o->est_dans(floatrect(x1_, y1_, x2_, y2_));
	}
private:
	float x1_, y1_, x2_, y2_;
};

struct objet_store {
	typedef std::list<objet*> liste_objets_t;
	typedef liste_objets_t::iterator iterator;
	
	objet_store() {}
	virtual ~objet_store() {
		std::for_each(objets_.begin(), objets_.end(), deleter<objet>());
	}
	iterator begin() { return objets_.begin(); }
	iterator end() { return objets_.end(); }
	liste_objets_t objets() { return objets_; }
	objet* get(int id) {
		iterator it = std::find_if(objets_.begin(), objets_.end(), pred_id(id));
		if(it == objets_.end()) {
			return 0;
		} else
			return *it;
	}
	objet& get_or_create(int id) {
		objet* op = get(id);
		if(!op) {
			op = new objet(this, id);
			objets_.push_back(op);
		}
		return *op;
	}
	objet* get_at(float x, float y) {
		iterator it = std::find_if(objets_.begin(), objets_.end(), pred_at(x,y));
		if(it == objets_.end()) {
			return 0;
		} else
			return *it;
	}
	
	objet* get_in(floatrect f) {
		return get_in(f.x1, f.y1, f.x2, f.y2);
	}
	
	objet* get_in(float x1, float y1, float x2, float y2) {
		iterator it = std::find_if(objets_.begin(), objets_.end(), pred_in(x1,y1,x2,y2));
		if(it == objets_.end()) {
			return 0;
		} else
			return *it;
	}
private:
	liste_objets_t objets_;	
};

#endif /* end of include guard: OBJET_STORE_H_PQ7R7H3Q */
