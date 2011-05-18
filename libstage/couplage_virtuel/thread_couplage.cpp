#include "couplage_virtuel/thread_couplage.h"

void thread_couplage::run_thread() {
	for(;;) {
		// Actualise les calculs
		for(liste_couplages::iterator it = couplages_.begin(); it != couplages_.end(); ++it)
			(*it)->operator()();
		// Attend 1ms
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}
}