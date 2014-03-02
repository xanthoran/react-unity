/*  portVideo, a cross platform camera framework
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

#ifndef REGIONPROCESSOR_H
#define REGIONPROCESSOR_H

#include <SDL.h>
#include "segment.h"
#include "fidtrackX.h"

class RegionProcessor
{
public:

	RegionProcessor() {
		width     = 0;
		height    = 0;
		srcBytes  = 0;
		destBytes = 0;
		srcSize   = 0;
		destSize  = 0;
		dispSize  = 0;
		initialized = false;
	};	
	virtual ~RegionProcessor() {};
	
	virtual bool init(int w, int h, int sb, int db) {
		width  = w;
		height = h;
		srcBytes  = sb;
		destBytes = db;
		
		srcSize   = w*h*sb;
		destSize  = w*h*db;
		dispSize  = w*h*4;
		
		initialized = true;
		return true;
	};

	virtual void process(RegionX *region, unsigned char *src, unsigned char *dest, SDL_Surface *display) = 0;
	
	virtual void finish() {};
	
protected:
	int width;
	int height;
	int srcBytes;
	int destBytes;
	int srcSize;
	int destSize;
	int dispSize;

	bool initialized;
};

#endif
