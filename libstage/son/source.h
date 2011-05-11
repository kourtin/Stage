#if !defined(SOURCE_H_ECCM7CY7)
#define SOURCE_H_ECCM7CY7

#include "son/processus_sonore.h"

struct source : public processus_sonore {
	source();
	virtual ~source();
	virtual void creer();
protected:
	virtual void connecter_inlet0(boite_pd* src) {
		throw "Connection a l'inlet d'une source";
	}
	virtual void deconnecter_inlet0(boite_pd* src) {
		throw "Deconnection de l'inlet d'une source";
	}
};

#endif /* end of include guard: SOURCE_H_ECCM7CY7 */
