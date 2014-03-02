/*  reacTIVision fiducial tracking framework
    CalibrationEngine.cpp
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

#include "CalibrationEngine.h"
#ifdef __APPLE__
#include <CoreFoundation/CFBundle.h>
#endif

CalibrationEngine::CalibrationEngine(const char* out) {
	
	calibration = false;
	//auto_calibration = false;
	//manual_calibration = true;
	file_exists = false;

	if (strcmp(out, "none" ) == 0 ) {
#ifdef __APPLE__
		char path[1024];
		CFBundleRef mainBundle = CFBundleGetMainBundle();
		CFURLRef mainBundleURL = CFBundleCopyBundleURL( mainBundle);
		CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
		CFStringGetCString( cfStringRef, path, 1024, kCFStringEncodingASCII);	
		CFRelease( mainBundleURL);
		CFRelease( cfStringRef);
		sprintf(full_path,"%s/Contents/Resources/calibration.grid",path);
		calib_out = full_path;
#else
		calib_out = "./calibration.grid";
#endif
	} else calib_out = out;


	#ifdef WIN32
	FILE *infile = fopen (calib_out, "r");
	if (infile != NULL) {
		file_exists = true;
		fclose (infile);
	}
	#else
	file_exists = (access (calib_out, F_OK )==0);
	#endif

	grid_size_x = 9;
	grid_size_y = 7;
	
	field_count_x = grid_size_x-1;
	field_count_y = grid_size_y-1;
	
	grid = new CalibrationGrid(grid_size_x,grid_size_y);
	grid->Reset();

	if (file_exists) {
		grid->Load(calib_out);
		sprintf(calib_bak,"%s.bak",calib_out);
		grid->Store(calib_bak);
	}
	
	grid_xpos = (char)(grid_size_x/2);
	grid_ypos = (char)(grid_size_y/2);

	help_text.push_back( "CalibrationEngine:");
	help_text.push_back( "   c - toggle calibration mode");
	help_text.push_back( "   j - reset calibration grid");
	help_text.push_back( "   k - reset selected point");
	help_text.push_back( "   l - revert to saved grid");
	//help_text.push_back( "   z - toggle manual calibration");
	help_text.push_back( "   r - show calibration result");
	help_text.push_back( "   a,d,w,x - move within grid");
	help_text.push_back( "   cursor keys - adjust grid point");
};

CalibrationEngine::~CalibrationEngine() {
	delete grid;

	/*
	if (auto_calibration) {
		delete []finger_marks;
		delete []fiducial_marks;
	}
	*/
	
	if (file_exists) {
		remove(calib_bak);
	}
};

/*
void CalibrationEngine::activateAutoCalibration(FidtrackFinder *ft) {

	if (ft==NULL) return;
	
	fidtracker = ft;
	auto_calibration = true;
	
	finger_marks = new PointPair[grid_size_x*grid_size_y];
	fiducial_marks = new PointPair[field_count_x*field_count_y];
}
*/

bool CalibrationEngine::init(int w, int h, int sb, int db) {

	FrameProcessor::init(w,h,sb,db);
	
	cell_size_x = w/field_count_x;
	cell_size_y = h/field_count_y;
	return true;
}


void CalibrationEngine::setFlag(int flag, bool value) {
	toggleFlag(flag);
}

