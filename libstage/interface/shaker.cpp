#include "interface/shaker.h"
#include "temps.h"
#include <cmath>
#include <iostream>

shaker::shaker() : changements_requis(8), temps_limite(2000), seuil_distance(0.005), ancien_signe_(0), nb_chgts_signes_(0), tempo_(temps_actuel()) {}

void shaker::reset() {
	tempo_ = temps_actuel();
	nb_chgts_signes_ = 0;
	ancien_signe_ = 0;
}

bool shaker::operator()(float x, float y, float z) {
	// Maj vitesses
	vitesse_[0] = x - ancienne_pos_[0];
	vitesse_[1] = y - ancienne_pos_[1];
	vitesse_[2] = z - ancienne_pos_[2];
	
	// Calcule la distance
	float distance = sqrt(vitesse_[0]*vitesse_[0] + vitesse_[1]*vitesse_[1]/* + vitesse_[2]*vitesse_[2]*/);
	
	// std::cout << vitesse_[0] << " \t\t " << vitesse_[1] << " \t\t " << vitesse_[2] << std::endl;
	// std::cout << distance << std::endl;
	
	// Regarde si c'est gagné
	bool ok = (nb_chgts_signes_ > changements_requis);
	
	// Réinitialise
	if(ok || temps_actuel() - tempo_ > temps_limite)
		reset();
	
	// Regarde si la vitesse est plus grande qu'un seuil
	if(distance > seuil_distance/* * (1-z)*/) {
		float produit_scalaire = vitesse_[0] * ancienne_vitesse_[0] + vitesse_[1] * ancienne_vitesse_[1]/* + vitesse_[2] * ancienne_vitesse_[2]*/;
		
		float signe = (produit_scalaire >= 0 ? 1 : -1);
		
		if(ancien_signe_ == 0)
			ancien_signe_ = signe;
		
		if(signe != ancien_signe_)
			++nb_chgts_signes_;
		
		ancien_signe_ = signe;
	}
	
	// Met à jour les positions et vitesses "anciennes"
	ancienne_pos_[0] = x;
	ancienne_pos_[1] = y;
	ancienne_pos_[2] = z;
	ancienne_vitesse_[0] = vitesse_[0];
	ancienne_vitesse_[1] = vitesse_[1];
	ancienne_vitesse_[2] = vitesse_[2];
	
	return ok;
}