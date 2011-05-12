#if !defined(MARTEAU_H_C9GVQN5E)
#define MARTEAU_H_C9GVQN5E

#include "comportements/comportement.h"
#include <list>

struct marteau : public comportement_tool {
	typedef std::list<objet*> liste_objets;
	marteau(objet& o);
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return titre(); }
	static std::string titre() { return "marteau"; }
private:
	liste_objets deja_en_collision_;
};

#endif /* end of include guard: MARTEAU_H_C9GVQN5E */
