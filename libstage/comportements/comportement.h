#if !defined(COMPORTEMENT_H_U8D66Y9P)
#define COMPORTEMENT_H_U8D66Y9P

#include "objets/objet.h"
#include "objets/objet_store.h"
#include <cinder/cairo/Cairo.h>
#include <string>

double temps_actuel();

struct comportement {
	comportement(objet& o);
	virtual ~comportement();
	virtual void operator()() = 0; // action
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	objet& objet_attache() { return objet_; }
	virtual std::string classe() = 0;
	static std::string titre() { return "comportement"; }
private:
	objet& objet_;
};

// struct source : public comportement {
// 	source(objet& o) : comportement(o) {}
// 	virtual ~source() {}
// 	virtual void operator()() = 0;
// 	
// };
// 
// struct fx : public source {
// 	
// };
// 
// struct controleur : public comportement {
// 	controleur(source* s) : 
// };
// 
// 
// 
// struct sampler : public comportement {
// 	sampler(objet& o) : comportement(o) {}
// 	virtual ~sampler() {}
// 	
// private:
// 	
// };

#endif /* end of include guard: COMPORTEMENT_H_U8D66Y9P */