void CalibrationEngine::toggleFlag(int flag) {

	if (flag=='c') {
		if (calibration) {
			calibration = false;
			if(msg_listener) msg_listener->setDisplayMode(prevMode);
		} else {
			calibration = true;
			//manual_calibration=true;
			grid_xpos = (char)(grid_size_x/2);
			grid_ypos = (char)(grid_size_y/2);
			//if (auto_calibration) fidtracker->reset();
			if(msg_listener) {
				prevMode = msg_listener->getDisplayMode();
				msg_listener->setDisplayMode(msg_listener->SOURCE_DISPLAY);
			}
		}
	}
/*
	if (flag=='z') {
		if (calibration) manual_calibration=!manual_calibration;
		if (manual_calibration) {
			grid_xpos = (char)(grid_size_x/2);
			grid_ypos = (char)(grid_size_y/2);
		}
	}
*/			
	if(!calibration) return;

	if ((flag=='t') || (flag=='n'))  {
		msg_listener->setDisplayMode(msg_listener->SOURCE_DISPLAY);
	}

	//if (!manual_calibration) return;

	if (flag=='a') {
		grid_xpos--;
		if (grid_xpos<0) grid_xpos=0;
	} else if (flag=='d') {
		grid_xpos++;
		if (grid_xpos>field_count_x) grid_xpos=field_count_x;
	} else if (flag=='x') {
		grid_ypos++;
		if (grid_ypos>field_count_y) grid_ypos=field_count_y;
	} else if (flag=='w') {
		grid_ypos--;
		if (grid_ypos<0) grid_ypos=0;
	} else if (flag=='j') {
		grid->Reset();
	} else if (flag=='k') {
		grid->Set(grid_xpos,grid_ypos,0.0,0.0);
	} else if (flag=='l') {
		if (file_exists) grid->Load(calib_bak);
	} else if (flag==276) {
		GridPoint p = grid->GetInterpolated(grid_xpos,grid_ypos);
		grid->Set(grid_xpos,grid_ypos,p.x-0.01,p.y);
	} else if (flag==275)  {
		GridPoint p = grid->GetInterpolated(grid_xpos,grid_ypos);
		grid->Set(grid_xpos,grid_ypos,p.x+0.01,p.y);
	} else if (flag==273) {
		GridPoint p = grid->GetInterpolated(grid_xpos,grid_ypos);
		grid->Set(grid_xpos,grid_ypos,p.x,p.y-0.01);
	} else if (flag==274) {
		GridPoint p = grid->GetInterpolated(grid_xpos,grid_ypos);
		grid->Set(grid_xpos,grid_ypos,p.x,p.y+0.01);
	}

	grid->Store(calib_out);
}

void CalibrationEngine::process(unsigned char *src, unsigned char *dest, SDL_Surface *display) {

	if(!calibration) return;
	drawDisplay( (unsigned char*)(display->pixels));
}

