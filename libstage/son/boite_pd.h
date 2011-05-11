#if !defined(BOITE_PD_H_3S0I55PX)
#define BOITE_PD_H_3S0I55PX

#include <list>
#include <string>
#include <sstream>

/*
 * RAPPEL: Toujours connecter avec connecter_outlet0, deconnecter_outlet0 !
 */

struct boite_pd {
	typedef std::list<boite_pd*> liste_connections_t;
	typedef liste_connections_t::iterator iterator;
	
	boite_pd(std::string string_id);
	virtual ~boite_pd();
	
	virtual void connecter_outlet0(boite_pd* dest);
	virtual void deconnecter_outlet0(boite_pd* dest);
	
	virtual liste_connections_t& inlets() { return inlets_; }
	virtual liste_connections_t& outlets() { return outlets_; }
	
	// Une boite pd a toujours un identifiant, dans ce monde de brutes assoiffées de sang
	virtual std::string id() { return string_id_; }
	virtual std::string classe() = 0;

	virtual void creer() = 0;

	// Pour générer un identifiant unique
	static unsigned int next_unique_id() { static unsigned int n; return n++; }
	// Pour générer une chaîne identifiant lisible
	template <class T>
	std::string compute_string_id(std::string prefix, T suffix) {
		std::ostringstream oss;
		oss << prefix << suffix;
		return oss.str();
	}
	
	// ATTENTION: Ce délai doit être inférieur à la fréquence d'établissement et de coupure des connections (sinon on a des objets perdus)
	static float& delai_coupure() { static float d = 100; return d; }
protected:
	virtual void connecter_inlet0(boite_pd* src);
	virtual void deconnecter_inlet0(boite_pd* src);
private:
	liste_connections_t inlets_, outlets_;
	std::string string_id_;
};

#endif /* end of include guard: BOITE_PD_H_3S0I55PX */
