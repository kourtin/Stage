#if !defined(FX_H_NXIAR6E4)
#define FX_H_NXIAR6E4

#include "son/processus_sonore.h"
#include "son/master_pd.h"

struct fx : public processus_sonore {
	fx(processus_sonore* from, boite_pd* to = &(master_pd::instance()));
	virtual ~fx();
	virtual void creer();
private:
	processus_sonore* from_;
	boite_pd* to_;
};

#endif /* end of include guard: FX_H_NXIAR6E4 */
