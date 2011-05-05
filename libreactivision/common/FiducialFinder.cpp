/*  reacTIVision fiducial tracking framework
    FiducialFinder.cpp
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

#include "FiducialFinder.h"
#include "CalibrationGrid.h"
#include "objet_store.h"

FiducialFinder::FiducialFinder(MessageServer *serv, const char* grid_cfg) {
	if(serv == 0) return;
	message_server = serv;
	if (serv->getType()==TUIO_SERVER) {
		tuio_server = (TuioServer*)serv;
		// midi_server = NULL;
	} // else if (server->getType()==MIDI_SERVER) {
	// 			midi_server = (MidiServer*)server;
	// 			tuio_server = NULL;
	// 		}

if (strcmp(grid_cfg, "none" ) == 0 ) {
	char path[1024];
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef mainBundleURL = CFBundleCopyBundleURL( mainBundle);
	CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
	CFStringGetCString( cfStringRef, path, 1024, kCFStringEncodingASCII);	
	CFRelease( mainBundleURL);
	CFRelease( cfStringRef);
	sprintf(full_path,"%s/Contents/Resources/calibration.grid",path);
	grid_config = full_path;
} else grid_config = grid_cfg;

	calibration = false;
	totalframes = 0;
	session_id = 0;
	
	currentSetting = INV_NONE;
	show_settings = false;
}

bool FiducialFinder::init(int w, int h, int sb ,int db) {
	FrameProcessor::init(w,h,sb,db);

	help_text.push_back( "FiducialFinder:");
	help_text.push_back( "   i - invert x-axis, y-axis or angle");
	
	show_grid=false;
	dmap = new ShortPoint[height*width];
	computeGrid();

	return true;

}

void FiducialFinder::computeGrid() {

	// load the distortion grid
	grid_size_x = 9;
	grid_size_y = 7;
	cell_width = width/(grid_size_x-1);
	cell_height = height/(grid_size_y-1);

	CalibrationGrid grid(grid_size_x,grid_size_y);
	grid.Load(grid_config);

	// we do not calculate the matrix if the grid is not configured
	if (grid.IsEmpty()) {
		empty_grid = true;
		for (int y=0;y<height;y++) {
			for (int x=0;x<width;x++) {
				dmap[y*width+x].x = x;
				dmap[y*width+x].y = y;
			}
		}
		return;
	} else empty_grid = false;

	if(msg_listener) msg_listener->displayMessage("computing distortion matrix ...");
	// reset the distortion matrix
	for (int y=0;y<height;y++) {
		for (int x=0;x<width;x++) {
			dmap[y*width+x].x = 0;
			dmap[y*width+x].y = 0;
		}
	}

	// calculate the distortion matrix
	for (float y=0;y<height;y+=0.5) {
		for (float x=0;x<width;x+=0.5) {
		
			// get the displacement
			GridPoint new_point =  grid.GetInterpolated(x/cell_width,y/cell_height);

			// apply the displacement
			short dx = (short)floor(x+0.5f+new_point.x*cell_width);
			short dy = (short)floor(y+0.5f+new_point.y*cell_height);
			
			int pixel =  dy*width+dx;
			if ((dx>=0) && (dx<width) && (dy>=0) && (dy<height)) {
				dmap[pixel].x = (short)x;
				dmap[pixel].y = (short)y;
			}
		}
	}
	
	if(msg_listener) msg_listener->displayMessage("");
}

// void FiducialFinder::toggleFlag(int flag) {

	// if ((flag=='r') && (!calibration) && (!empty_grid)) {
	// 	if (!show_grid) {
	// 		show_grid = true;
	// 		if(msg_listener) {
	// 			prevMode = msg_listener->getDisplayMode();
	// 			msg_listener->setDisplayMode(msg_listener->DEST_DISPLAY);
	// 		}
	// 	} else {
	// 		show_grid = false;
	// 		if(msg_listener) msg_listener->setDisplayMode(prevMode);
	// 	}
	// } else if (flag=='c') {
	// 	if(!calibration) {
	// 		calibration=true;
	// 		show_grid=false;
	// 		for (int y=0;y<height;y++) {
	// 			for (int x=0;x<width;x++) {
	// 				dmap[y*width+x].x = x;
	// 				dmap[y*width+x].y = y;
	// 			}
	// 		}
	// 	}
	// 	else {
	// 		calibration = false;
	// 		computeGrid();
	// 	}
	// } else if (flag=='i') {
	// 	if (currentSetting != INV_NONE) {
	// 		currentSetting = INV_NONE;
	// 		show_settings = false;
	// 		PortVideoSDL::display_lock = false;
	// 	} else if (!PortVideoSDL::display_lock) { 
	// 		currentSetting = INV_XPOS;
	// 		show_settings = true;
	// 		PortVideoSDL::display_lock = true;
	// 	}
	// } else if ( currentSetting != INV_NONE ) {
	// 	switch(flag) {
	// 		case SDLK_LEFT: 
	// 			if (currentSetting==INV_XPOS) message_server->setInvertX(false);
	// 			else if (currentSetting==INV_YPOS) message_server->setInvertY(false);
	// 			else if (currentSetting==INV_ANGLE) message_server->setInvertA(false);
	// 			break;
	// 		case SDLK_RIGHT:
	// 			if (currentSetting==INV_XPOS) message_server->setInvertX(true);
	// 			else if (currentSetting==INV_YPOS) message_server->setInvertY(true);
	// 			else if (currentSetting==INV_ANGLE) message_server->setInvertA(true);
	// 			break;
	// 		case SDLK_UP: 
	// 			currentSetting--;
	// 			if (currentSetting == INV_NONE) currentSetting = INV_ANGLE;
	// 			break;
	// 		case SDLK_DOWN:
	// 			currentSetting++;
	// 			if (currentSetting > INV_ANGLE) currentSetting = INV_XPOS;
	// 			break;
	// 	}
	// }
	
// }

void FiducialFinder::sendTuioMessages() {

		// get the current set of objects on the table
		// we do not only use the raw objects retrieved from the frame
		// but also the remaining (filtered) objects from our fiducialObject list
		bool aliveMessage = false;
		int aliveSize = 0;
		int *aliveList = new int[fiducialList.size()];
		
		for(std::list<FiducialObject>::iterator fiducial = fiducialList.begin(); fiducial!=fiducialList.end(); ) {

			std::string remove_message = fiducial->checkRemoved();
			if (remove_message!="") {
				if(msg_listener) msg_listener->setMessage(remove_message);
				
				fiducial = fiducialList.erase(fiducial);
				// send a single alive message if any
				// of the objects reported to have been removed
				aliveMessage = true;
			} else {
				aliveList[aliveSize]=fiducial->session_id;
				//aliveList[aliveSize]=pos->classId;
				aliveSize++;
				fiducial++;
			} 
		}

		totalframes++;
		long fseq = totalframes;
	
		// after a certain number of frames we should send the redundant alive message
		if ((aliveSize>0) || (totalframes%30==0)) aliveMessage = true;
		if ((aliveSize==0) && (totalframes%30==0)) fseq = -1;
		
		// add the alive message
		if (aliveMessage) {
			tuio_server->addObjAlive(aliveList, aliveSize);
		
			char unsent = 0;
			// send a set message (in case anything changed compared to the last frame)
			for(std::list<FiducialObject>::iterator fiducial = fiducialList.begin(); fiducial!=fiducialList.end(); fiducial++) {
				if (!tuio_server->freeObjSpace()) {
					tuio_server->addObjSeq(fseq);
					tuio_server->sendObjMessages();
				}
				
				std::string set_message = fiducial->addSetMessage(tuio_server);
				if(set_message!="" && msg_listener) msg_listener->setMessage(set_message);
		
				if (fiducial->unsent>unsent) unsent = fiducial->unsent;
			}
	
			// fill the rest of the packet with unchanged objects
			// sending the least sent ones first
			while (unsent>0) {
				for(std::list<FiducialObject>::iterator fiducial = fiducialList.begin(); fiducial!=fiducialList.end(); fiducial++) {
					
					if (fiducial->unsent==unsent) {
						if (tuio_server->freeObjSpace()) { 
							fiducial->redundantSetMessage(tuio_server);
						} else goto send;
					}
				}
				unsent--;
			}
				
			send:
			// finally send the packet
			tuio_server->addObjSeq(fseq);
			tuio_server->sendObjMessages();
			//if (fseq>0) std::cout << "sent frame " << totalframes << " at " << PortVideoSDL::currentTime() << std::endl; 
		}
		delete[] aliveList;
}

void FiducialFinder::finish() {
	
	// on exit we have to send an empty alive message to clear the scene
	if (tuio_server) {
		tuio_server->addObjAlive(NULL, 0);
		tuio_server->addObjSeq(-1);
		tuio_server->sendObjMessages();
		tuio_server->addCurAlive(NULL, 0);
		tuio_server->addCurSeq(-1);
		tuio_server->sendCurMessages();
	} // else if (midi_server){
	 // 		midi_server->sendNullMessages();
	 // 	}
}
#include <iostream>
void FiducialFinder::update(objet_store* store) {
	if(!store) return;
	for(std::list<FiducialObject>::iterator fiducial = fiducialList.begin(); fiducial!=fiducialList.end(); ) {
		objet& o = store->get_or_create(fiducial->fiducial_id);
		if(fiducial->removalFilter()) {
			o.present(false);
			std::cout << "remove" << fiducial->fiducial_id << std::endl;
			fiducial = fiducialList.erase(fiducial);
		}
		else {
			// Change l'objet
			if (( fiducial->current.xpos!=fiducial->last.xpos) || ( fiducial->current.ypos!=fiducial->last.ypos) || ( fiducial->current.angle!=fiducial->last.angle) ) {
				float x = fiducial->current.xpos/fiducial->width;
				float y = fiducial->current.ypos/fiducial->height;
				float a = fiducial->current.angle;
				float X = fiducial->current.motion_speed_x;
				float Y = fiducial->current.motion_speed_y;
				// float A = fiducial->current.rotation_speed;
				float m = fiducial->current.motion_accel;
				// float r = fiducial->current.rotation_accel;
				o.x(x);
				o.y(y);
				o.r(a);
				o.v((X+Y)/2);
				o.a(m);
				o.present(true);
				fiducial->updated = false;
			}
			++fiducial;
		}
	}
	
}


