#if !defined(OBJET_ALPHA_H_DZTB9LIT)
#define OBJET_ALPHA_H_DZTB9LIT

#include "comportements/comportement.h"
#include "interface/menu.h"

struct objet_alpha2 : public comportement {
	objet_alpha2(objet& o);
	virtual ~objet_alpha2();
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return titre(); }
	static std::string titre() { return "objet alpha"; }
private:
	void menu_draw(ci::cairo::Context ctx, int w, int h);
	enum { etat_initial, etat_selection_possible, etat_selection_confirmee, etat_pourrait_accepter, etat_presque_accepte, etat_accepte };
	unsigned int etat_;
	objet* objet_selectionne_;
	double temporisation_;
	menu_edition_objet mnu_ed_;
	menu* menu_selectionne_;
	float pourcentage_initial_;
};

#endif /* end of include guard: OBJET_ALPHA_H_DZTB9LIT */
