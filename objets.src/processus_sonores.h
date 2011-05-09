#if !defined(PROCESSUS_SONORES_H_5QL0EYYL)
#define PROCESSUS_SONORES_H_5QL0EYYL

#include "liveosc.h"
#include <string>
#include <sstream>
#include <algorithm>

/*
 * RAPPEL: Toujours connecter avec connecter_outlet0, deconnecter_outlet0 !
 */

struct boite_pd {
	typedef std::list<boite_pd*> liste_connections_t;
	typedef liste_connections_t::iterator iterator;
	
	// ATTENTION: Ce délai doit être inférieur à la fréquence d'établissement et de coupure des connections (sinon on a des objets perdus)
	static float& delai_coupure() { static float d = 100; return d; }
	
	boite_pd(std::string string_id) : string_id_(string_id) {}
	
	virtual ~boite_pd() {
		// On déconnecte toutes les boîtes reliées à l'inlet
		iterator it = inlets().begin();
		while(it != inlets().end()) {
			boite_pd* b = *it;
			b->deconnecter_outlet0(this);
			it = inlets().begin();
		}
		// On déconnecte toutes les boîtes reliées à l'outlet
		for(iterator it = outlets().begin(); it != outlets().end(); ++it) {
			boite_pd* b = *it;
			if(b != 0) {
				b->deconnecter_inlet0(this);
			}
		}
		
		if(id().compare("") == 0)
			return;
		
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyndelay") << delai_coupure() << "del" << id().c_str() << osc::EndMessage);
	}
	
	virtual void connecter_outlet0(boite_pd* dest) {
		if(!dest) return;
		// Cherche s'il est déjà connecté
		iterator it = std::find(outlets().begin(), outlets().end(), dest);
		if(it == outlets().end()) {
			// Connecte cet outlet 0 à l'inlet 0 de dest
			dest->connecter_inlet0(this);
			// Rajoute à la liste des outlets
			outlets().push_back(dest);
		}
	}
	
	virtual void deconnecter_outlet0(boite_pd* dest) {
		if(!dest) return;
		// Cherche s'il est déjà connecté
		iterator it = std::find(outlets().begin(), outlets().end(), dest);
		if(it != outlets().end()) {
			// Déconnecte cet outlet 0 de l'inlet 0 de dest
			dest->deconnecter_inlet0(this);
			// Enlève de la liste des outlets
			outlets().remove(dest);
		}
	}
	
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
protected:
	virtual void connecter_inlet0(boite_pd* src) {
		if(!src) return;
		// Cherche s'il est pas déjà connecté
		iterator it = std::find(inlets().begin(), inlets().end(), src);
		if(it == inlets().end()) {
			// Connecte l'outlet 0 de src à cet inlet 0 dans dyn~
			liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "conn" << (src->id().c_str()) << 0 << id().c_str() << 0 << osc::EndMessage);
			// Rajoute à la liste des inlets
			inlets().push_back(src);
		}
	}
	
	virtual void deconnecter_inlet0(boite_pd* src) {
		if(!src) return;
		// Cherche s'il est déjà connecté
		iterator it = std::find(inlets().begin(), inlets().end(), src);
		if(it != inlets().end()) {
			// Déconnecte l'outlet 0 de src de cet inlet 0 dans dyn~
			liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyndelay") << delai_coupure() << "dis" << src->id().c_str() << 0 << id().c_str() << 0 << osc::EndMessage);
			// Enlève de la liste des inlets
			inlets().remove(src);
		}
	}
private:
	liste_connections_t inlets_, outlets_;
	std::string string_id_;
};

struct master_pd : public boite_pd {
	virtual ~master_pd() {
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "reset" << osc::EndMessage);
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/semicolon") << "pd dsp 0" << osc::EndMessage);
	}
	
	virtual void connecter_outlet0(boite_pd*)   {}
	virtual void deconnecter_outlet0(boite_pd*) {}
	virtual std::string id() { throw "ERREUR FATALE: appel de std::string master_pd::id()"; return ""; }
	virtual std::string classe() { return "master"; }
	
	// Le master est déjà présent, il s'appelle dyn_out
	virtual void creer() {}
	
	// singleton
	static master_pd& instance() {
		static master_pd* singleton = new master_pd;
		return *singleton;
	}
