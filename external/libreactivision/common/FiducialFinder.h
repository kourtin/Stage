/*  reacTIVision fiducial tracking framework
    FiducialFinder.h
    Copyright (C) 2005-2008 Martin Kaltenbrunner <mkalten@iua.upf.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef FIDUCIALFINDER
#define FIDUCIALFINDER

#include <vector>
#include <list>
#include <sstream>
#include <string>

#include "FrameProcessor.h"
#include "TuioServer.h"
// #include "MidiServer.h"
#include "FiducialObject.h"
// #include "PortVideoSDL.h"
#include "floatpoint.h"

#include <CoreFoundation/CFBundle.h>

class objet_store;

class FiducialFinder: public FrameProcessor
{
public:	
	FiducialFinder(MessageServer *serv, const char* grid_cfg);
	
	~FiducialFinder() {
		if (dmap) delete[] dmap;
	}
	
	virtual void process(unsigned char *src, unsigned char *dest) = 0;
	
	bool init(int w, int h, int sb ,int db);
	// void toggleFlag(int flag);
	void finish();
	void update(objet_store*);
protected:
	std::list<FiducialObject> fiducialList;
	
	int session_id;
	long totalframes;

	const char* grid_config;
	int cell_width, cell_height;
	int grid_size_x, grid_size_y;
	#ifdef __APPLE__
	char full_path[1024];
	#endif

	bool calibration, show_grid, empty_grid;
	ShortPoint* dmap;

	// void drawObject(int id, int xpos, int ypos, SDL_Surface *display,int state);
	// void drawGrid(unsigned char *src, unsigned char *dest, SDL_Surface *display);
	// void drawGUI(SDL_Surface *display);
	void computeGrid();
	
	TuioServer *tuio_server;
	// MidiServer *midi_server;
	MessageServer *message_server;
	
	void sendTuioMessages();
	// void sendMidiMessages();

	MessageListener::DisplayMode prevMode;
	bool show_settings;
	
private:
	enum InvertSetting { INV_NONE, INV_XPOS, INV_YPOS, INV_ANGLE };
	int currentSetting;

};

#endif
