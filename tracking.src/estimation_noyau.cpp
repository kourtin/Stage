#include "estimation_noyau.h"
#include <vector>
#include <iostream>

estimation_noyau::estimation_noyau(std::vector<float>& v, double bandwidth) : valeurs_(v), bandwidth_(bandwidth) {}

float estimation_noyau::operator()(float x) {
	float facteur = 1.0 / (bandwidth_ * valeurs_.size());
	float acc = 0.f;
	for(std::vector<float>::iterator it = valeurs_.begin(); it != valeurs_.end(); ++it)
		acc += gaussien(1.0 * (x - (*it)) / bandwidth_);
	return facteur * acc;
}