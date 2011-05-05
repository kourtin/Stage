#if !defined(BLOB_MATCHER_H_HC68VFZ)
#define BLOB_MATCHER_H_HC68VFZ

#include "blobby.h"
#include <boost/numeric/ublas/matrix.hpp>

struct blob_matcher {
	blob_matcher() {}
	virtual ~blob_matcher() {}
	void operator()(std::vector<blobby>&);
private:
	std::vector<blobby> blobs_avant_;
	boost::numeric::ublas::matrix<double> matrice_distances_;
	void association_naive(std::vector<blobby>&);
	void association_naive2(std::vector<blobby>&);
	void association_park_aggarwal(std::vector<blobby>&);
	void construire_matrice(std::vector<blobby>&);
};

#endif /* end of include guard: BLOB_MATCHER_H_HC68VFZ */
