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

#include "V4LinuxCamera.h"

V4LinuxCamera::V4LinuxCamera(const char* cfg) : CameraEngine(cfg)
{
	cameraID = -1;

	buffer = NULL;
	running = false;
}

V4LinuxCamera::~V4LinuxCamera(void)
{
	if (buffer!=NULL) delete []buffer;
}

bool V4LinuxCamera::initCamera() {

	if (cameraID < 0) return false;

    	if (config.width==SETTING_MAX) width = 640;
    	if (config.height==SETTING_MAX) height = 480;
	if (config.fps == SETTING_MAX) fps = 30;

	if ((v4l_caps.type & VID_TYPE_CAPTURE) == 0) {
		printf("This device does not support memory capture.\n");
		return false;
	}
	
	if( ioctl(cameraID, VIDIOCGMBUF, &v4l_buf) < 0 ) {
		printf("Cannot get capture buffer properties: %s\n" , strerror(errno));
		return false;
	}


	if (ioctl(cameraID, VIDIOCGWIN, &v4l_win) < 0) {
		printf("Unable to query window settings: %s\n",  strerror(errno));
		return false;
	}

	v4l_win.width  = width;
	v4l_win.height = height;
	v4l_win.clipcount = 0;
	v4l_win.flags  = v4l_win.flags |= ( (int)fps << 16 );
	
	if (ioctl(cameraID, VIDIOCSWIN, &v4l_win) < 0) {
		printf("Cannot set window settings: %s\n", strerror(errno));
		return false;
	}

	if( ioctl(cameraID, VIDIOCGPICT, &v4l_pict) < 0 ) {
		printf("Unable to query picture properties: %s\n" , strerror(errno));
		return false;
	}

	v4l_pict.brightness = 32768;
	v4l_pict.contrast = 32768; 
	v4l_pict.hue = 32768;
	v4l_pict.colour = 32768;

	v4l_pict.palette = VIDEO_PALETTE_YUV420P;
	if( ioctl(cameraID, VIDIOCSPICT, &v4l_pict) < 0 ) {
		v4l_pict.palette = VIDEO_PALETTE_YUYV;
		if( ioctl(cameraID, VIDIOCSPICT, &v4l_pict) < 0 ) {
			v4l_pict.palette = VIDEO_PALETTE_UYVY;
			if( ioctl(cameraID, VIDIOCSPICT, &v4l_pict) < 0 ) {
				printf("Cannot set picture properties: %s\n" , strerror(errno));
				return false;
			}
		}
	}

	if( ioctl(cameraID, VIDIOCGPICT, &v4l_pict) < 0 ) {
		printf("Cannot get picture properties: %s\n" , strerror(errno));
		return false;
	}

	v4l_mmap.height = height;
	v4l_mmap.width  = width;
	v4l_mmap.format = v4l_pict.palette;
	v4l_mmap.frame = 0;

	buffer = new unsigned char[width*height*bytes];
	return true;
}

bool V4LinuxCamera::findCamera() {

	readSettings();
	if (config.device<0) config.device=0;

	// video4linux device name:
	char v4l_device[128];
	sprintf(v4l_device,"/dev/video%d",config.device);

	int fd = open(v4l_device, O_RDWR);
	if (fd < 0) {
		printf("no v4linux camera found at %s\n", v4l_device);
		return false;
	}

	if (ioctl(fd, VIDIOCGCAP, &v4l_caps) < 0) {
		sprintf(cameraName, "unknown camera");
	} else sprintf(cameraName, "%s", v4l_caps.name);

	cameraID = fd;
	return true;
}

bool V4LinuxCamera::startCamera() {
	camBuffer = (unsigned char *) mmap(0, v4l_buf.size, PROT_READ|PROT_WRITE, MAP_SHARED, cameraID, 0);
	running = true;
	return true;
}

unsigned char* V4LinuxCamera::getFrame()  {

	if (cameraID==-1) return NULL;

	int frame = 0;
	v4l_mmap.frame = frame;

	ioctl(cameraID, VIDIOCSYNC, &frame);
	if(ioctl(cameraID, VIDIOCMCAPTURE, &v4l_mmap)<0) {
		stopCamera();
		closeCamera();
		return NULL;
	}
	
	camBuffer = camBuffer + v4l_buf.offsets[frame];	
	
	if (camBuffer==NULL) return NULL;

	switch (colour) {
		case false:
			if (v4l_mmap.format==VIDEO_PALETTE_YUYV) yuyv2gray(this->width, this->height, camBuffer, buffer);
        		else if (v4l_mmap.format==VIDEO_PALETTE_UYVY) uyvy2gray(this->width, this->height, camBuffer, buffer);
        		else if (v4l_mmap.format==VIDEO_PALETTE_YUV420P) return camBuffer;
			return buffer;
			break;
		case true: 
			//ccvt_420p_bgr24(width, height, camBuffer, buffer);
			break;
	}
	 
	return buffer;
}

bool V4LinuxCamera::stopCamera() {
  running = false;
  return true;
}

bool V4LinuxCamera::stillRunning() {
 	return running;
}

bool V4LinuxCamera::resetCamera()
{
  return (stopCamera() && startCamera());
}


bool V4LinuxCamera::closeCamera() {

	if (cameraID >= 0) {
		close(cameraID);
		cameraID = -1;
		return true;
	} else return false;

}

void V4LinuxCamera::showSettingsDialog() {
	printf("not yet implemented\n");
}


