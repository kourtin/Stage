#include "son/sources/oscillateur.h"
#include "son/opensoundcontrol.h"

oscillateur::oscillateur(float f) : freq_(f), vol_(1.0) {
	// On crée la boîte dans dyn~
	creer();
}

void oscillateur::creer() {
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << id().c_str() << "oscillateur" << osc::EndMessage);
	source::creer();
}

void oscillateur::frequence(float f) {
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "freq" << f << osc::EndMessage);
	freq_ = f;
}

void oscillateur::volume(float v) {
	v = std::max(0.f, v);
	v = std::min(v, 1.f);
	vol_ = v;
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "vol" << v << osc::EndMessage);
}