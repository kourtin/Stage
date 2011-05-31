#ifndef VARIATEUR_H_J35W9QH4
#define VARIATEUR_H_J35W9QH4

#include "comportements/comportement_ctrl.h"

struct variateur_parametre : public comportement_ctrl {
	variateur_parametre(objet& o, std::string parametre);
	virtual ~variateur_parametre() {}
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual void operator()();
	static std::string titre() { return std::string("variateur parametre"); }
protected:
	virtual void maj_objet_associe(objet*);
private:
	std::string parametre_;
};

template <class T>
struct variateur : public variateur_parametre {
	variateur(objet& o) : variateur_parametre(o, T::id()) {}
	virtual ~variateur() {}
	virtual std::string classe() { return titre(); }
	static std::string titre() { return std::string("variateur ") + T::id(); }
};

#endif /* end of include guard: VARIATEUR_H_J35W9QH4 */
