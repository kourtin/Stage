#if !defined(OSCILLATEUR_H_6O1348OZ)
#define OSCILLATEUR_H_6O1348OZ

#include "son/source.h"

struct oscillateur : public source {
	oscillateur(float f = 440.f);
	virtual void creer();
	float frequence() { return freq_; }
	void frequence(float f);
	float volume() { return vol_; }
	void volume(float v);
	virtual std::string classe() { return "oscillateur"; }
private:
	float freq_;
	float vol_;
};

#endif /* end of include guard: OSCILLATEUR_H_6O1348OZ */
