/*  reacTIVision fiducial tracking framework
    DtouchFinder.cpp
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

#include "DtouchFinder.h"

bool DtouchFinder::init(int w, int h, int sb, int db) {
	
	FiducialFinder::init(w,h,sb,db);

	if (sb!=1) {
		printf("source buffer must be grayscale\n");
		return false;
	}
		
	frecognition = new FiducialRecognition(width, height);
	return true;
}
void DtouchFinder::process(unsigned char *src, unsigned char *dest, SDL_Surface *display) {

	FiducialData *fdata = NULL;
	int count = frecognition->process(src,dest,&fdata);

	// process symbols
	for(int i=0;i< count;i++) {

		//apply distortion
		int index = fdata[i].getCentre().y*width+fdata[i].getCentre().x;
		fdata[i].setCentre(DTPoint(dmap[index].x,dmap[index].y));

		FiducialObject *existing_fiducial = NULL;

		// check if we have an ID/Position conflict
		for (std::list<FiducialObject>::iterator fiducial = fiducialList.begin(); fiducial!=fiducialList.end(); fiducial++) {

			float distance = fiducial->distance(fdata[i].getCentre().x,fdata[i].getCentre().y);

			if (fdata[i].getId()==fiducial->fiducial_id) {
				// find and match a fiducial we had last frame already ...
				if(existing_fiducial) {
					if (distance<existing_fiducial->distance(fdata[i].getCentre().x,fdata[i].getCentre().y))
						existing_fiducial = &(*fiducial);
				} else {
					existing_fiducial = &(*fiducial);
					for (int j=0;j<count;j++) {
						if ((i!=j) && (fiducial->distance(fdata[j].getCentre().x,fdata[j].getCentre().y)<distance)) {
							existing_fiducial = NULL;
							break;
						}
					}	
				}
			} else if (distance<5.0f) {
				// do we have a different ID at the same place?

				// this correct wrong or invalid fiducial IDs
				// assuming that between two frames
				// there can't be a rapid exchange of tw symbols
				// at the same place
				fdata[i].setId(fiducial->fiducial_id);
				existing_fiducial = &(*fiducial);
				break;
			}
		}
		
		if  (existing_fiducial!=NULL) {
			// just update the fiducial from last frame ...
			existing_fiducial->update(fdata[i].getCentre().x,fdata[i].getCentre().y,fdata[i].getAngle(),0,0);
		} else {
			// add the newly found object
			FiducialObject addFiducial(session_id, fdata[i].getId(), width, height);
			addFiducial.update(fdata[i].getCentre().x,fdata[i].getCentre().y,fdata[i].getAngle(),0,0);
			fiducialList.push_back(addFiducial);
			if (midi_server!=NULL) midi_server->sendAddMessage(fdata[i].getId());
			if (msg_listener) {
				//char add_message[16];
				//sprintf(add_message,"add obj %d %ld",fdata[i].getId(),session_id);
				//msg_listener->setMessage(std::string(add_message));
				std::stringstream add_message;
				add_message << "add obj " << " " << fdata[i].getId() << " " << session_id;
				msg_listener->setMessage(add_message.str());
			}
			session_id++;
		}

		drawObject(fdata[i].getId(),(int)(fdata[i].getCentre().x),(int)(fdata[i].getCentre().y),display,1);
	}

	if (tuio_server) sendTuioMessages();
	if (midi_server) sendMidiMessages();
	if (show_grid) drawGrid(src,dest,display);
	if (show_settings) drawGUI(display);
	delete[] fdata;
}
