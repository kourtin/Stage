#include "son/fx.h"

fx::fx(processus_sonore* from, boite_pd* to) : processus_sonore(compute_string_id("fx", next_unique_id()), 1), from_(from), to_(to) {}

fx::~fx() {}

void fx::creer() {
	processus_sonore::creer();
	// Le from est déchu de sa connection avec le to
	from_->deconnecter_outlet0(to_);
	// L'effet commence connecté au from et au to
	from_->connecter_outlet0(this);
	connecter_outlet0(to_);
}