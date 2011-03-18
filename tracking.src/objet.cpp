#include "objet.h"
#include <limits>

void objet::actualiser(std::vector<blobby>& v) {
	blobby* blob_min = 0;
	float min = std::numeric_limits<float>::max();
	for(std::vector<blobby>::iterator it2 = v.begin(); it2 != v.end(); ++it2) {
		blobby& b = *it2;
		float d = b.distance(blob_);
		if(d < min) {
			min = d;
			blob_min = &b;
		}
	}
	// TODO: Etudier l'évolution de la distance avec une gaussienne (moyenne, variance)
	if(distance_avant_ == -1)
		distance_avant_ = min;
	else if(std::abs(min - distance_avant_) < 10)
		if(blob_min)
			blob_ = *blob_min;
}