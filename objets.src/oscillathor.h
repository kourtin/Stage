#if !defined(OSCILLATHOR_H_ZR1XVAN5)
#define OSCILLATHOR_H_ZR1XVAN5

#include "comportement.h"
#include "processus_sonores.h"

struct oscillathor : public comportement {
	oscillathor(objet& o);
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return "oscillathor"; }
private:
	oscillateur oscil_;
	float frequence_, volume_;
	// line_in line;
	// simple_delay d1_;
};

#endif /* end of include guard: OSCILLATHOR_H_ZR1XVAN5 */
