#if !defined(OSCILLATEUR_H_6O1348OZ)
#define OSCILLATEUR_H_6O1348OZ

#include "son/source.h"

struct oscillateur : public source {
	oscillateur(float f = 440.f);
	virtual void creer();
	parametre_frequence& frequence() { return freq_; }
	virtual std::string classe() { return "oscillateur"; }
	void frequence_changed();
private:
	parametre_frequence freq_;
	float ancienne_freq_;
};

#endif /* end of include guard: OSCILLATEUR_H_6O1348OZ */
