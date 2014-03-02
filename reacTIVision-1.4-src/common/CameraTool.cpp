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

#include "CameraTool.h"

void CameraTool::listDevices() {
	std::cout << "video device selection will be available in a later reacTIVision version" << std::endl;
}
	
CameraEngine* CameraTool::findCamera(const char* config_file) {

	CameraEngine* camera = NULL;
	
	#ifndef NDEBUG
	camera = new FileCamera(config_file);
	if( !camera->findCamera() ) delete camera;
	else return camera;

	camera = new FolderCamera(config_file);
	if( !camera->findCamera() ) delete camera;
	else return camera;
	#endif
	
	#ifdef WIN32
	camera = new DSlibCamera(config_file);
	if( !camera->findCamera() ) delete camera;
	else return camera;
	#endif

	#ifdef __APPLE__
	camera = new DC1394Camera(config_file);
	if( !camera->findCamera() ) delete camera;
	else return camera;

	#if !defined (__x86_64__) && !defined (__ppc64__)
	camera = new MacVdigCamera(config_file);
	if( !camera->findCamera()) delete camera;
	else return camera;
	#endif
	#endif		
		
	#ifdef LINUX
	camera = new DC1394Camera(config_file);
	if( !camera->findCamera() ) delete camera;
	else return camera;
	
	camera = new V4Linux2Camera(config_file);
	if( !camera->findCamera()) delete camera;
	else return camera;

	camera = new V4LinuxCamera(config_file);
	if( !camera->findCamera()) delete camera;
	else return camera;
	#endif
	
	return NULL;
}

