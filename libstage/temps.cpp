#include "temps.h"
#include <ctime>

double temps_actuel() {
	return std::clock() * 1000.0 / CLOCKS_PER_SEC;
}