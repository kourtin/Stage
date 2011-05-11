#include "couplage_virtuel/couplage_virtuel.h"
#include "couplage_virtuel/thread_couplage.h"

couplage_virtuel::couplage_virtuel(objet& o) : objet_(o), k_(0.002), z_(0.05), zmilieu_(0.4), invm_(1), px_(o.x_), py_(o.y_), pz_(o.z_), pr_(o.r_), ax_(px_), ay_(py_), az_(pz_), ar_(pr_), fx_(0), fy_(0), fz_(0), fr_(0), ox_(o.x_c()), oy_(o.y_c()), oz_(o.z_c()), or_(o.r_c()) {
	thread_couplage::ajouter(*this);
	
	// Calcule les distances sur les axes
	float dist_x = px_ - ox_;
	float dist_y = py_ - oy_;
	float dist_z = pz_ - oz_;
	
	// Calcule la distance euclidienne
	dist_ = sqrt(dist_x * dist_x + dist_y * dist_y + dist_z * dist_z);
}
couplage_virtuel::~couplage_virtuel() {
	thread_couplage::retirer(*this);
}

void couplage_virtuel::operator()() {
	ox_ = objet_.x_c();
	oy_ = objet_.y_c();
	oz_ = objet_.z_c();
	or_ = objet_.r_c();
	maj_position();
	maj_forces();
}

void couplage_virtuel::maj_position() {
	// Calcule les vitesses
	float dx = px_ - ax_;
	float dy = py_ - ay_;
	float dz = pz_ - az_;
	float dr = pr_ - ar_;

	// Sauvegarde les anciennes positions
	ax_ = px_;
	ay_ = py_;
	az_ = pz_;
	ar_ = pr_;

	// Met à jour les positions
	px_ += dx - (dx * zmilieu_ - fx_) * invm_;
	py_ += dy - (dy * zmilieu_ - fy_) * invm_;
	pz_ += dz - (dz * zmilieu_ - fz_) * invm_;
	pr_ = or_;
	// pr_ += dr - (dr * zmilieu_ - fr_) * invm_;
	// 	while(pr_ > 2*M_PI)
	// 		pr_ -= 2*M_PI;
	// 	while(pr_ < -2*M_PI)
	// 		pr_ += 2*M_PI;
	// std::cout << pr_ * 180 / M_PI << std::endl;

	// Ayé maman j'ai tout mangé les forces.
	fx_ = 0;
	fy_ = 0;
	fz_ = 0;
	// fr_ = 0;
	// // Interpolation linéaire bidon
	// double q = 0.01;
	// px_ = q*objet_.x_ + (1-q)*px_;
	// py_ = q*objet_.y_ + (1-q)*py_;
	// pz_ = q*objet_.z_ + (1-q)*pz_;
}

void couplage_virtuel::maj_forces() {
	static float distra = 0;
	// Calcule les distances sur les axes
	float dist_x = px_ - ox_;
	float dist_y = py_ - oy_;
	float dist_z = pz_ - oz_;
	// float dist_r = pr_ - or_;
	// 	while(dist_r > 2*M_PI)
	// 		dist_r -= 2*M_PI;
	// 	while(dist_r < -2*M_PI)
	// 		dist_r += 2*M_PI;
	
	// Calcule la distance euclidienne
	float dist = sqrt(dist_x * dist_x + dist_y * dist_y + dist_z * dist_z);

	// Calcule la force grâce à la 2è loi de Newton
	float f = k_ * dist + z_ * (dist - dist_);
	
	dist_ = dist;
	
	// // Calcule la force en 1D pour la rotation
	// 	float distr = sqrt(dist_r*dist_r);
	// 	fr_ += k_ * distr + z_ * (distr - distra);
	// 	distra = distr;
	
	// Applique la force sur chaque axe
	if(dist_ != 0)
		fx_ -= f * dist_x / dist_;
	if(dist_ != 0)
		fy_ -= f * dist_y / dist_;
	if(dist_ != 0)
		fz_ -= f * dist_z / dist_;
}