#if !defined(OSCILLATHOR_H_ZR1XVAN5)
#define OSCILLATHOR_H_ZR1XVAN5

#include "comportements/comportement.h"
#include "son/sources/oscillateur.h"
#include "son/sources/line_in.h"

struct oscillathor : public comportement_source {
	oscillathor(objet& o);
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return titre(); }
	static std::string titre() { return "oscillathor"; }
private:
	oscillateur oscil_;
};

struct micro : public comportement_source {
	micro(objet& o);
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return titre(); }
	static std::string titre() { return "micro"; }
private:
	line_in line_;
};

#endif /* end of include guard: OSCILLATHOR_H_ZR1XVAN5 */
