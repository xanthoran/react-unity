/*  reacTIVision fiducial tracking framework
    ObjectAnalyzer.h
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

#ifndef OBJECTANALYZER_H
#define OBJECTANALYZER_H

#include "RegionProcessor.h"
#include "segment.h"
#include "fidtrackX.h"
//#include <unistd.h>

class ObjectAnalyzer: public RegionProcessor
{
public:	
	ObjectAnalyzer():RegionProcessor() {};
	~ObjectAnalyzer() {
		if (initialized) {

		}
	};
	
	void process(RegionX *region, unsigned char *src, unsigned char *dest, SDL_Surface *display);
	bool init(int w ,int h, int sb, int db);
	
private:
};

#endif
