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