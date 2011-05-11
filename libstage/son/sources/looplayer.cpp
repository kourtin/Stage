#include "son/sources/looplayer.h"
#include "son/opensoundcontrol.h"

looplayer::looplayer() {
	// On crée la boîte dans dyn~
	creer();
}

looplayer::~looplayer() {}

void looplayer::creer() {
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << id().c_str() << "looplayer" << osc::EndMessage);
	source::creer();
}

void looplayer::start() {
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "play" << 1 << osc::EndMessage);
}

void looplayer::stop() {
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "play" << 0 << osc::EndMessage);
}