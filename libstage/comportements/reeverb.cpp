#include "reeverb.h"
#include "son/fx/reverb.h"
#include "comportement_source.h"
#include "son/source.h"

reeverb::reeverb(objet& o) : comportement_fx(o), verb_(0) {}

void reeverb::draw(ci::cairo::Context ctx, int w, int h) {
	comportement_fx::draw(ctx, w, h);
}

void reeverb::operator()() {
	comportement_fx::operator()();
	if(objet_associe() && objet_associe()->est_attache() && verb_) {
		// delay_->delai(1500 * objet_attache().x());
		// delay_->wet(objet_attache().y());
	}
}

void reeverb::maj_objet_associe(objet* ancien) {
	if(verb_) {
		delete verb_;
		verb_ = 0;
	}
	if(objet_associe()) {
		verb_ = new reverb(
			(dynamic_cast<comportement_source*>(objet_associe()->comportement_attache()))->get_source()
		);
	}
}