void CalibrationEngine::drawDisplay(unsigned char* display) {

	// draw the circles
	int offset = (width-height)/2;
	for (double a=-M_PI;a<M_PI;a+=0.005) {
		int half = height/2;

		// ellipse
		float x = width/2+cos(a)*width/2;
		float y = half+sin(a)*half;

		GridPoint gp = grid->GetInterpolated(x/cell_size_x,y/cell_size_y);
		int gx = (int)(x + gp.x*cell_size_x);
		int gy = (int)(y + gp.y*cell_size_y);

		if ((gx>=0) && (gx<width) && (gy>=0) && (gy<height)) {
			int pixel = (int)(gy*width+gx);
			display[pixel*3] = 255;
		}
		
		// outer circle
		x = offset+half+cos(a)*half;
		y = half+sin(a)*half;

		gp = grid->GetInterpolated(x/cell_size_x,y/cell_size_y);
		gx = (int)(x + gp.x*cell_size_x);
		gy = (int)(y + gp.y*cell_size_y);

		if ((gx>=0) && (gx<width) && (gy>=0) && (gy<height)) {
			int pixel = (int)(gy*width+gx)*3;
			display[pixel] = 255;
		}

		// middle circle
		x = offset+half+cos(a)*(half-cell_size_x);
		y = half+sin(a)*(half-cell_size_y);

		gp = grid->GetInterpolated(x/cell_size_x,y/cell_size_y);
		gx = (int)(x + gp.x*cell_size_x);
		gy = (int)(y + gp.y*cell_size_y);

		if ((gx>=0) && (gx<width) && (gy>=0) && (gy<height)) {
			int pixel = (int)(gy*width+gx)*3;
			display[pixel] = 255;
		}

		// inner circle
		x = offset+half+cos(a)*cell_size_x;
		y = half+sin(a)*cell_size_y;

		gp = grid->GetInterpolated(x/cell_size_x,y/cell_size_y);
		gx = (int)(x + gp.x*cell_size_x);
		gy = (int)(y + gp.y*cell_size_y);

		if ((gx>=0) && (gx<width) && (gy>=0) && (gy<height)) {
			int pixel = (int)(gy*width+gx)*3;
			display[pixel] = 255;
		}

	}
	
	// draw the horizontal lines
	for (int i=0;i<grid_size_y;i++) {
		float start_x = 0;
		float start_y = i*cell_size_y;
		float end_x = field_count_x*cell_size_x;
		float end_y = start_y;
				
		for (float lx=start_x;lx<end_x;lx++) {
			float ly = end_y - (end_y-start_y)/(end_x-start_x)*(end_x-lx);
				
			GridPoint gp = grid->GetInterpolated(lx/cell_size_x,ly/cell_size_y);
			int gx = (int)(lx+gp.x*cell_size_x);
			int gy = (int)(ly+gp.y*cell_size_y);

			if ((gx>=0) && (gx<width) && (gy>=0) && (gy<height)) {
				int pixel = (gy*width+gx)*3+1;
				display[pixel] = 255;
				display[pixel-1] = 0;
			}
		}
	}

	// draw the vertical lines
	for (int i=0;i<grid_size_x;i++) {

		float start_x = i*cell_size_x;
		float start_y = 0;
		float end_x = start_x;
		float end_y = field_count_y*cell_size_y;
				
		for (float ly=start_y;ly<end_y;ly++) {
			float lx = end_x - (end_x-start_x)/(end_y-start_y)*(end_y-ly);	
				
			GridPoint gp = grid->GetInterpolated(lx/cell_size_x,ly/cell_size_y);
			int gx = (int)(lx+gp.x*cell_size_x);
			int gy = (int)(ly+gp.y*cell_size_y);
			
			
			if ((gx>=0) && (gx<width) && (gy>=0) && (gy<height)) {
				int pixel = (gy*width+gx)*3+1;
				display[pixel] = 255;
				display[pixel-1] = 0;
			}
		}
	}

	// draw the red box for the selected point
	//if (manual_calibration) {
	GridPoint p = grid->GetInterpolated(grid_xpos,grid_ypos);
	int gx =(int)(grid_xpos*cell_size_x + p.x*cell_size_x);
	int gy =(int)(grid_ypos*cell_size_y + p.y*cell_size_y);

	for (int xx=gx-2;xx<=gx+2;xx++) {
		for (int yy=gy-2;yy<=gy+2;yy++) {
			if ((xx>=0) && (xx<width) && (yy>=0) && (yy<height)) {
				int pixel = (yy*width+xx)*3+2;
				display[pixel] = 255;
				display[pixel-1] = 0;
				display[pixel-2] = 0;
			}
		}
	}
	//}
	
	// draw the grid points
	for (int i=0;i<grid_size_x;i++) {
		for (int j=0;j<grid_size_y;j++) {
			GridPoint gp = grid->GetInterpolated(i,j);
			int gx = (int)(i*cell_size_x + gp.x*cell_size_x);
			int gy = (int)(j*cell_size_y + gp.y*cell_size_y);

			for (int xx=gx-1;xx<=gx+1;xx++) {
				for (int yy=gy-1;yy<=gy+1;yy++) {
					if ((xx>=0) && (xx<width) && (yy>=0) && (yy<height)) {
						int pixel = (yy*width+xx)*3;
						display[pixel] = 0;
						display[pixel+1] = 0;
						display[pixel+2] = 0;
					}
				}
			}
		}
	}

/*
	for (int y=0;y<grid_size_y;y++) {
		for (int x=0;x<grid_size_x;x++) {
			int index = y*grid_size_x+x;
			if ((finger_marks[index].original.x>=0) &&  (finger_marks[index].original.y>=0)) {
				if (finger_marks[index].original.x>=width) continue;
				if (finger_marks[index].original.y>=height) continue;
				int pixel = (finger_marks[index].original.y*width+finger_marks[index].original.x)*3;
				display[pixel] = 255;
			} 
		}
	} 	 
	
	int mk = 0;
	for (int y=0;y<field_count_y;y++) {
		for (int x=0;x<field_count_x;x++) {
			int index = y*field_count_x+x;
			if ((fiducial_marks[index].original.x>=0) &&  (fiducial_marks[index].original.y>=0)) {			
				int pixel = (fiducial_marks[index].original.y*width+fiducial_marks[index].original.x)*3;
				if ((pixel>0) && (pixel<width*height*3)) { display[pixel] = 255;	mk++; }			
			
			}
		} 
	}
	std::cout << mk << std::endl;
*/	
	
}


