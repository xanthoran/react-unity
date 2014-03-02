/*  reacTIVision fiducial tracking framework
    FrameEqualizer.cpp
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

#include "FrameEqualizer.h"

bool FrameEqualizer::init(int w, int h, int sb, int db) {
	
	FrameProcessor::init(w,h,sb,db);
	
	if (sb!=1) {
		printf("source buffer must be grayscale");
		return false;
	}

	help_text.push_back( "FrameEqualizer:");
	help_text.push_back( "   e - toggle frame equalizer");
	help_text.push_back( "   SPACE - activate frame equalizer");
	
	int size = width*height;
	pointmap = new short[size];
	for (int i=0;i<size;i++) pointmap[i] = 0;
	return true;
}

void FrameEqualizer::process(unsigned char *src, unsigned char *dest, SDL_Surface *display) {

	if (equalize) {
		int size = width*height;
		for (int i=0;i<size;i++) {
			int e = src[i]+pointmap[i];
 			if (e & (~255)) { if (e < 0) e = 0; else e = 255; }
			src[i] = (unsigned char)e;
		}
	} else if (calibrate) {
		long sum = 0;
		for (int x=width/2-5;x<width/2+5;x++) {
			for (int y=height/2-5;y<height/2+5;y++) {
				sum+=src[y*width+x];
			}
		}
		unsigned char average = (unsigned char)(sum/100);

		for (int i=0;i< width*height;i++) 
			pointmap[i] = average-src[i];

		calibrate = false;
		equalize = true;
	} 
	
}

void FrameEqualizer::toggleFlag(int flag) {

	if (flag=='e') {
		equalize=!equalize;
		calibrate=false;
	}

	if (flag==' ') {
		equalize=false;
		calibrate=true;
	}

}


