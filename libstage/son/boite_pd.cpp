#include "son/boite_pd.h"
#include "son/opensoundcontrol.h"

boite_pd::boite_pd(std::string string_id) : string_id_(string_id) {}

boite_pd::~boite_pd() {
	// On déconnecte toutes les boîtes reliées à l'inlet
	iterator it = inlets().begin();
	while(it != inlets().end()) {
		boite_pd* b = *it;
		b->deconnecter_outlet0(this);
		it = inlets().begin();
	}
	// On déconnecte toutes les boîtes reliées à l'outlet
	for(iterator it = outlets().begin(); it != outlets().end(); ++it) {
		boite_pd* b = *it;
		if(b != 0) {
			b->deconnecter_inlet0(this);
		}
	}
	
	if(id().compare("") == 0)
		return;
	
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyndelay") << delai_coupure() << "del" << id().c_str() << osc::EndMessage);
}

void boite_pd::connecter_outlet0(boite_pd* dest) {
	if(!dest) return;
	// Cherche s'il est déjà connecté
	iterator it = std::find(outlets().begin(), outlets().end(), dest);
	if(it == outlets().end()) {
		// Connecte cet outlet 0 à l'inlet 0 de dest
		dest->connecter_inlet0(this);
		// Rajoute à la liste des outlets
		outlets().push_back(dest);
	}
}

void boite_pd::deconnecter_outlet0(boite_pd* dest) {
	if(!dest) return;
	// Cherche s'il est déjà connecté
	iterator it = std::find(outlets().begin(), outlets().end(), dest);
	if(it != outlets().end()) {
		// Déconnecte cet outlet 0 de l'inlet 0 de dest
		dest->deconnecter_inlet0(this);
		// Enlève de la liste des outlets
		outlets().remove(dest);
	}
}

void boite_pd::connecter_inlet0(boite_pd* src) {
	if(!src) return;
	// Cherche s'il est pas déjà connecté
	iterator it = std::find(inlets().begin(), inlets().end(), src);
	if(it == inlets().end()) {
		// Connecte l'outlet 0 de src à cet inlet 0 dans dyn~
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "conn" << (src->id().c_str()) << 0 << id().c_str() << 0 << osc::EndMessage);
		// Rajoute à la liste des inlets
		inlets().push_back(src);
	}
}

void boite_pd::deconnecter_inlet0(boite_pd* src) {
	if(!src) return;
	// Cherche s'il est déjà connecté
	iterator it = std::find(inlets().begin(), inlets().end(), src);
	if(it != inlets().end()) {
		// Déconnecte l'outlet 0 de src de cet inlet 0 dans dyn~
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyndelay") << delai_coupure() << "dis" << src->id().c_str() << 0 << id().c_str() << 0 << osc::EndMessage);
		// Enlève de la liste des inlets
		inlets().remove(src);
	}
}