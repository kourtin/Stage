#include "son/fx/simple_delay.h"
#include "son/opensoundcontrol.h"

simple_delay::simple_delay(processus_sonore* from, boite_pd* to) : fx(from, to), delai_(100.0), wet_(0.33) {
	// On crée la boîte dans dyn~
	creer();
	// On la paramètre
	delai(delai_);
	wet(wet_);
}

void simple_delay::creer() {
	// Crée la freeverb
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << id().c_str() << "simpledelay" << osc::EndMessage);
	fx::creer();
}

void simple_delay::delai(float d) {
	delai_ = d;
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "delay" << delai_ << osc::EndMessage);
}

void simple_delay::wet(float w) {
	wet_ = w;
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "wet" << wet_ << osc::EndMessage);
}

void simple_delay::feedback(float f) {
	feedback_ = f;
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "feedback" << feedback_ << osc::EndMessage);
}