protected:
	virtual void connecter_inlet0(boite_pd* src) {
		if(!src) return;
		// Cherche s'il est pas déjà connecté
		iterator it = std::find(inlets().begin(), inlets().end(), src);
		if(it == inlets().end()) {
			// Crée un atténuateur line avec un grain rate de 2ms
			liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << boite_pd::compute_string_id(src->id(), "_line").c_str() << "line" << 0 << 2 << osc::EndMessage);
			// Crée un amplificateur
			liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << boite_pd::compute_string_id(src->id(), "_gain").c_str() << "*~" << 0 << osc::EndMessage);
			// Connecte l'amplificateur à src
			liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "conn" << (src->id().c_str()) << 0 << boite_pd::compute_string_id(src->id(), "_gain").c_str() << 0 << osc::EndMessage);
			// Connecte l'atténuateur à l'amplificateur
			liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "conn" << boite_pd::compute_string_id(src->id(), "_line").c_str() << 0 << boite_pd::compute_string_id(src->id(), "_gain").c_str() << 1 << osc::EndMessage);
			// Connecte l'outlet 0 de l'ampli à l'inlet 0 du master dans dyn~
			liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "conn" << /*src->id().c_str()*/boite_pd::compute_string_id(src->id(), "_gain").c_str() << 0 << 0 << osc::EndMessage);
			// Envoie le volume progressivement
			liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << /*src->id().c_str()*/boite_pd::compute_string_id(src->id(), "_line").c_str() << 1 << delai_coupure() << osc::EndMessage);
			// Rajoute à la liste des inlets
			inlets().push_back(src);
		}
	}
	virtual void deconnecter_inlet0(boite_pd* src) {
		if(!src) return;
		// Cherche s'il est déjà connecté
		iterator it = std::find(inlets().begin(), inlets().end(), src);
		if(it != inlets().end()) {
			// Baisse le volume progressivement
			liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << boite_pd::compute_string_id(src->id(), "_line").c_str() << 0 << delai_coupure() << osc::EndMessage);
			// Déconnectera l'outlet 0 de l'amplificateur de l'inlet 0 du master dans dyn~
			liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyndelay") << delai_coupure() << "dis" << boite_pd::compute_string_id(src->id(), "_gain").c_str() << 0 << 0 << osc::EndMessage);
			// Détruira l'atténuateur
			liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyndelay") << delai_coupure() << "del" << boite_pd::compute_string_id(src->id(), "_line").c_str() << osc::EndMessage);
			// Détruira l'amplificateur
			liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyndelay") << delai_coupure() << "del" << boite_pd::compute_string_id(src->id(), "_gain").c_str() << osc::EndMessage);
			// Enlève de la liste des inlets
			inlets().remove(src);
		}
	}
private:
	master_pd() : boite_pd("") {
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "reset" << osc::EndMessage);
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/semicolon") << "pd" << "dsp" << 1 << osc::EndMessage);
	}
};

struct processus_sonore : public boite_pd {
	processus_sonore(std::string string_id, unsigned int control_inlet = 0) : boite_pd(string_id), control_inlet_(control_inlet) {}
	
	virtual ~processus_sonore() {
		// Déconnecte le proxy
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "dis" << proxy_id().c_str() << 0 << id().c_str() << (char)control_inlet_ << osc::EndMessage);
		
		// Détruit le proxy
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "del" << proxy_id().c_str() << osc::EndMessage);
		
		// Connecte tous ses inlets à tous ses outlets, pour remplacer son absence.
		for(iterator it = inlets().begin(); it != inlets().end(); ++it) {
			boite_pd* inlet_actuel = *it;
			if(!inlet_actuel) continue; // Parafoudre
			for(iterator it2 = outlets().begin(); it2 != outlets().end(); ++it2) {
				boite_pd* outlet_actuel = *it2;
				inlet_actuel->connecter_outlet0(outlet_actuel);
			}
		}
	}
	
	virtual void creer() {
		// Crée le proxy
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << proxy_id().c_str() << "list" << osc::EndMessage);
		// Connectons l'outlet du proxy à notre inlet
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "conn" << proxy_id().c_str() << 0 << id().c_str() << (char)control_inlet_ << osc::EndMessage);
	}
	
	std::string proxy_id() { return compute_string_id(id(), "_proxy"); }
private:
	unsigned int control_inlet_;
};

