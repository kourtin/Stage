#if !defined(COMPORTEMENT_SOURCE_H_GA3W12W6)
#define COMPORTEMENT_SOURCE_H_GA3W12W6

#include "comportements/comportement.h"

class source;
struct comportement_source : public comportement {
	comportement_source(objet& o);
	virtual ~comportement_source();
	virtual void operator()() = 0;
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	static std::string titre() { return "comportement_source"; }
	virtual type what() { return type_source; }
	virtual liste_parametres* parametres();
protected:
	void set_source(source* s) { source_ = s; }
private:
	source* source_;
};

#endif /* end of include guard: COMPORTEMENT_SOURCE_H_GA3W12W6 */
