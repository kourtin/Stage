#if !defined(OBJET_ALPHA_H_DZTB9LIT)
#define OBJET_ALPHA_H_DZTB9LIT

#include "comportement.h"

struct objet_alpha : public comportement {
	objet_alpha(objet& o);
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return "objet alpha"; }
private:
	enum { etat_initial, etat_selection_possible, etat_selection_confirmee, etat_pourrait_accepter, etat_presque_accepte, etat_accepte };
	unsigned int etat_;
	objet* objet_selectionne_;
	double temporisation_;
	
};

#endif /* end of include guard: OBJET_ALPHA_H_DZTB9LIT */
