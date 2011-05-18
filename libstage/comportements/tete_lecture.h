#if !defined(TETE_LECTURE_H_AS09N7HG)
#define TETE_LECTURE_H_AS09N7HG

#include "comportements/comportement_tool.h"
#include <list>

struct tete_lecture : public comportement_tool {
	typedef std::list<objet*> liste_objets;
	tete_lecture(objet& o);
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return titre(); }
	static std::string titre() { return "tete de lecture"; }
private:
	liste_objets deja_en_collision_;
};

#endif /* end of include guard: TETE_LECTURE_H_AS09N7HG */
