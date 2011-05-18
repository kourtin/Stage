#if !defined(OBJET_ALPHA_H_DZTB9LIT)
#define OBJET_ALPHA_H_DZTB9LIT

#include "comportements/comportement.h"
#include "interface/menu_draw.h"

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
};

struct objet_alpha : public comportement {
	objet_alpha(objet& o);
	virtual ~objet_alpha();
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return titre(); }
	static std::string titre() { return "objet alpha"; }
private:
	enum { etat_initial, etat_selection_possible, etat_selection_confirmee, etat_pourrait_accepter, etat_presque_accepte, etat_accepte };
	unsigned int etat_;
	objet* objet_selectionne_;
	double temporisation_;
	menu_edition_objet mnu_ed_;
	menu* menu_selectionne_;
	menu_draw* mnu_drw_;
};

#endif /* end of include guard: OBJET_ALPHA_H_DZTB9LIT */
