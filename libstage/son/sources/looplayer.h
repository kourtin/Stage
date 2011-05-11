#if !defined(LOOPLAYER_H_35W0O7HR)
#define LOOPLAYER_H_35W0O7HR

#include "son/source.h"

struct looplayer : public source {
	looplayer();
	virtual ~looplayer();
	virtual void creer();
	void start();
	void stop();
	virtual std::string classe() { return "looplayer"; }
};

#endif /* end of include guard: LOOPLAYER_H_35W0O7HR */
