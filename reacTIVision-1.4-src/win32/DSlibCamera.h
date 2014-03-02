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

#ifndef DSlibCamera_H
#define DSlibCamera_H

#include <DSVL.h>
#include <stdio.h>
#include "../common/CameraEngine.h"

#define hibyte(x) (unsigned char)((x)>>8)

class DSlibCamera : public CameraEngine
{

public:
	DSlibCamera(const char* config_file);
	~DSlibCamera();

	bool findCamera();
	bool initCamera();
	bool startCamera();
	unsigned char* getFrame();
	bool stopCamera();
	bool stillRunning();
	bool resetCamera();
	bool closeCamera();

	int getCameraSettingStep(int mode) { return 0; }
	int getMinCameraSetting(int mode) { return 0; }
	int getMaxCameraSetting(int mode) { return 0; }
	int getCameraSetting(int mode) { return 0; }
	bool setCameraSetting(int mode, int value) { return false; }
	bool setCameraSettingAuto(int mode, bool flag) { return false; }

	void control(int key);
	void showSettingsDialog();

private:

	char xml_config[255];
	DSVL_VideoSource *dsvl_vs;
	MemoryBufferHandle g_mbHandle;
	LONGLONG g_Timestamp;
	unsigned char *pbuffer;
};

#endif
