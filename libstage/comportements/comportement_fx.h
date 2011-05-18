#if !defined(COMPORTEMENT_FX_H_IBJT4JGL)
#define COMPORTEMENT_FX_H_IBJT4JGL

#include "comportements/comportement.h"

class fx;
struct comportement_fx : public comportement {
	comportement_fx(objet& o);
	virtual ~comportement_fx();
	virtual void operator()() = 0;
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	static std::string titre() { return "comportement_fx"; }
	virtual type what() { return type_fx; }
	virtual liste_parametres* parametres();
protected:
	void set_fx(fx* s) { fx_ = s; }
private:
	fx* fx_;
};

#endif /* end of include guard: COMPORTEMENT_FX_H_IBJT4JGL */
