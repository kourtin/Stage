#include "son/sources/line_in.h"
#include "son/opensoundcontrol.h"

line_in::line_in(unsigned int n) : channel_(n) {
	creer();
}

void line_in::creer() {
	osc_send::instance().send(osc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << id().c_str() << "adc~" << (char)channel_ << osc::EndMessage);
	source::creer();
}