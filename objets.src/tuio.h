#if !defined(TUIO_H_NMDUZ1N6)
#define TUIO_H_NMDUZ1N6

#include "objet_store.h"
#include <TuioListener.h>
#include <TuioClient.h>

struct tuio_demux : public TUIO::TuioListener {
	tuio_demux(objet_store& store) : store_(store), client_(3333) {
		client_.addTuioListener(this);
		client_.connect();
	}
	virtual ~tuio_demux() {
		client_.disconnect();
	}
	
	void addTuioObject(TUIO::TuioObject *tobj);
	void updateTuioObject(TUIO::TuioObject *tobj);
	void removeTuioObject(TUIO::TuioObject *tobj);

	void addTuioCursor(TUIO::TuioCursor *tcur) {}
	void updateTuioCursor(TUIO::TuioCursor *tcur) {}
	void removeTuioCursor(TUIO::TuioCursor *tcur) {}

	void refresh(TUIO::TuioTime frameTime) {}
	
private:
	objet_store& store_;
	TUIO::TuioClient client_;
};

#endif /* end of include guard: TUIO_H_NMDUZ1N6 */
