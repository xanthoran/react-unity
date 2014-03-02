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

#ifndef NDEBUG
#include "FileCamera.h"
#ifdef WIN32
#include <windows.h>
#endif 

FileCamera::FileCamera(const char* cfg): CameraEngine(cfg)
{
	cameraID = -1;
	
	buffer = NULL;

	sprintf(cameraName,"FileCamera");

	running=false;
}

FileCamera::~FileCamera()
{
	if (buffer!=NULL) delete []buffer;
}

bool FileCamera::findCamera() {
	readSettings();
	if (config.file==NULL) return false;
	FILE* imagefile=fopen(config.file, "r");
	if (imagefile==NULL) return false;
	fclose(imagefile);
	return true;
}

bool FileCamera::initCamera() {	

	int gray = 0;
	char header[32];
	char *param;
	
	if (config.file==NULL) return false;
	FILE*  imagefile=fopen(config.file, "r");
	if (imagefile==NULL) return false;

 	fgets(header,32,imagefile);
	while (strstr(header,"#")!=NULL) fgets(header,32,imagefile);
	if (strstr(header,"P5")==NULL) return false;
		
	fgets(header,32,imagefile);
	while (strstr(header,"#")!=NULL) fgets(header,32,imagefile);
	param = strtok(header," "); if (param) width = atoi(param);
	param = strtok(NULL," "); if (param) height =  atoi(param);
	param = strtok(NULL," "); if (param) gray = atoi(param);

	if (height==0) 	{ 
		fgets(header,32,imagefile);
		while (strstr(header,"#")!=NULL) fgets(header,32,imagefile);
		param = strtok(header," "); if (param) height = atoi(param);
		param = strtok(NULL," "); if (param) gray = atoi(param);
	}

	if (gray==0) {
		fgets(header,32,imagefile);
		while (strstr(header,"#")!=NULL) fgets(header,32,imagefile);
		param = strtok(header," "); if (param) gray = atoi(param);
	}

	if ((width==0) || (height==0) ) return false; 

	buffer = new unsigned char[width*height*bytes];		
	fread(buffer, bytes,  width*height, imagefile);
	fclose(imagefile);

	config.width = width;
	config.height = height;

	bytes = 1;
	colour = false;
	fps = 10;
	
	return true;
}

unsigned char* FileCamera::getFrame()
{
#ifdef WIN32
	Sleep(100);
#else 
	usleep( 100000 ); // simulate 10fps
#endif
	return buffer;	
}

bool FileCamera::startCamera()
{
	running = true;
	return true;
}

bool FileCamera::stopCamera()
{
	running = false;
	return true;
}

bool FileCamera::stillRunning() {
	return running;
}

bool FileCamera::resetCamera()
{
  return (stopCamera() && startCamera());
}

bool FileCamera::closeCamera()
{
	return true;
}

int FileCamera::getCameraSettingStep(int mode) { 
	return 0;
}

bool FileCamera::setCameraSettingAuto(int mode, bool flag) {
	return false;
}

bool FileCamera::setCameraSetting(int mode, int setting) {
	return false;
}

int FileCamera::getCameraSetting(int mode) {
	return 0;
}

int FileCamera::getMaxCameraSetting(int mode) {
	return 0;
}

int FileCamera::getMinCameraSetting(int mode) {
	return 0;
}

void FileCamera::showSettingsDialog() {
	return;
}
#endif

