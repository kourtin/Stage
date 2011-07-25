#include "delayman.h"
#include "son/fx/simple_delay.h"
#include "comportement_source.h"
#include "son/source.h"

delayman::delayman(objet& o) : comportement_fx(o), delay_(0) {}

void delayman::draw(ci::cairo::Context ctx, int w, int h) {
	comportement_fx::draw(ctx, w, h);
}

void delayman::operator()() {
	comportement_fx::operator()();
	if(objet_associe() && objet_associe()->est_attache() && delay_) {
		// parametre* p = objet_associe()->comportement_attache()->parametres()->get_param(parametre_);
		// if(p) {
			// double note = p->min() + objet_attache().y() * (p->max() - p->min());
			// p->set(note);
		// }  
		delay_->delai(1500 * objet_attache().x());
		delay_->wet(objet_attache().y());
	}
}

void delayman::maj_objet_associe(objet* ancien) {
	if(delay_) {
		delete delay_;
		delay_ = 0;
	}
	
	if(objet_associe()) {
		delay_ = new simple_delay(
			(dynamic_cast<comportement_source*>(objet_associe()->comportement_attache()))->get_source()
		);
		delay_->feedback(0.4);
	}
}