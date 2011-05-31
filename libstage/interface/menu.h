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
	virtual bool commande() { return false; }
};

struct menu {
	typedef std::vector<element_menu*> liste_elements;
	typedef liste_elements::iterator iterator;
	menu(std::string titre = "");
	virtual ~menu();
	iterator begin() { return liste_.begin(); }
	iterator end() { return liste_.end(); }
	iterator iterator_at(unsigned int x);
	element_menu* at(unsigned int x) { return liste_.at(x); }
	unsigned int size() { return liste_.size(); }
	std::string titre() { return titre_; }
	virtual void confirmer(objet&);
	virtual void select(iterator s) { selection_ = s; }
	virtual void select(unsigned int x);
	virtual iterator selection() { return selection_; }
	unsigned int index_selection() { return std::distance(begin(), selection_); }
protected:
	liste_elements liste_;
	iterator selection_;
	std::string titre_;
};

struct elt_annuler : public element_menu {
	std::string titre() { return "Annuler"; }
	void operator()(objet& o) {}
	virtual bool commande() { return true; }
};

struct elt_detruire : public element_menu {
	std::string titre() { return "Détruire"; }
	void operator()(objet& o);
};

template <class T>
struct element_menu_ajouter_comportement : public element_menu {
	virtual std::string titre() { return T::titre(); }
	virtual void operator()(objet& o) {
		new T(o);
	}
};

struct menu_affectation_objet : public menu {
	static menu_affectation_objet& instance() {
		static menu_affectation_objet inst;
		return inst;
	}
	template <class T>
	void supporter_comportement() {
		element_menu* e = new element_menu_ajouter_comportement<T>;
		liste_.push_back(e);
	}
private:
	menu_affectation_objet();
};

struct menu_edition_objet : public menu {
	menu_edition_objet();
	virtual ~menu_edition_objet() {}
};

#endif /* end of include guard: MENU_H_L2D19Y1J */
