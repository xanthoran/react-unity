/*  portVideo, a cross platform camera framework
    Copyright (C) 2005-2009 Martin Kaltenbrunner <mkalten@iua.upf.edu>

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

#include "DSlibCamera.h"

DSlibCamera::DSlibCamera(const char *cfg) : CameraEngine (cfg)
{
	CoInitialize(NULL);
	dsvl_vs = new DSVL_VideoSource();

	pbuffer = NULL;
	running = false;
}


DSlibCamera::~DSlibCamera()
{
	if (pbuffer!=NULL) delete []pbuffer;
}

bool DSlibCamera::findCamera() {
	readSettings();
	if (config.width<=0) config.height=SETTING_MAX;
	if (config.height<=0) config.height=SETTING_MAX;

	if (config.width!=SETTING_MAX && config.height!=SETTING_MAX) {
		if (config.device==SETTING_AUTO) {
			sprintf(xml_config, "<?xml version='1.0' encoding='UTF-8'?> <dsvl_input> <camera frame_width='%d' frame_height='%d' show_format_dialog='true'><pixel_format><RGB24/></pixel_format></camera></dsvl_input>", config.width, config.height, (int)config.fps);
		} else {
			sprintf(xml_config, "<?xml version='1.0' encoding='UTF-8'?> <dsvl_input> <camera frame_width='%d' frame_height='%d'><pixel_format><RGB24/></pixel_format></camera></dsvl_input>", config.width, config.height, (int)config.fps);
		}
	} else {
		if (config.device<0) {
			sprintf(xml_config, "<?xml version='1.0' encoding='UTF-8'?> <dsvl_input> <camera show_format_dialog='true'><pixel_format><RGB24/></pixel_format></camera></dsvl_input>");
		} else {
			sprintf(xml_config, "<?xml version='1.0' encoding='UTF-8'?> <dsvl_input> <camera> <pixel_format><RGB24/></pixel_format></camera></dsvl_input>");
		}
	}
	return true;
}

bool DSlibCamera::initCamera() {

	this->width = config.width;
	this->height = config.height;
	this->fps = config.fps;
	this->colour = config.color;
	this->bytes = (colour?3:1);

	LONG w,h;
	double f;

	if(FAILED(dsvl_vs->BuildGraphFromXMLString(xml_config))) return false;
	if(FAILED(dsvl_vs->GetCurrentMediaFormat(&w,&h,&f,NULL))) return false;

	if (w>0) this->width = w;
	if (h>0) this->height = h;
	if (f>0) this->fps = f;

	sprintf(cameraName,"WDM Camera");

	pbuffer = new unsigned char[this->width*this->height*bytes];
	return true;
}


bool DSlibCamera::startCamera()
{ 
	if(FAILED(dsvl_vs->EnableMemoryBuffer())) return false;
	if(FAILED(dsvl_vs->Run())) return false ;

	running = true;
	return true;
}

unsigned char* DSlibCamera::getFrame()
{
		DWORD wait_result = dsvl_vs->WaitForNextSample(/*INFINITE*/1000.0f/fps);

		if(wait_result != WAIT_OBJECT_0) 
		{
			//wait another 2 seconds
			wait_result = dsvl_vs->WaitForNextSample(2000);
			if(wait_result != WAIT_OBJECT_0) stopCamera();
		}

		if(wait_result == WAIT_OBJECT_0)
		{

			unsigned char r,g,b;
			int dwidth = 2*width;

			dsvl_vs->CheckoutMemoryBuffer(&g_mbHandle, &buffer);
			g_Timestamp = dsvl_vs->GetCurrentTimestamp();
			switch (colour) {
				case false: {
					unsigned char *src = (unsigned char*)buffer;
					unsigned char *dest = pbuffer;
					dest+=width*height-width;

					for(int y=0;y<height;y++) {
						for(int x=0;x<width;x++) {
							//quicker
							/**dest++ = *src++;
							src++;
							src++;*/

							//better
							/*r = *src++;
							g = *src++;
							b = *src++;
							*dest++ = (unsigned char)(0.30 * r + 0.59 * g + 0.11 * b);*/

							//fastest and best
							r = *src++;
							g = *src++;
							b = *src++;
							*dest++ = hibyte(r * 77 + g * 151 + b * 28);
						}
						dest-=dwidth;
					}
					break;
				}
				case true: {
					return buffer;
					//memcpy(pbuffer,buffer,width*height*bytes);
					break;
				}
			}
			dsvl_vs->CheckinMemoryBuffer(g_mbHandle);
			return pbuffer;
		} 

		return NULL;
}

bool DSlibCamera::stopCamera()
{
	if(!running) return false;
	dsvl_vs->Stop();
	running = false;
	return true;
}
bool DSlibCamera::stillRunning() {
	return running;
}

bool DSlibCamera::resetCamera()
{
  return (stopCamera() && startCamera());

}

bool DSlibCamera::closeCamera()
{
	if (!dsvl_vs->IsGraphInitialized()) return false;
	if(FAILED(dsvl_vs->DisableMemoryBuffer())) return false;
	delete dsvl_vs;
	return true;
}

void DSlibCamera::control(int key) {
	 return;
}

void DSlibCamera::showSettingsDialog() {

	dsvl_vs->ShowFilterProperties();
}
