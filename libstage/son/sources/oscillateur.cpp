#include "son/sources/oscillateur.h"
#include "son/opensoundcontrol.h"

oscillateur::oscillateur(float f) : ancienne_freq_(0) {
	// On crée la boîte dans dyn~
	creer();
	// On associe des callbacks aux paramètres
	freq_.callback(boost::bind(&oscillateur::frequence_changed, this));
	// On règle les paramètres par défaut
	freq_.set(440);
	// On remplit la liste des paramètres
	liste_parametres_.add(&freq_);
}

void oscillateur::creer() {
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << id().c_str() << "oscillateur" << osc::EndMessage);
	source::creer();
}

void oscillateur::frequence_changed() {
	if(freq_.get() != ancienne_freq_) {
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "freq" << freq_.get() << osc::EndMessage);
		ancienne_freq_ = freq_.get();
	}
}