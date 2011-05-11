#if !defined(MASTER_PD_H_I6N6YX35)
#define MASTER_PD_H_I6N6YX35

#include "son/boite_pd.h"

struct master_pd : public boite_pd {
	virtual ~master_pd();
	
	virtual void connecter_outlet0(boite_pd*)   {}
	virtual void deconnecter_outlet0(boite_pd*) {}
	virtual std::string id() { throw "ERREUR FATALE: appel de std::string master_pd::id()"; return ""; }
	virtual std::string classe() { return "master"; }
	
	// Le master est déjà présent, il s'appelle dyn_out
	virtual void creer() {}
	
	// singleton
	static master_pd& instance();
protected:
	virtual void connecter_inlet0(boite_pd* src);
	virtual void deconnecter_inlet0(boite_pd* src);
private:
	master_pd();
};

#endif /* end of include guard: MASTER_PD_H_I6N6YX35 */
