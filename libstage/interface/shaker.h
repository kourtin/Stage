#ifndef SHAKER_H_F3O3MNDY
#define SHAKER_H_F3O3MNDY

struct shaker {
	shaker();
	~shaker() {}
	void reset();
	bool operator()(float x, float y, float z);
	
	int changements_requis;
	double temps_limite;
	float seuil_distance;
private:
	float ancienne_pos_[3];
	float vitesse_[3];
	float ancienne_vitesse_[3];
	float ancien_signe_;
	int nb_chgts_signes_;
	double tempo_;
};

#endif /* end of include guard: SHAKER_H_F3O3MNDY */
