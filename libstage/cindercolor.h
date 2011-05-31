#ifndef CINDERCOLOR_H_UW4103N5
#define CINDERCOLOR_H_UW4103N5

#include <cinder/Rand.h>
#include <cinder/Color.h>

ci::Color randColor() {
	static bool randomized = false;
	if(!randomized) {
		randomized = true;
		ci::Rand::randomize();
	}
	return ci::Color(ci::CM_HSV, ci::Rand::randFloat(), 0.5f, 1.f);
}

#endif /* end of include guard: CINDERCOLOR_H_UW4103N5 */
