#include "son/sources/karplus_strong.h"
#include "son/opensoundcontrol.h"

karplus_strong::karplus_strong() : ancienne_note_(0) {
	// On crée la boîte dans dyn~
	creer();
	// On associe des callbacks aux paramètres
	note_.callback(boost::bind(&karplus_strong::note_changed, this));
	bang_.callback(boost::bind(&karplus_strong::banged, this));
	// On règle les paramètres par défaut
	note_.set(69);
	// On remplit la liste des paramètres
	liste_parametres_.add(&note_);
	liste_parametres_.add(&bang_);
}

void karplus_strong::creer() {
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << id().c_str() << "karplus_strong" << osc::EndMessage);
	source::creer();
}

void karplus_strong::note_changed() {
	if(note_.get() != ancienne_note_) {
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "note" << note_.get() << osc::EndMessage);
		ancienne_note_ = note_.get();
	}
}

void karplus_strong::banged() {
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "bang" << osc::EndMessage);
}