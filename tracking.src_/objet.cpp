#include "objet.h"
#include <limits>

objet::objet(blobby& b) : nom_("@@@@@@@@@"), blob_(b), distance_avant_(-1), distance_initiale_(-1) {}

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
	if(present_) {
		if(blob_min && blob_.id_ == blob_min->id_) {
			blob_ = *blob_min;
		}
		else {
			present_ = false;
		}
	} else {
		if(min < 0.6) {
			blob_ = *blob_min;
			present_ = true;
		}
	}
}