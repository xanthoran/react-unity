/*  portVideo, a cross platform camera framework
    Copyright (C) 2005 Martin Kaltenbrunner <mkalten@iua.upf.es>

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

#ifndef V4LinuxCamera_H
#define V4LinuxCamera_H

#include "../common/CameraEngine.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/videodev.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <linux/videodev.h>

class V4LinuxCamera : public CameraEngine
{
public:
	V4LinuxCamera(const char* cfg);
	~V4LinuxCamera(void);

	bool findCamera();
	bool initCamera();
	bool startCamera();
	unsigned char* getFrame();
	bool stopCamera();
	bool stillRunning();
	bool resetCamera();
	bool closeCamera();
	void showSettingsDialog();
	
	int getCameraSettingStep(int mode) { return 0; };
 	bool setCameraSettingAuto(int mode, bool flag) { return false; }
 	bool setCameraSetting(int mode, int value) { return false; }
 	int getCameraSetting(int mode) { return 0; }
 	int getMaxCameraSetting(int mode) { return 0; }
 	int getMinCameraSetting(int mode) { return 0; }

private:
	video_capability v4l_caps;
	video_channel v4l_chan;
	video_mbuf v4l_buf;
	video_mmap v4l_mmap;
	video_window v4l_win;
	video_picture v4l_pict;
	unsigned char *camBuffer;
};

#endif
