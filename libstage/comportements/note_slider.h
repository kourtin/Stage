#if !defined(NOTE_SLIDER_H_D8W5MOYI)
#define NOTE_SLIDER_H_D8W5MOYI

#include "comportements/comportement.h"

struct note_slider : public comportement_ctrl {
	note_slider(objet& o);
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	virtual std::string classe() { return titre(); }
	static std::string titre() { return "note slider"; }
};

#endif /* end of include guard: NOTE_SLIDER_H_D8W5MOYI */
