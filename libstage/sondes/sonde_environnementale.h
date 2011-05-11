#if !defined(SONDE_ENVIRONNEMENTALE_H_4MHKM0JQ)
#define SONDE_ENVIRONNEMENTALE_H_4MHKM0JQ

class objet_store;

struct sonde_environnementale {
	virtual ~sonde_environnementale() {}
	virtual void operator()(objet_store* store = 0) = 0;
};

struct sonde_primaire : public sonde_environnementale {
	virtual ~sonde_primaire() {}
	virtual void operator()(objet_store* store = 0) = 0;
};

struct sonde_secondaire : public sonde_environnementale {
	virtual ~sonde_secondaire() {}
	virtual void operator()(objet_store* store = 0) = 0;
};

#endif /* end of include guard: SONDE_ENVIRONNEMENTALE_H_4MHKM0JQ */
