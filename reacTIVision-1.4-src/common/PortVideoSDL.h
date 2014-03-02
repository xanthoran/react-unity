/*  reacTIVision fiducial tracking framework
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

#ifndef PORTVIDEOSDL_H
#define PORTVIDEOSDL_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#ifdef WIN32
#include <windows.h> 
#else
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#endif

#include <sys/stat.h>

#include <SDL.h>
#include <SDL_thread.h>
#include <time.h>

#define WIDTH 640
#define HEIGHT 480

#include "CameraTool.h"
#include "RingBuffer.h"
#include "FrameProcessor.h"
#include "FontTool.h"

#include "Resources.h"

class PortVideoSDL: public MessageListener
{

public:
	PortVideoSDL(const char* name, bool background, const char* camera_config);
	~PortVideoSDL() {};
	
	void run();
	void stop();

	CameraEngine *camera_;
	unsigned char *cameraBuffer_;
	const char* camera_config;
	
	bool running_;
	bool error_;
	bool pause_;
	bool calibrate_;
	bool help_;
	
	RingBuffer *ringBuffer;
	
	void addFrameProcessor(FrameProcessor *fp);
	void removeFrameProcessor(FrameProcessor *fp);
	void setMessage(std::string message);	
	void displayMessage(const char *message);	
	
	long framenumber_;

	void setDisplayMode(DisplayMode mode);
	DisplayMode getDisplayMode() { return displayMode_; }

	static unsigned int current_fps;
	static bool display_lock;

	static long currentTime() {
#ifdef WIN32
		return GetTickCount();
#else
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv,&tz);
		return ((tv.tv_sec*1000000)+(tv.tv_usec));
#endif
	}
	
protected:
	bool setupWindow();
	void teardownWindow();

	bool setupCamera();
	void teardownCamera();
	
	void initFrameProcessors();
	
	void allocateBuffers();
	void freeBuffers();
	
	void mainLoop();
	void endLoop();
	
	void process_events();
	
#ifndef NDEBUG
	void saveBuffer(unsigned char* buffer);
#endif
	
	void drawHelp();
	void showError(const char* error);

	SDL_Surface *window_;
	SDL_Surface *sourceImage_;
	SDL_Surface *destImage_;
	SDL_Surface *displayImage_;
	SDL_Surface *iconImage_;
	SDL_Color palette_[256];

	unsigned char* sourceBuffer_;
	unsigned char* destBuffer_;
	unsigned char* displayBuffer_;
	
private:
	long frames_;
  	long lastTime_;
	unsigned int cameraTime_, processingTime_, totalTime_;

	void frameStatistics(long cameraTime, long processingTime, long totalTime);
	bool recording_;

	bool calibrated_;

	int width_;
	int height_;
	int fps_;
	int sourceDepth_;
	int destDepth_;
	int bytesPerSourcePixel_;
	int bytesPerDestPixel_;
	
	std::vector<FrameProcessor*> processorList;
	std::vector<FrameProcessor*>::iterator frame;
	
	DisplayMode displayMode_;
	SDL_Thread *cameraThread;
	
	std::string app_name_;
	std::vector<std::string> help_text;
};

#endif
