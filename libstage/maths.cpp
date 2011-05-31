#include "maths.h"
#include <cmath>

void angles_euler(float m[3][4], float& x, float& y, float& z) {
	if(std::abs(m[2][0]) != 1) {
		float th1 = -asin(m[2][0]);
		// float th2 = M_PI - th1;
		float psi1 = atan2(m[2][1] / cos(th1), m[2][2] / cos(th1));
		// float psi2 = atan2(m[2][1] / cos(th2), m[2][2] / cos(th2));
		float phi1 = atan2(m[1][0] / cos(th1), m[0][0] / cos(th1));
		// float phi2 = atan2(m[1][0] / cos(th2), m[0][0] / cos(th2));
		x = psi1;
		y = th1;
		z = phi1;
	} else {
		z = 0;
		if(m[2][0] == -1) {
			y = M_PI / 2;
			x = z + atan2(m[0][1], m[0][2]);
		} else {
			y = -M_PI / 2;
			x = -z + atan2(-m[0][1], -m[0][2]);
		}
	}
}