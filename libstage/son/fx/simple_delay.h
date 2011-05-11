#if !defined(SIMPLE_DELAY_H_3I60S6Q0)
#define SIMPLE_DELAY_H_3I60S6Q0

#include "son/fx.h"

struct simple_delay : public fx {
	simple_delay(processus_sonore* from, boite_pd* to = &(master_pd::instance()));
	virtual void creer();
	virtual std::string classe() { return "simple_delay"; }
	float delai() { return delai_; }
	void delai(float d);
	float wet() { return wet_; }
	void wet(float w);
	float feedback() { return feedback_; }
	void feedback(float f);
private:
	float delai_;
	float wet_;
	float feedback_;
};

#endif /* end of include guard: SIMPLE_DELAY_H_3I60S6Q0 */
