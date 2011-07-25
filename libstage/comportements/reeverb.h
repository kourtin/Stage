#ifndef REEVERB_H_K0QSJK3H
#define REEVERB_H_K0QSJK3H

#include "comportements/comportement_fx.h"

class reverb;

struct reeverb : public comportement_fx {
	reeverb(objet& o);
	virtual ~reeverb() {}
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return titre(); }
	static std::string titre() { return std::string("reeverb"); }
protected:
	virtual void maj_objet_associe(objet*);
private:
	reverb* verb_;
};

#endif /* end of include guard: REEVERB_H_K0QSJK3H */
