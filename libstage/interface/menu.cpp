#include "interface/menu.h"
#include <iostream>

menu::menu(std::string titre) : selection_(liste_.end()), titre_(titre) {}

menu::~menu() {
	for(iterator it = liste_.begin(); it != liste_.end(); ++it) {
		if(*it)
			delete *it;
	}
}

void menu::confirmer(objet& o) {
	if(selection_ != liste_.end()) {
		element_menu* ep = *selection();
		if(ep) {
			ep->operator()(o);
		} else {
			std::cout << "erreur menu::confirmer" << std::endl;
		}
	}
}