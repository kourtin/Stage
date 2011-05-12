#include "son/parametres.h"
#include <limits>

const std::string parametre_note::notes_italien[12] = {"Do", "Do#", "Re", "Re#", "Mi", "Fa", "Fa#", "Sol", "Sol#", "La", "La#", "Si"};
const std::string parametre_note::notes_anglosaxon[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

struct pred_nom {
	pred_nom(std::string nom) : nom_(nom) {}
	bool operator()(parametre* o) {
		return o && nom_.compare(o->nom()) == 0;
	}
private:
	std::string nom_;
};

liste_parametres::liste_parametres() {}

liste_parametres::~liste_parametres() {}

void liste_parametres::add(parametre* p) {
	param_.push_back(p);
}

void liste_parametres::remove(parametre* p) {
	param_.remove(p);
}

void liste_parametres::set(std::string nom, float v) {
	iterator it = std::find_if(param_.begin(), param_.end(), pred_nom(nom));
	if(it != param_.end() && *it) {
		(*it)->set(v);
	}
}

float liste_parametres::get(std::string nom) {
	iterator it = std::find_if(param_.begin(), param_.end(), pred_nom(nom));
	if(it != param_.end() && *it) {
		return (*it)->get();
	} else {
		return std::numeric_limits<float>::quiet_NaN();
	}
}