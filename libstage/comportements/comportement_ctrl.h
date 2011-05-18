#if !defined(COMPORTEMENT_CTRL_H_AC1F58T0)
#define COMPORTEMENT_CTRL_H_AC1F58T0

#include "comportements/comportement.h"

class processus_sonore;
struct comportement_ctrl : public comportement {
	comportement_ctrl(objet& o);
	virtual ~comportement_ctrl();
	virtual void operator()();
	static std::string titre() { return "comportement_ctrl"; }
	virtual type what() { return type_ctrl; }
	virtual void draw(ci::cairo::Context ctx, int w, int h);
	virtual liste_parametres* parametres() { return &params_; }
protected:
	void objet_associe(objet* o) { o_ = o; }
	objet* objet_associe() { return o_; }
	virtual bool accepte(objet&);
	virtual void maj_objet_associe(objet* ancien) {};
private:
	objet* o_, *o_promis_;
	bool detacher_;
	double temporisation_;
	liste_parametres params_;
};

#endif /* end of include guard: COMPORTEMENT_CTRL_H_AC1F58T0 */
