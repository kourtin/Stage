#if !defined(COUPLAGE_VIRTUEL_H_SUNSRRBK)
#define COUPLAGE_VIRTUEL_H_SUNSRRBK

#include "objet.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/xtime.hpp>

class calculateur_couplage;

struct couplage_virtuel {
	couplage_virtuel(objet& o);
	virtual ~couplage_virtuel();
	void operator()();
	float x() { return px_; }
	float y() { return py_; }
	float z() { return pz_; }
	float r() { return pr_; }
private:
	void maj_position();
	void maj_forces();
private:
	objet& objet_;
	float k_, z_, zmilieu_, invm_;
	float px_, py_, pz_, pr_;
	float ax_, ay_, az_, ar_;
	float fx_, fy_, fz_, fr_;
	float ox_, oy_, oz_, or_;
	float dist_;
};

struct calculateur_couplage {
	typedef std::list<couplage_virtuel*> liste_couplages;
	static void init() {
		instance();
	}
	static void join() {
		instance().thread_.join();
	}
	static void ajouter(couplage_virtuel& c) {
		instance().couplages_.push_back(&c);
	}
	static void retirer(couplage_virtuel& c) {
		instance().couplages_.remove(&c);
	}
	virtual ~calculateur_couplage() {
		for(liste_couplages::iterator it = couplages_.begin(); it != couplages_.end(); ++it)
			delete *it;
	}
private:
	struct thread_runner {
		void operator()() {
			calculateur_couplage::instance().run_thread();
		}
	};
	friend class thread_runner;
	void run_thread();
private:
	calculateur_couplage() : thread_(r_) {}
	calculateur_couplage(calculateur_couplage&);
	static calculateur_couplage& instance() {
		static calculateur_couplage inst;
		return inst;
	}
private:
	liste_couplages couplages_;
	// boost::mutex mutex_;
	thread_runner r_;
	boost::thread thread_;
};

#endif /* end of include guard: COUPLAGE_VIRTUEL_H_SUNSRRBK */
