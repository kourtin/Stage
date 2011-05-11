#include "sondes/tuio.h"
#include "objets/objet.h"

tuio_demux::tuio_demux() : client_(3333) {
	client_.addTuioListener(this);
	client_.connect();
}
tuio_demux::~tuio_demux() {
	client_.disconnect();
}

void tuio_demux::addTuioObject(TUIO::TuioObject *tobj) {
	objet o(0, tobj->getSymbolID());
	o.x(tobj->getX());
	o.y(tobj->getY());
	o.r(tobj->getAngle());
	o.present(true);
	pending_.push_back(o);
}

void tuio_demux::updateTuioObject(TUIO::TuioObject *tobj) {
	objet o(0, tobj->getSymbolID());
	o.x(tobj->getX());
	o.y(tobj->getY());
	o.r(tobj->getAngle());
	o.present(true);
	pending_.push_back(o);
}

void tuio_demux::removeTuioObject(TUIO::TuioObject *tobj) {
	objet o(0, tobj->getSymbolID());
	o.x(tobj->getX());
	o.y(tobj->getY());
	o.r(tobj->getAngle());
	o.present(false);
	pending_.push_back(o);
}

void tuio_demux::operator()(objet_store* store) {
	if(!store) return;
	while(!pending_.empty()) {
		objet op = pending_.front();
		objet& o = store->get_or_create(op.id());
		o.x(op.x());
		o.y(op.y());
		o.r(op.r());
		o.present(op.present());
		pending_.pop_front();
	}
}