#if !defined(MENU_H_L2D19Y1J)
#define MENU_H_L2D19Y1J

#include "objets/objet.h"
#include <string>
#include <vector>

struct element_menu {
	element_menu() {}
	virtual ~element_menu() {}
	virtual void operator()(objet&) = 0;
	virtual std::string titre() = 0;
};

struct menu {
	typedef std::vector<element_menu*> liste_elements;
	typedef liste_elements::iterator iterator;
	menu(std::string titre);
	virtual ~menu();
	iterator begin() { return liste_.begin(); }
	iterator end() { return liste_.end(); }
	unsigned int size() { return liste_.size(); }
	std::string titre() { return titre_; }
	virtual void confirmer(objet&);
	virtual void select(iterator s) { selection_ = s; }
	virtual iterator selection() { return selection_; }
protected:
	liste_elements liste_;
	iterator selection_;
	std::string titre_;
};

#endif /* end of include guard: MENU_H_L2D19Y1J */
