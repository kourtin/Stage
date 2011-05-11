#if !defined(MENU_DRAW_H_9I419LL8)
#define MENU_DRAW_H_9I419LL8

#include "interface/menu.h"
#include <cinder/cairo/Cairo.h>

struct menu_draw {
	menu_draw(menu* m) : menu_(m) {}
	virtual ~menu_draw() {}
	void change(menu* m) { menu_ = m; }
	menu* which() { return menu_; }
	virtual void draw(ci::cairo::Context ctx, int w, int h) = 0;
protected:
	menu* menu_;
};

struct menu_draw_lineaire : public menu_draw {
	menu_draw_lineaire(menu* m) : menu_draw(m) {}
	virtual ~menu_draw_lineaire() {}
	virtual void draw(ci::cairo::Context ctx, int w, int h);
};

#endif /* end of include guard: MENU_DRAW_H_9I419LL8 */
