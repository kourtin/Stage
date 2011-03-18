#if !defined(ESTIMATION_NOYAU_H_APW9RITI)
#define ESTIMATION_NOYAU_H_APW9RITI

#include <cmath>
#include <vector>

static const double PI = 3.14159265359;

struct estimation_noyau {
	estimation_noyau(std::vector<float>& v, double bandwidth = 2);
	virtual ~estimation_noyau() {}
	float operator()(float x);
	float gaussien(float u) {
		return 1.0 / sqrt(2 * PI) * exp(-1.0 / 2 * u * u);
	}
private:
	std::vector<float> valeurs_;
	double bandwidth_;
};

#endif /* end of include guard: ESTIMATION_NOYAU_H_APW9RITI */
