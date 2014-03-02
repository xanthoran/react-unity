/*  reacTIVision fiducial tracking framework
	DtouchFinder.h
	Copyright (C) 2005-2008 Martin Kaltenbrunner <mkalten@iua.upf.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 2.0, as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License v2.0
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef DTOUCHFINDER
#define DTOUCHFINDER_H

#include "FiducialFinder.h"

#include "fiducialrecognition.h"
#include "types.h"

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif


class DtouchFinder: public FiducialFinder
{
public:	
	DtouchFinder(MessageServer *server, const char* grid_cfg) : FiducialFinder (server,grid_cfg) 
{};
	~DtouchFinder() {
		delete frecognition;
	};
	
	void process(unsigned char *src, unsigned char *dest, SDL_Surface *display);
	bool init(int w ,int h, int sb, int db);
	
private:
	FiducialRecognition *frecognition;
};

#endif
