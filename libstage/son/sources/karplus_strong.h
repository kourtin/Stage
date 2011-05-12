#if !defined(KARPLUS_STRONG_H_6PWTMYG9)
#define KARPLUS_STRONG_H_6PWTMYG9

#include "son/source.h"

struct karplus_strong : public source {
	karplus_strong();
	virtual void creer();
	parametre_note& note() { return note_; }
	void bang() { return bang_.set(1); }
	virtual std::string classe() { return "karplus-strong"; }
	void note_changed();
	void banged();
private:
	parametre_note note_;
	parametre_bang bang_;
};

#endif /* end of include guard: KARPLUS_STRONG_H_6PWTMYG9 */
