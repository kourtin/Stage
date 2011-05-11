#include "son/processus_sonore.h"
#include "son/opensoundcontrol.h"

processus_sonore::processus_sonore(std::string string_id, unsigned int control_inlet) : boite_pd(string_id), control_inlet_(control_inlet) {}

processus_sonore::~processus_sonore() {
	// Déconnecte le proxy
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "dis" << proxy_id().c_str() << 0 << id().c_str() << (char)control_inlet_ << osc::EndMessage);
	
	// Détruit le proxy
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "del" << proxy_id().c_str() << osc::EndMessage);
	
	// Connecte tous ses inlets à tous ses outlets, pour remplacer son absence.
	for(iterator it = inlets().begin(); it != inlets().end(); ++it) {
		boite_pd* inlet_actuel = *it;
		if(!inlet_actuel) continue; // Parafoudre
		for(iterator it2 = outlets().begin(); it2 != outlets().end(); ++it2) {
			boite_pd* outlet_actuel = *it2;
			inlet_actuel->connecter_outlet0(outlet_actuel);
		}
	}
}

void processus_sonore::creer() {
	// Crée le proxy
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << proxy_id().c_str() << "list" << osc::EndMessage);
	// Connectons l'outlet du proxy à notre inlet
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "conn" << proxy_id().c_str() << 0 << id().c_str() << (char)control_inlet_ << osc::EndMessage);
}