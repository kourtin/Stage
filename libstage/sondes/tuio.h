#if !defined(TUIO_H_NMDUZ1N6)
#define TUIO_H_NMDUZ1N6

#include "sondes/sonde_environnementale.h"
#include "objets/objet_store.h"
#include <TuioListener.h>
#include <TuioClient.h>
#include <deque>

struct tuio_demux : public TUIO::TuioListener, public sonde_primaire {
	typedef std::deque<objet> liste_objets;
	typedef liste_objets::iterator iterator;
	tuio_demux();
	virtual ~tuio_demux();
	
	virtual void operator()(objet_store* store = 0);
	
	void addTuioObject(TUIO::TuioObject *tobj);
	void updateTuioObject(TUIO::TuioObject *tobj);
	void removeTuioObject(TUIO::TuioObject *tobj);

	void addTuioCursor(TUIO::TuioCursor *tcur) {}
	void updateTuioCursor(TUIO::TuioCursor *tcur) {}
	void removeTuioCursor(TUIO::TuioCursor *tcur) {}

	void refresh(TUIO::TuioTime frameTime) {}
	
private:
	TUIO::TuioClient client_;
	liste_objets pending_;
};

#endif /* end of include guard: TUIO_H_NMDUZ1N6 */
