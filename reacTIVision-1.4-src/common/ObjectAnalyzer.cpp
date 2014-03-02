/*  reacTIVision fiducial tracking framework
    ObjectAnalyzer.cpp
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

#include "ObjectAnalyzer.h"

bool ObjectAnalyzer::init(int w, int h, int sb, int db) {
	RegionProcessor::init(w,h,sb,db);
	return true;
}

void ObjectAnalyzer::process(RegionX *region, unsigned char *src, unsigned char *dest, SDL_Surface *display) {

	unsigned char* disp = (unsigned char*)(display->pixels);

	//draw a blue debug rectangle around the region candidate into the display buffer 
	for (int x=region->left;x<=region->right;x++) {
		disp[3*(region->top*width+x)] = 255;
		disp[3*(region->bottom*width+x)] = 255;
	}

	for (int y=region->top;y<=region->bottom;y++) {
		disp[3*(y*width+region->left)] = 255;
		disp[3*(y*width+region->right)] = 255;
	}

/*
	
	region->area = 0;
	
	Span *span = region->span;
	while (span) {
		region->area += span->end - span->start + 1;


		//fill the region area
		for (int i=span->start;i<=span->end;i++) {
			disp[3*i] = 255;
			disp[3*i+1] = 255;
		}	

		//fill the region contour
		disp[3*span->start+1] = 255;
		disp[3*span->end+1] = 255;
		
		span = span->next;
	}
 
*/
}