struct source : public processus_sonore {
	source() : processus_sonore(compute_string_id("source", next_unique_id()), 0) {}
	virtual ~source() {}
	virtual void creer() {
		processus_sonore::creer();
		
		// La source commence connectée au master
		connecter_outlet0(&(master_pd::instance()));
	}
protected:
	virtual void connecter_inlet0(boite_pd* src) {
		throw "Connection a l'inlet d'une source";
	}
	virtual void deconnecter_inlet0(boite_pd* src) {
		throw "Deconnection de l'inlet d'une source";
	}
};

struct oscillateur : public source {
	oscillateur(float f = 440.f) : freq_(f), vol_(1.0) {
		// On crée la boîte dans dyn~
		creer();
	}
	virtual void creer() {
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << id().c_str() << "oscillateur" << osc::EndMessage);
		source::creer();
	}
	float frequence() { return freq_; }
	void frequence(float f) {
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "freq" << f << osc::EndMessage);
		freq_ = f;
	}
	float volume() { return vol_; }
	void volume(float v) {
		v = std::max(0.f, v);
		v = std::min(v, 1.f);
		vol_ = v;
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "vol" << v << osc::EndMessage);
	}
	virtual std::string classe() { return "oscillateur"; }
private:
	float freq_;
	float vol_;
};

struct looplayer : public source {
	looplayer() {
		// On crée la boîte dans dyn~
		creer();
	}
	
	virtual void creer() {
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << id().c_str() << "looplayer" << osc::EndMessage);
		source::creer();
	}
	void start() {
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "play" << 1 << osc::EndMessage);
	}
	void stop() {
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "play" << 0 << osc::EndMessage);
	}
	virtual std::string classe() { return "looplayer"; }
};

struct line_in : public source {
	line_in(unsigned int n = 1) : channel_(n) {
		creer();
	}
	virtual void creer() {
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << id().c_str() << "adc~" << (char)channel_ << osc::EndMessage);
		source::creer();
	}
	virtual std::string classe() { return "line_in"; }
private:
	unsigned int channel_;
};

struct fx : public processus_sonore {
	fx(processus_sonore* from, boite_pd* to = &(master_pd::instance())) : processus_sonore(compute_string_id("fx", next_unique_id()), 1), from_(from), to_(to) {}
	virtual ~fx() {}
	virtual void creer() {
		processus_sonore::creer();
		// Le from est déchu de sa connection avec le to
		from_->deconnecter_outlet0(to_);
		// L'effet commence connecté au from et au to
		from_->connecter_outlet0(this);
		connecter_outlet0(to_);
	}
private:
	processus_sonore* from_;
	boite_pd* to_;
};

struct reverb : public fx {
	reverb(processus_sonore* from, boite_pd* to = &(master_pd::instance())) : fx(from, to) {
		// On crée la boîte dans dyn~
		creer();
	}
	virtual void creer() {
		// Crée la freeverb
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << id().c_str() << "freeverb~" << osc::EndMessage);
		// Paramètres par défaut
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << id().c_str() << "wet" << 1 << osc::EndMessage);
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << id().c_str() << "dry" << 0 << osc::EndMessage);
		fx::creer();
	}
	virtual std::string classe() { return "reverb"; }
private:
	
};

struct simple_delay : public fx {
	simple_delay(processus_sonore* from, boite_pd* to = &(master_pd::instance())) : fx(from, to), delai_(100.0), wet_(0.33) {
		// On crée la boîte dans dyn~
		creer();
		// On la paramètre
		delai(delai_);
		wet(wet_);
	}
	virtual void creer() {
		// Crée la freeverb
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "newobj" << "." << id().c_str() << "simpledelay" << osc::EndMessage);
		fx::creer();
	}
	virtual std::string classe() { return "simple_delay"; }
	float delai() { return delai_; }
	void delai(float d) {
		delai_ = d;
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "delay" << delai_ << osc::EndMessage);
	}
	float wet() { return wet_; }
	void wet(float w) {
		wet_ = w;
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "wet" << wet_ << osc::EndMessage);
	}
	float feedback() { return feedback_; }
	void feedback(float f) {
		feedback_ = f;
		liveosc_send::instance().send(liveosc_send::instance().new_packet() << osc::BeginMessage("/dyn") << "send" << proxy_id().c_str() << "feedback" << feedback_ << osc::EndMessage);
	}
private:
	float delai_;
	float wet_;
	float feedback_;
};

#endif /* end of include guard: PROCESSUS_SONORES_H_5QL0EYYL */
