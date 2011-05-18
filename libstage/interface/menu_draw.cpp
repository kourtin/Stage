#include "interface/menu_draw.h"

void menu_draw_lineaire::draw(ci::cairo::Context ctx, int w, int h) {
	ctx.setSourceRgba(0, 0, 0, 0.5);
	ctx.rectangle(0, 50, w, 200);
	ctx.setLineWidth(0);
	ctx.fill();
	// ctx.setFont(fnt_ttl_);
	ctx.setFontSize(20);
	ctx.setSourceRgb(1, 1, 1);
	ctx.moveTo(5, 60);
	ctx.showText(menu_->titre());
	ctx.stroke();
	
	double y = 80;
	for(menu::iterator it = menu_->begin(); it != menu_->end(); ++it) {
		if(*it) {
			ctx.setFontSize(13);
			ctx.setSourceRgb(1, 1, 0.6);
			ctx.moveTo(15, y + 2);
			ctx.showText((*it)->titre());
			ctx.stroke();
			if(it == menu_->selection()) {
				ctx.setLineWidth(1);
				ctx.rectangle(5, y - 12, w - 10, 20);
				ctx.stroke();
			}
			y += 20;
		} else {
			std::cout << "erreur menu::draw" << std::endl;
		}
	}
}

void menu_draw_circulaire::draw(ci::cairo::Context ctx, int w, int h) {
	float x = w / 2;
	float y = h / 2;
	ci::Color couleur(1.0, 0.2, 0.2);
	if(o_) {
		x = o_->x() * w;
		y = o_->y() * h;
		couleur = o_->couleur();
	}
	static const float rayon = 50;
	
	// Affiche le titre bien au-dessus
	ci::cairo::TextExtents e = ctx.textExtents(menu_->titre());
	ctx.moveTo(x - e.width() * 0.5, y - e.height() * 0.5 - rayon - 15);
	ctx.setSourceRgb(1, 1, 1);
	ctx.setFontSize(15);
	ctx.showText(menu_->titre());
	ctx.stroke();
	
	// Calcule le nombre d'éléments du menu et l'arc qu'occupe un élément
	int nb_elts = menu_->size();
	if(nb_elts == 0)
		return;
	double angle_elt = 2*M_PI / nb_elts;
	// std::cout << angle_elt << std::endl;
	
	// Affiche chaque élément
	ctx.setFontSize(12);
	int i = 0;
	// int decalage = std::distance(menu_->begin(), menu_->selection());
	for(menu::iterator it = menu_->begin(); it != menu_->end(); ++it) {
		if(*it) {
			ctx.save();
			ctx.translate(x,y);
			ctx.rotate(angle_elt * i);
			if(it == menu_->selection())
				ctx.setSource(ci::ColorA(couleur, 0.8));
			else
				ctx.setSource(ci::ColorA(couleur, 0.3));
			
			ctx.moveTo(0,0);
			ctx.arc(0, 0, rayon, 0, angle_elt);
			// ctx.moveTo(0,0);
			ctx.closePath();
			ctx.fill();
			ctx.save();
			ctx.rotate(angle_elt * 0.5);
			if(it == menu_->selection())
				ctx.setSourceRgb(1,1,1);
			else
				ctx.setSourceRgba(1,1,1,0.4);
			
			ctx.showText((*it)->titre());
			ctx.stroke();
			ctx.restore();
			ctx.restore();
		} else {
			std::cout << "erreur menu::draw" << std::endl;
		}
		++i;
	}
}