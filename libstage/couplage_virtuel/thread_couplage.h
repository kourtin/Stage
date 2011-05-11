#if !defined(THREAD_COUPLAGE_H_XGZW5H53)
#define THREAD_COUPLAGE_H_XGZW5H53

#include "couplage_virtuel/couplage_virtuel.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/xtime.hpp>

struct thread_couplage {
	typedef std::list<couplage_virtuel*> liste_couplages;
	static void init() {
		boost::thread b(boost::bind(&thread_couplage::run_thread, instance()));
	}
	static void ajouter(couplage_virtuel& c) {
		instance()->couplages_.push_back(&c);
	}
	static void retirer(couplage_virtuel& c) {
		instance()->couplages_.remove(&c);
	}
	virtual ~thread_couplage() {
		for(liste_couplages::iterator it = couplages_.begin(); it != couplages_.end(); ++it)
			delete *it;
	}
private:
	void run_thread();
private:
	thread_couplage() {}
	thread_couplage(calculateur_couplage&);
	static thread_couplage* instance() {
		static thread_couplage* inst = new thread_couplage;
		return inst;
	}
private:
	liste_couplages couplages_;
};

#endif /* end of include guard: THREAD_COUPLAGE_H_XGZW5H53 */
