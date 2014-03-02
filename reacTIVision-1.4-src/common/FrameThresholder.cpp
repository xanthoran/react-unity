/*  reacTIVision fiducial tracking framework
    FrameThresholder.cpp
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

#include "FrameThresholder.h"

bool FrameThresholder::init(int w, int h, int sb, int db) {
	
	FrameProcessor::init(w,h,sb,db);
	tile_size = 12;
	thresholder = new TiledBernsenThresholder();
	initialize_tiled_bernsen_thresholder(thresholder, w, h, tile_size );

	help_text.push_back( "FrameThresholder:");
	help_text.push_back( "   g - adjust gradient gate");

	return true;
}
void FrameThresholder::process(unsigned char *src, unsigned char *dest, SDL_Surface *display) {

	tiled_bernsen_threshold( thresholder, dest, src, srcBytes, width,  height, tile_size, gradient );
	
	//simple_threshold( src, srcBytes,dest,width, height,128 );
	//simple_adaptive_threshold( src, srcBytes,dest,width, height, 16 );
	//overlapped_adaptive_threshold( src, srcBytes,dest,width, height, 8 );
	//overlapped_adaptive_threshold2( src, srcBytes,dest,width, height, 32, gradient );
	
	if (setGradient) drawGUI(display);
}

void FrameThresholder::setFlag(int flag, bool value) {
	if (flag=='g') setGradient=value;
}

void FrameThresholder::toggleFlag(int flag) {

	if (flag=='g') {
		if (setGradient) {
			setGradient=false;
			PortVideoSDL::display_lock=false;		
		} else if (!PortVideoSDL::display_lock) {
			setGradient=true;
			PortVideoSDL::display_lock=true;
		} 
	} else if (setGradient) {
		switch(flag) {
			case SDLK_LEFT: 
				gradient--;
				if (gradient<0) gradient=0;
				break;
			case SDLK_RIGHT:
				gradient++;
				if (gradient>64) gradient=64;
				break;
		}
	}
}

void FrameThresholder::drawGUI(SDL_Surface *display) {
	unsigned char* disp = (unsigned char*)(display->pixels);
	
	int settingValue = gradient;
	int maxValue = 64;
	
	int x_offset=width/2-128;
	int y_offset=height-100;
	
	char displayText[64]; 
	sprintf(displayText,"gradient gate %d",settingValue);
	FontTool::drawText(x_offset+128-(FontTool::getTextWidth(displayText)/2),y_offset-FontTool::getFontHeight(),displayText,display);
	
	// draw the border
	for (int i=x_offset;i<(x_offset+256);i++) {
		int pixel=(width*y_offset+i)*3+1;
		disp[pixel]=255;
		pixel=(width*(y_offset+25)+i)*3+1;
		disp[pixel]=255;
	}
	for (int i=y_offset;i<(y_offset+25);i++) {
		int pixel=(width*i+x_offset)*3+1;
		disp[pixel]=255;
		pixel=(width*i+x_offset+256)*3+1;
		disp[pixel]=255;
	}
	
	// draw the bar
	int xpos = (int)(254*((float)settingValue/(float)maxValue));
	for (int i=x_offset+2;i<=(x_offset+xpos);i++) {
		for (int j=y_offset+2;j<=(y_offset+23);j++) {
			int pixel=(width*j+i)*3+1;
			disp[pixel]=255;
		}
	}
	
	
}

