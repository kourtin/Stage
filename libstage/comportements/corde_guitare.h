#if !defined(CORDE_GUITARE_H_YFY96U1X)
#define CORDE_GUITARE_H_YFY96U1X

#include "comportements/comportement.h"
#include "son/sources/karplus_strong.h"

struct corde_guitare : public comportement_source {
	corde_guitare(objet& o);
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return titre(); }
	static std::string titre() { return "corde de guitare"; }
private:
	karplus_strong karplus_;
};

#endif /* end of include guard: CORDE_GUITARE_H_YFY96U1X */
