#if !defined(PARAMETRES_H_SH6ZYJ7J)
#define PARAMETRES_H_SH6ZYJ7J

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <algorithm>
#include <string>
#include <list>
#include <sstream>

class parametre;
struct liste_parametres {
	typedef std::list<parametre*> liste_params;
	typedef liste_params::iterator iterator;
	liste_parametres();
	virtual ~liste_parametres();
	iterator begin() { return param_.begin(); }
	iterator end() { return param_.end(); }
	unsigned int size() { return param_.size(); }
	void add(parametre*);
	void remove(parametre*);
	void set(std::string, float);
	float get(std::string);
	parametre* get_param(std::string nom);
private:
	liste_params param_;
};

struct parametre {
	typedef boost::function<void()> fct_callback;
	parametre(std::string nom) : x_(0.0), nom_(nom) {}
	virtual ~parametre() {}
	void callback(fct_callback f) { callback_ = f; }
	fct_callback callback() { return callback_; }
	virtual void set(float f) { internal_set(f); callback_(); }
	virtual float get() { return x_; }
	virtual float min() { return 0.0; }
	virtual float max() { return 1.0; }
	virtual std::string nom() { return nom_; }
	virtual std::string get_str() { std::ostringstream oss; oss << (get()*100) << "%"; return oss.str(); }
protected:
	virtual void internal_set(float f) { x_ = std::max(std::min(f, max()), min()); }
	float x_;
private:
	fct_callback callback_;
	std::string nom_;
};

struct parametre_volume : public parametre {
	parametre_volume() : parametre("volume") {}
	virtual ~parametre_volume() {}
};

struct parametre_note : public parametre {
	parametre_note() : parametre("note") {}
	virtual ~parametre_note() {}
	virtual float min() { return 23; }
	virtual float max() { return 75; }
	virtual std::string get_str() { return notes_italien[static_cast<unsigned int>(get()) % 12]; }
protected:
	virtual void internal_set(float f) { parametre::internal_set(f); x_ = static_cast<unsigned int>(x_); }
private:
	static const std::string notes_italien[12];
	static const std::string notes_anglosaxon[12];
};

struct parametre_frequence : public parametre {
	parametre_frequence() : parametre("frequence") {}
	virtual ~parametre_frequence() {}
	virtual float min() { return 10; }
	virtual float max() { return 20000; }
	virtual std::string get_str() { std::ostringstream oss; oss << get() << "Hz"; return oss.str(); }
};

struct parametre_bang : public parametre {
	parametre_bang() : parametre("bang") {}
	virtual ~parametre_bang() {}
	virtual float min() { return 0; }
	virtual float max() { return 0; }
	virtual float get() { x_= 0; return 0; }
	virtual std::string get_str() { return "<bang>"; }
protected:
	virtual void internal_set(float f) { x_ = 1; }
};

#endif /* end of include guard: PARAMETRES_H_SH6ZYJ7J */
