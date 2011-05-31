#if !defined(COMPORTEMENT_H_U8D66Y9P)
#define COMPORTEMENT_H_U8D66Y9P

#include "objets/objet.h"
#include "objets/objet_store.h"
#include "son/parametres.h"
#include <cinder/cairo/Cairo.h>
#include <string>

struct comportement {
	enum type { type_autre, type_source, type_fx, type_ctrl, type_tool };
	comportement(objet& o);
	virtual ~comportement();
	virtual void operator()() = 0; // action
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	objet& objet_attache() { return objet_; }
	virtual std::string classe() = 0;
	static std::string titre() { return "comportement"; }
	virtual type what() { return type_autre; }
	virtual liste_parametres* parametres() { return 0; }
	virtual ci::Color couleur() { return couleur_; }
	virtual void couleur(ci::Color c) { couleur_ = c;}
	virtual void couleur(bool) { couleur_ = ci::Color(0.5, 0.5, 0.5); }
	virtual bool provisoire() { return false; }
protected:
	virtual void draw_params(ci::cairo::Context ctx, int w, int h, liste_parametres* l);
	bool afficher_nom_;
private:
	objet& objet_;
	ci::Color couleur_;
};

#endif /* end of include guard: COMPORTEMENT_H_U8D66Y9P */
