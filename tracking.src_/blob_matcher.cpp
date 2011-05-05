#include "blob_matcher.h"
#include <limits>
#include <algorithm>

void blob_matcher::operator()(std::vector<blobby>& blobs) {
	if(blobs_avant_.size() != 0) {
		// Construction de la matrice de distances
		construire_matrice(blobs);
		// Association naïve
		association_park_aggarwal(blobs);
	} else {
		// Initialisation des identifiants des blobs
		for(std::vector<blobby>::iterator it = blobs.begin(); it != blobs.end(); ++it)
			it->id_ = blobby::next_id();
	}
	
	// Copie des blobs
	blobs_avant_ = blobs;
}

bool plusgrand(blobby* i, blobby* j) { 
	return i->aire() > j->aire();
}

template <class T>
T* pointeur(T& v) { return &v; }

// tiré de S. Park and J. K. Aggarwal, “Segmentation and Tracking of Interacting Human Body Parts under Occlusion and Shadowing,” in MOTION '02: Proceedings of the Workshop on Motion and Video Computing, 2002, p. 105-111.
void blob_matcher::association_park_aggarwal(std::vector<blobby>& blobs) {
	// 1. Trier les blobs en fonction de leur aire
	std::vector<blobby*> blobs_aire;
	std::transform(blobs.begin(), blobs.end(), std::back_inserter(blobs_aire), pointeur<blobby>);
	std::sort(blobs_aire.begin(), blobs_aire.end(), plusgrand);
	
	std::vector<blobby*> blobs_remaining(blobs_aire);
	std::vector<blobby*> tracks_remaining;
	std::transform(blobs_avant_.begin(), blobs_avant_.end(), std::back_inserter(tracks_remaining), pointeur<blobby>);
	std::vector<blobby*> tracks_matched, blobs_matched;
	

	// 2. Fait une première passe d'association
	std::vector<bool> deja_fait(blobs_avant_.size(), false);
	for(size_t i=0; i<blobs_aire.size(); ++i) {
		int min = -1;
		double dist_min = std::numeric_limits<double>::max();
		for(size_t j=0; j<matrice_distances_.size2(); ++j) {
			float dist = blobs_aire[i]->distance(blobs_avant_[j]);
			if(dist < dist_min && !deja_fait[j]) {
				dist_min = dist;
				min = j;
			}
		}
		if(min != -1) {
			// On fait l'association
			blobs_aire[i]->id_ = blobs_avant_[min].id_;
			if(!deja_fait[min]) {
				// Le blob minimum a été trouvé
				deja_fait[min] = true;
				// On supprime le track de la liste des tracks restants
				std::vector<blobby*>::iterator t = std::find(tracks_remaining.begin(), tracks_remaining.end(), &(blobs_avant_[min]));
				tracks_remaining.erase(t);
				tracks_matched.push_back(&(blobs_avant_[min]));
				// Pareil pour le blob
				t = std::find(blobs_remaining.begin(), blobs_remaining.end(), blobs_aire[i]);
				blobs_remaining.erase(t);
				blobs_matched.push_back(blobs_aire[min]);
			}
		} else {
			// Pas de blob trouvé, ce blob est donc nouveau
			blobs_aire[i]->id_ = blobby::next_id();
		}
	}
	
	// 3. On associe maintenant les tracks déjà associés et les blobs non déjà associés
	return;
	std::fill(deja_fait.begin(), deja_fait.end(), false);
	for(size_t i=0; i<blobs_remaining.size(); ++i) {
		int min = -1;
		double dist_min = std::numeric_limits<double>::max();
		for(size_t j=0; j<tracks_matched.size(); ++j) {
			float dist = blobs_remaining[i]->distance(*tracks_matched[j]);
			if(j < dist_min && !deja_fait[j]) {
				dist_min = dist;
				min = j;
			}
		}
		if(min != -1) {
			// Le blob minimum a été trouvé
			deja_fait[min] = true;
			// On fait l'association
			blobs_remaining[i]->id_ = tracks_matched[min]->id_;
		} //else
			// blobs_remaining[i]->id_ = blobby::next_id();
	}
	// 4. On associe maintenant les tracks non déjà associés et les blobs déjà associés
}

void blob_matcher::association_naive(std::vector<blobby>& blobs) {
	// On garde la trace des blobs d'avant déjà associés
	std::vector<bool> deja_fait(blobs_avant_.size(), false);
	// On a les blobs à
	for(size_t i=0; i<matrice_distances_.size1(); ++i) {
		int min = -1;
		double dist_min = std::numeric_limits<double>::max();
		for(size_t j=0; j<matrice_distances_.size2(); ++j) {
			if(matrice_distances_(i,j) < dist_min && !deja_fait[j]) {
				dist_min = matrice_distances_(i,j);
				min = j;
			}
		}
		if(min != -1) {
			// Le blob minimum a été trouvé
			deja_fait[min] = true;
			// On fait l'association
			blobs[i].id_ = blobs_avant_[min].id_;
		} else {
			// Pas de blob trouvé, ce blob est donc nouveau
			blobs[i].id_ = blobby::next_id();
		}
	}
}

void blob_matcher::association_naive2(std::vector<blobby>& blobs) {
	// On garde la trace des blobs d'avant déjà associés
	std::vector<int> deja_fait(blobs_avant_.size(), -1);
	
	// On a les blobs à
	for(size_t i=0; i<matrice_distances_.size1(); ++i) {
		int min = -1;
		double dist_min = std::numeric_limits<double>::max();
		for(size_t j=0; j<matrice_distances_.size2(); ++j) {
			if(matrice_distances_(i,j) < dist_min) {
				dist_min = matrice_distances_(i,j);
				min = j;
			}
		}
		if(deja_fait[min] == -1) {
			// Le blob minimum a été trouvé
			deja_fait[min] = i;
			// On fait l'association
			blobs[i].id_ = blobs_avant_[min].id_;
		} else {
			// Le blob minimum est déjà assigné, on le "désassigne"
			blobs[static_cast<size_t>(deja_fait[static_cast<size_t>(min)])].id_ = blobby::next_id();
			deja_fait[min] = i;
			// On fait l'association
			blobs[i].id_ = blobs_avant_[min].id_;
		}
	}
}

void blob_matcher::construire_matrice(std::vector<blobby>& blobs) {
	matrice_distances_ = boost::numeric::ublas::matrix<double>(blobs.size(), blobs_avant_.size());
	// Remplissage
	for(size_t i=0; i<matrice_distances_.size1(); ++i) {
		for(size_t j=0; j<matrice_distances_.size2(); ++j) {
			matrice_distances_(i,j) = blobs[i].distance(blobs_avant_[j]);
		}
	}
}