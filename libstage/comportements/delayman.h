#ifndef DELAYMAN_H_85VPIR7C
#define DELAYMAN_H_85VPIR7C

#include "comportements/comportement_fx.h"

class simple_delay;

struct delayman : public comportement_fx {
	delayman(objet& o);
	virtual ~delayman() {}
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return titre(); }
	static std::string titre() { return std::string("delayman"); }
protected:
	virtual void maj_objet_associe(objet*);
private:
	simple_delay* delay_;
};

#endif /* end of include guard: DELAYMAN_H_85VPIR7C */
