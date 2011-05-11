#include "son/source.h"
#include "son/master_pd.h"

source::source() : processus_sonore(compute_string_id("source", next_unique_id()), 0) {}

source::~source() {}

void source::creer() {
	processus_sonore::creer();
	
	// La source commence connectée au master
	connecter_outlet0(&(master_pd::instance()));
}