#include "objet.h"
#include <cmath>
#include <cstdlib>

objet::objet(objet_store* store, int id, float x, float y, float r, float v, float a, bool present) : id_(id), x_(x), y_(y), r_(r), v_(v), a_(a), z_(-1), present_(present), rayon_(0.01f + 0.001f*(rand() % 30)), store_(store) {
	
}