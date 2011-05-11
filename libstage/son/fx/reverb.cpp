#include "son/fx/reverb.h"
#include "son/opensoundcontrol.h"

reverb::reverb(processus_sonore* from, boite_pd* to) : fx(from, to) {
	// On crée la boîte dans dyn~
	creer();
}

void reverb::creer() {
	// Crée la freeverb
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << id().c_str() << "freeverb~" << osc::EndMessage);
	// Paramètres par défaut
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << id().c_str() << "wet" << 1 << osc::EndMessage);
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << id().c_str() << "dry" << 0 << osc::EndMessage);
	fx::creer();
}