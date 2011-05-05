#include "tuio.h"
#include "objet.h"
// TODO: découplage thread

void tuio_demux::addTuioObject(TUIO::TuioObject *tobj) {
	// std::cout << "+" << tobj->getSymbolID() << " @" << tobj->getX() << "," << tobj->getY() << std::endl;
	objet& o = store_.get_or_create(tobj->getSymbolID());
	o.x(tobj->getX());
	o.y(tobj->getY());
	o.r(tobj->getAngle());
	o.v(tobj->getMotionSpeed());
	o.a(tobj->getMotionAccel());
	o.present(true);
	// store_.update(tobj->getSymbolID(), tobj->getX(), tobj->getY(), tobj->getAngle(), tobj->getMotionSpeed(), tobj->getMotionAccel(), true);
}

void tuio_demux::updateTuioObject(TUIO::TuioObject *tobj) {
	// std::cout << "=" << tobj->getSymbolID() << " @" << tobj->getX() << "," << tobj->getY() << " >" << tobj->getMotionSpeed() << std::endl;
	objet& o = store_.get_or_create(tobj->getSymbolID());
	o.x(tobj->getX());
	o.y(tobj->getY());
	o.r(tobj->getAngle());
	o.v(tobj->getMotionSpeed());
	o.a(tobj->getMotionAccel());
	o.present(true);
	// store_.update(tobj->getSymbolID(), tobj->getX(), tobj->getY(), tobj->getAngle(), tobj->getMotionSpeed(), tobj->getMotionAccel(), true);
}

void tuio_demux::removeTuioObject(TUIO::TuioObject *tobj) {
	// std::cout << "-" << tobj->getSymbolID() << std::endl;
	objet& o = store_.get_or_create(tobj->getSymbolID());
	o.x(tobj->getX());
	o.y(tobj->getY());
	o.r(tobj->getAngle());
	o.v(tobj->getMotionSpeed());
	o.a(tobj->getMotionAccel());
	o.present(false);
	// store_.update(tobj->getSymbolID(), tobj->getX(), tobj->getY(), tobj->getAngle(), tobj->getMotionSpeed(), tobj->getMotionAccel(), false);
}

// int main() {
// 	tuio t;
// 	TUIO::TuioClient c(3333);
// 	c.addTuioListener(&t);
// 	c.connect();
// 	for(;;);
// 	return 0;
// }