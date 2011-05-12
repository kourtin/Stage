#if !defined(PROCESSUS_SONORE_H_EX9JXNYL)
#define PROCESSUS_SONORE_H_EX9JXNYL

#include "son/boite_pd.h"
#include "son/parametres.h"

struct processus_sonore : public boite_pd {
	processus_sonore(std::string string_id, unsigned int control_inlet = 0);
	
	virtual ~processus_sonore();
	
	virtual void creer();
	
	std::string proxy_id() { return compute_string_id(id(), "_proxy"); }
	
	liste_parametres& parametres() { return liste_parametres_; }
protected:	
	liste_parametres liste_parametres_;
private:
	unsigned int control_inlet_;
};

#endif /* end of include guard: PROCESSUS_SONORE_H_EX9JXNYL */
