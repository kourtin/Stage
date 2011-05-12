#if !defined(COMPORTEMENT_H_U8D66Y9P)
#define COMPORTEMENT_H_U8D66Y9P

#include "objets/objet.h"
#include "objets/objet_store.h"
#include "son/parametres.h"
#include <cinder/cairo/Cairo.h>
#include <string>

double temps_actuel();

struct comportement {
	enum type { type_autre, type_source, type_fx, type_ctrl, type_tool };
	comportement(objet& o);
	virtual ~comportement();
	virtual void operator()() = 0; // action
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	objet& objet_attache() { return objet_; }
	virtual std::string classe() = 0;
	static std::string titre() { return "comportement"; }
	virtual type what() { return type_autre; }
protected:
	virtual void draw_params(ci::cairo::Context ctx, int w, int h, liste_parametres* l);
private:
	objet& objet_;
};

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

class processus_sonore;
struct comportement_ctrl : public comportement {
	comportement_ctrl(objet& o);
	virtual ~comportement_ctrl();
	virtual void operator()() = 0;
	static std::string titre() { return "comportement_ctrl"; }
	virtual type what() { return type_ctrl; }
protected:
	void set_processus_sonore(processus_sonore* p) { p_ = p; }
private:
	processus_sonore* p_;
};

struct comportement_tool : public comportement {
	comportement_tool(objet& o);
	virtual ~comportement_tool();
	virtual void operator()() = 0;
	static std::string titre() { return "comportement_tool"; }
	virtual type what() { return type_tool; }
};

// struct source : public comportement {
// 	source(objet& o) : comportement(o) {}
// 	virtual ~source() {}
// 	virtual void operator()() = 0;
// 	
// };
// 
// struct fx : public source {
// 	
// };
// 
// struct controleur : public comportement {
// 	controleur(source* s) : 
// };
// 
// 
// 
// struct sampler : public comportement {
// 	sampler(objet& o) : comportement(o) {}
// 	virtual ~sampler() {}
// 	
// private:
// 	
// };

#endif /* end of include guard: COMPORTEMENT_H_U8D66Y9P */
