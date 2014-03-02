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
#if !defined (__x86_64__) && !defined (__ppc64__)
#ifndef macvdCamera_H
#define macvdCamera_H

#include "../common/cameraEngine.h"
#include "VdigGrab.h"
#include <unistd.h>
	#define hibyte(x) (unsigned char)((x)>>8)
	
class MacVdigCamera : public CameraEngine
{
public:
	MacVdigCamera(const char* config_file);
	~MacVdigCamera();
	
	bool findCamera();
	bool initCamera();
	bool startCamera();
	unsigned char* getFrame();
	bool stopCamera();
	bool stillRunning();
	bool resetCamera();
	bool closeCamera();

	int getCameraSettingStep(int mode) { return 0; }
	bool setCameraSettingAuto(int mode, bool flag) { return false; }
	bool setCameraSetting(int mode, int value) { return false; }
	int getCameraSetting(int mode) { return 0; }
	int getMaxCameraSetting(int mode) { return 0; }
	int getMinCameraSetting(int mode) { return 0; }
		
	void showSettingsDialog();
	void drawGUI(SDL_Surface *display) {};
	
private:
	VdigGrab  *pVdg;
	ImageDescriptionHandle vdImageDesc;
	CGrafPtr	dstPort;
	Rect		dstPortBounds;
	Rect		dstDisplayBounds;
	void*		pDstData;
	long		dstDataSize;
};

#endif
#endif
