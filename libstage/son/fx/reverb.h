#if !defined(REVERB_H_S6KFEDIH)
#define REVERB_H_S6KFEDIH

#include "son/fx.h"

struct reverb : public fx {
	reverb(processus_sonore* from, boite_pd* to = &(master_pd::instance()));
	virtual void creer();
	virtual std::string classe() { return "reverb"; }
private:
	
};

#endif /* end of include guard: REVERB_H_S6KFEDIH */
