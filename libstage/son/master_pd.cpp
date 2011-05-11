#include "son/master_pd.h"
#include "son/opensoundcontrol.h"

master_pd::master_pd() : boite_pd("") {
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "reset" << osc::EndMessage);
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/semicolon") << "pd" << "dsp" << 1 << osc::EndMessage);
}

master_pd::~master_pd() {
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "reset" << osc::EndMessage);
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/semicolon") << "pd dsp 0" << osc::EndMessage);
}

master_pd& master_pd::instance() {
	static master_pd* singleton = new master_pd;
	return *singleton;
}

void master_pd::connecter_inlet0(boite_pd* src) {
	if(!src) return;
	// Cherche s'il est pas déjà connecté
	iterator it = std::find(inlets().begin(), inlets().end(), src);
	if(it == inlets().end()) {
		// Crée un atténuateur line avec un grain rate de 2ms
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << boite_pd::compute_string_id(src->id(), "_line").c_str() << "line" << 0 << 2 << osc::EndMessage);
		// Crée un amplificateur
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << boite_pd::compute_string_id(src->id(), "_gain").c_str() << "*~" << 0 << osc::EndMessage);
		// Connecte l'amplificateur à src
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "conn" << (src->id().c_str()) << 0 << boite_pd::compute_string_id(src->id(), "_gain").c_str() << 0 << osc::EndMessage);
		// Connecte l'atténuateur à l'amplificateur
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "conn" << boite_pd::compute_string_id(src->id(), "_line").c_str() << 0 << boite_pd::compute_string_id(src->id(), "_gain").c_str() << 1 << osc::EndMessage);
		// Connecte l'outlet 0 de l'ampli à l'inlet 0 du master dans dyn~
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "conn" << boite_pd::compute_string_id(src->id(), "_gain").c_str() << 0 << 0 << osc::EndMessage);
		// Envoie le volume progressivement
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << boite_pd::compute_string_id(src->id(), "_line").c_str() << 1 << delai_coupure() << osc::EndMessage);
		// Rajoute à la liste des inlets
		inlets().push_back(src);
	}
}
void master_pd::deconnecter_inlet0(boite_pd* src) {
	if(!src) return;
	// Cherche s'il est déjà connecté
	iterator it = std::find(inlets().begin(), inlets().end(), src);
	if(it != inlets().end()) {
		// Baisse le volume progressivement
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << boite_pd::compute_string_id(src->id(), "_line").c_str() << 0 << delai_coupure() << osc::EndMessage);
		// Déconnectera l'outlet 0 de l'amplificateur de l'inlet 0 du master dans dyn~
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyndelay") << delai_coupure() << "dis" << boite_pd::compute_string_id(src->id(), "_gain").c_str() << 0 << 0 << osc::EndMessage);
		// Détruira l'atténuateur
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyndelay") << delai_coupure() << "del" << boite_pd::compute_string_id(src->id(), "_line").c_str() << osc::EndMessage);
		// Détruira l'amplificateur
		osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyndelay") << delai_coupure() << "del" << boite_pd::compute_string_id(src->id(), "_gain").c_str() << osc::EndMessage);
		// Enlève de la liste des inlets
		inlets().remove(src);
	}
}