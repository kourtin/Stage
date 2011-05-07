#include "couplage_virtuel.h"

couplage_virtuel::couplage_virtuel(objet& o) : objet_(o), k_(0.002), z_(0.05), zmilieu_(0.4), invm_(1), px_(o.x_), py_(o.y_), pz_(o.z_), ax_(px_), ay_(py_), az_(pz_), fx_(0), fy_(0), fz_(0), ox_(o.x_c()), oy_(o.y_c()), oz_(o.z_c()) {
	calculateur_couplage::ajouter(*this);
	
	// Calcule les distances sur les axes
	float dist_x = px_ - ox_;
	float dist_y = py_ - oy_;
	float dist_z = pz_ - oz_;
	
	// Calcule la distance euclidienne
	dist_ = sqrt(dist_x * dist_x + dist_y * dist_y + dist_z * dist_z);
}
couplage_virtuel::~couplage_virtuel() {
	calculateur_couplage::retirer(*this);
}

void couplage_virtuel::operator()() {
	ox_ = objet_.x_c();
	oy_ = objet_.y_c();
	oz_ = objet_.z_c();
	maj_position();
	maj_forces();
}

void couplage_virtuel::maj_position() {
	// Calcule les vitesses
	float dx = px_ - ax_;
	float dy = py_ - ay_;
	float dz = pz_ - az_;

	// Sauvegarde les anciennes positions
	ax_ = px_;
	ay_ = py_;
	az_ = pz_;

	// Met à jour les positions
	px_ += dx - (dx * zmilieu_ - fx_) * invm_;
	py_ += dy - (dy * zmilieu_ - fy_) * invm_;
	pz_ += dz - (dz * zmilieu_ - fz_) * invm_;

	// Ayé maman j'ai tout mangé les forces.
	fx_ = 0;
	fy_ = 0;
	fz_ = 0;
	// // Interpolation linéaire bidon
	// double q = 0.01;
	// px_ = q*objet_.x_ + (1-q)*px_;
	// py_ = q*objet_.y_ + (1-q)*py_;
	// pz_ = q*objet_.z_ + (1-q)*pz_;
}

void couplage_virtuel::maj_forces() {
	// Calcule les distances sur les axes
	float dist_x = px_ - ox_;
	float dist_y = py_ - oy_;
	float dist_z = pz_ - oz_;
	
	// Calcule la distance euclidienne
	float dist = sqrt(dist_x * dist_x + dist_y * dist_y + dist_z * dist_z);

	// Calcule la force grâce à la 2è loi de Newton
	float f = k_ * dist_ + z_ * (dist - dist_);
	
	dist_ = dist;
	
	// Applique la force sur chaque axe
	if(dist_ != 0)
		fx_ -= f * dist_x / dist_;
	if(dist_ != 0)
		fy_ -= f * dist_y / dist_;
	if(dist_ != 0)
		fz_ -= f * dist_z / dist_;
}

void calculateur_couplage::run_thread() {
	for(;;) {
		for(liste_couplages::iterator it = couplages_.begin(); it != couplages_.end(); ++it)
			(*it)->operator()();
		// boost::this_thread::sleep(boost::posix_time::millisec(1000));
		std::clock_t t = std::clock();
		// 1kHz
		while(std::clock() - t < 0.001*CLOCKS_PER_SEC);
	}
}