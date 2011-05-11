#if !defined(OBJET_STORE_H_PQ7R7H3Q)
#define OBJET_STORE_H_PQ7R7H3Q

#include "objets/objet.h"
#include <cinder/Rect.h>
#include <cinder/Vector.h>
#include <list>

struct objet_store {
	typedef std::list<objet*> liste_objets_t;
	typedef liste_objets_t::iterator iterator;
	
	objet_store();
	virtual ~objet_store();
	iterator begin() { return objets_.begin(); }
	iterator end() { return objets_.end(); }
	liste_objets_t objets() { return objets_; }
	objet* get(int id);
	objet& get_or_create(int id);
	objet* get_at(ci::Vec2f v);
	objet* get_in(ci::Rectf r);
private:
	liste_objets_t objets_;	
};

#endif /* end of include guard: OBJET_STORE_H_PQ7R7H3Q */
