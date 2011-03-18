#if !defined(OBJET_H_PUVK6WJW)
#define OBJET_H_PUVK6WJW

#include "blobby.h"
#include <string>

struct objet {
	objet(blobby& b) : nom_("@@@@@@@@@"), blob_(b), distance_avant_(-1) {}
	void actualiser(std::vector<blobby>&);
	// bool present() { return blob_ != 0; }
	blobby& blob() { return blob_; }
	std::string nom() { return nom_; }
private:
	std::string nom_;
	blobby blob_;
	float distance_avant_;
};

#endif /* end of include guard: OBJET_H_PUVK6WJW */
