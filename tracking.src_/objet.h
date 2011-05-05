#if !defined(OBJET_H_PUVK6WJW)
#define OBJET_H_PUVK6WJW

#include "blobby.h"
#include <string>

struct objet {
	objet(blobby& b);
	void actualiser(std::vector<blobby>&);
	bool present() { return present_; }
	blobby& blob() { return blob_; }
	std::string nom() { return nom_; }
private:
	std::string nom_;
	bool present_;
	blobby blob_;
	float distance_avant_, distance_initiale_;
};

#endif /* end of include guard: OBJET_H_PUVK6WJW */
