#ifndef SHAKE_MENU_H_40JTHE8Q
#define SHAKE_MENU_H_40JTHE8Q

#include "comportements/comportement.h"
#include "interface/shaker.h"
#include "interface/menu.h"

struct shake_comportement : public comportement {
	shake_comportement(objet& o);
	virtual ~shake_comportement();
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return titre(); }
	static std::string titre() { return "shake me"; }
	virtual bool provisoire() { return true; }
private:
	void menu_draw(ci::cairo::Context ctx, int w, int h);
	unsigned int nb_pages();
	unsigned int idx_premier_element_page_actuelle();
	unsigned int nb_elements_page_actuelle();
	float angle_1_element_page_actuelle();
	
	enum { etat_initial, etat_secoue, etat_affiche_menu, etat_secoue_confirme, etat_accepte };
	
	unsigned int etat_;
	
	double temporisation_;
	menu* menu_selectionne_;
	unsigned int selection_avant_;
	
	shaker maracas_;
	unsigned int numero_page_;
	float x_, y_, rayon_, rayon_int_, rayon_page_;
	bool page_moins_sel_, page_plus_sel_, page_moins_sel_avant_, page_plus_sel_avant_;
};

#endif /* end of include guard: SHAKE_MENU_H_40JTHE8Q */
