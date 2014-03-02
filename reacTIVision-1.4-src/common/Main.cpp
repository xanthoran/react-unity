/*  reacTIVision fiducial tracking framework
    Main.cpp
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

#include "PortVideoSDL.h"
#include "FrameEqualizer.h"
#include "FrameThresholder.h"
#include "FidtrackFinder.h"
#include "FidtrackFinderClassic.h"
#include "DtouchFinder.h"
#include "CalibrationEngine.h"

#ifdef LINUX
#include <signal.h>
#endif

PortVideoSDL *engine;

static void terminate (int param)
{
	printf("terminating reacTIVision ...\n");
	if (engine!=NULL) engine->stop();
}

struct reactivision_settings {
	char file[1024];
	int port;
	char host[1024];
	char tree_config[1024];
	char grid_config[1024];
	char midi_config[1024];
	char camera_config[1024];
	bool invert_x;
	bool invert_y;
	bool invert_a;
	bool midi;
	bool amoeba;
	bool classic;
	bool dtouch;
	bool background;
	int finger_size;
	int finger_sensitivity;	
	int gradient_gate;	
	int display_mode;
};

void printUsage() {
	std::cout << std::endl;
	std::cout << "usage: reacTIVision -c [config_file]" << std::endl;
	std::cout << "the default configuration file is reacTIVision.xml" << std::endl;
	std::cout << std::endl;
}

void readSettings(reactivision_settings *config) {
	
	config->port = 3333;
	sprintf(config->host,"localhost");
	sprintf(config->tree_config,"none");
	sprintf(config->grid_config,"none");
	sprintf(config->midi_config,"none");
	sprintf(config->camera_config,"none");
	config->invert_x = false;
	config->invert_y = false;
	config->invert_a = false;
	config->midi = false;
	config->amoeba = true;
	config->classic = false;
	config->dtouch = false;
	config->background = false;
	config->finger_size = 0;
	config->finger_sensitivity = 100;	
	config->gradient_gate = 32;	
	config->display_mode = 2;
	
	if (strcmp( config->file, "none" ) == 0) {
#ifdef __APPLE__
	char app_path[1024];
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef mainBundleURL = CFBundleCopyBundleURL( mainBundle);
	CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
	CFStringGetCString( cfStringRef, app_path, 1024, kCFStringEncodingASCII);	
	CFRelease( mainBundleURL);
	CFRelease( cfStringRef);
	sprintf(config->file,"%s/Contents/Resources/reacTIVision.xml",app_path);
#elif !defined WIN32
        if (access ("./reacTIVision.xml", F_OK )==0) sprintf(config->file,"./reacTIVision.xml");
        else if (access ("/usr/share/reacTIVision/reacTIVision.xml", F_OK )==0) sprintf(config->file,"/usr/share/reacTIVision/reacTIVision.xml");
        else if (access ("/usr/local/share/reacTIVision/reacTIVision.xml", F_OK )==0) sprintf(config->file,"/usr/local/share/reacTIVision/reacTIVision.xml");
        else if (access ("/opt/share/reacTIVision/reacTIVision.xml", F_OK )==0) sprintf(config->file,"/opt/share/reacTIVision/reacTIVision.xml");
#else
        sprintf(config->file,"./reacTIVision.xml");
#endif
	}

	TiXml::TiXmlDocument xml_settings( config->file );
	xml_settings.LoadFile();
	if( xml_settings.Error() )
	{
		std::cout << "Error loading configuration file: " << config->file << std::endl;
		return;
	}
	
	TiXml::TiXmlHandle docHandle( &xml_settings );
	TiXml::TiXmlHandle config_root = docHandle.FirstChild("reactivision");

	TiXml::TiXmlElement* tuio_element = config_root.FirstChild("tuio").Element();
	if( tuio_element!=NULL )
	{
		if(tuio_element->Attribute("host")!=NULL) sprintf(config->host,"%s",tuio_element->Attribute("host"));
		if(tuio_element->Attribute("port")!=NULL) config->port = atoi(tuio_element->Attribute("port"));
	}

	TiXml::TiXmlElement* camera_element = config_root.FirstChild("camera").Element();
	if( camera_element!=NULL )
	{
		if(camera_element->Attribute("config")!=NULL) sprintf(config->camera_config,"%s",camera_element->Attribute("config"));
	}
	
	TiXml::TiXmlElement* midi_element = config_root.FirstChild("midi").Element();
	if( midi_element!=NULL )
	{
		if(midi_element->Attribute("config")!=NULL) {
			sprintf(config->midi_config,"%s",midi_element->Attribute("config"));
			config->midi=true;
		}
	}

	TiXml::TiXmlElement* finger_element = config_root.FirstChild("finger").Element();
	if( finger_element!=NULL )
	{
		if(finger_element->Attribute("size")!=NULL) config->finger_size = atoi(finger_element->Attribute("size"));
		if(finger_element->Attribute("sensitivity")!=NULL) config->finger_sensitivity = atoi(finger_element->Attribute("sensitivity"));
	}
	
	TiXml::TiXmlElement* image_element = config_root.FirstChild("image").Element();
	if( image_element!=NULL )
	{
		if(image_element->Attribute("gradient")!=NULL) config->gradient_gate = atoi(image_element->Attribute("gradient"));
		if(image_element->Attribute("display")!=NULL)  {
			if ( strcmp( image_element->Attribute("display"), "none" ) == 0 ) config->display_mode = 0;
			else if ( strcmp( image_element->Attribute("display"), "src" ) == 0 )  config->display_mode = 1;
			else if ( strcmp( image_element->Attribute("display"), "dest" ) == 0 )  config->display_mode = 2;
		}
		if(image_element->Attribute("equalize")!=NULL) {
			if ((strcmp( image_element->Attribute("equalize"), "true" ) == 0) ||  atoi(image_element->Attribute("equalize"))==1) config->background = true;
		}
	}

	TiXml::TiXmlElement* fiducial_element = config_root.FirstChild("fiducial").Element();
	if( fiducial_element!=NULL )
	{
		if(fiducial_element->Attribute("engine")!=NULL)  {
			if ( strcmp( fiducial_element->Attribute("engine"), "amoeba" ) == 0 ) config->amoeba = true;
			else if ( strcmp( fiducial_element->Attribute("engine"), "classic" ) == 0 ) { config->classic = true; config->amoeba = false; }
			else if ( strcmp( fiducial_element->Attribute("engine"), "dtouch" ) == 0 )  { config->dtouch = true;  config->amoeba = false; }
		}
		if(fiducial_element->Attribute("tree")!=NULL) sprintf(config->tree_config,"%s",fiducial_element->Attribute("tree"));
	}

	TiXml::TiXmlElement* calibration_element = config_root.FirstChild("calibration").Element();
	if( calibration_element!=NULL )
	{
		if(calibration_element->Attribute("invert")!=NULL)  {
			if (strstr(calibration_element->Attribute("invert"),"x")>0) config->invert_x = true;
			if (strstr(calibration_element->Attribute("invert"),"y")>0) config->invert_y = true; 
			if (strstr(calibration_element->Attribute("invert"),"a")>0) config->invert_a = true; 			
		}
		if(calibration_element->Attribute("grid")!=NULL) sprintf(config->grid_config,"%s",calibration_element->Attribute("grid"));
	}
	
}


void writeSettings(reactivision_settings *config) {
	
	TiXml::TiXmlDocument xml_settings( config->file );
	xml_settings.LoadFile();
	if( xml_settings.Error() )
	{
		std::cout << "Error loading configuration file: " << config->file << std::endl;
		return;
	}
	
	char config_value[64];
	
	TiXml::TiXmlHandle docHandle( &xml_settings );
	TiXml::TiXmlHandle config_root = docHandle.FirstChild("reactivision");
	
	TiXml::TiXmlElement* tuio_element = config_root.FirstChild("tuio").Element();
	if( tuio_element!=NULL )
	{
		if(tuio_element->Attribute("host")!=NULL) tuio_element->SetAttribute("host",config->host);
		if(tuio_element->Attribute("port")!=NULL) {
			sprintf(config_value,"%d",config->port);
			tuio_element->SetAttribute("port",config_value);
		}
	}
	
	TiXml::TiXmlElement* camera_element = config_root.FirstChild("camera").Element();
	if( camera_element!=NULL )
	{
		if(camera_element->Attribute("config")!=NULL) camera_element->SetAttribute("config",config->camera_config);
	}
	
	TiXml::TiXmlElement* midi_element = config_root.FirstChild("midi").Element();
	if( midi_element!=NULL )
	{
		if(midi_element->Attribute("config")!=NULL) midi_element->SetAttribute("config",config->midi_config);
	}
	
	TiXml::TiXmlElement* finger_element = config_root.FirstChild("finger").Element();
	if( finger_element!=NULL )
	{
		if(finger_element->Attribute("size")!=NULL) {
			sprintf(config_value,"%d",config->finger_size);
			finger_element->SetAttribute("size",config_value);
		}
		if(finger_element->Attribute("sensitivity")!=NULL) {
			sprintf(config_value,"%d",config->finger_sensitivity);
			finger_element->SetAttribute("sensitivity",config_value);
		}
	}
	
	TiXml::TiXmlElement* image_element = config_root.FirstChild("image").Element();
	if( image_element!=NULL )
	{
		if(image_element->Attribute("gradient")!=NULL) {
			sprintf(config_value,"%d",config->gradient_gate);
			image_element->SetAttribute("gradient",config_value);
		}
		if(image_element->Attribute("display")!=NULL)  {
			if (config->display_mode == 0) image_element->SetAttribute("display","none");
			else if (config->display_mode == 1) image_element->SetAttribute("display","src");
			else if (config->display_mode == 2) image_element->SetAttribute("display","dest");  
		}
		if(image_element->Attribute("equalize")!=NULL) {
			if (config->background) image_element->SetAttribute("equalize","true");
			else image_element->SetAttribute("equalize","false");
		}
	}
	
	TiXml::TiXmlElement* fiducial_element = config_root.FirstChild("fiducial").Element();
	if( fiducial_element!=NULL )
	{
		if(fiducial_element->Attribute("engine")!=NULL)  {
			if (config->amoeba) fiducial_element->SetAttribute("engine", "amoeba"); 
			else if (config->classic) fiducial_element->SetAttribute("engine","classic"); 
			else if (config->dtouch) fiducial_element->SetAttribute("engine","dtouch"); 
		}
		if(fiducial_element->Attribute("tree")!=NULL) fiducial_element->SetAttribute("tree",config->tree_config);
	}
	
	TiXml::TiXmlElement* calibration_element = config_root.FirstChild("calibration").Element();
	if( calibration_element!=NULL )
	{
		sprintf(config_value," ");
		if(calibration_element->Attribute("invert")!=NULL)  {
			if (config->invert_x) strcat(config_value,"x");
			if (config->invert_y) strcat(config_value,"y");
			if (config->invert_a) strcat(config_value,"a");
			calibration_element->SetAttribute("invert",config_value);
		}
		if(calibration_element->Attribute("grid")!=NULL) calibration_element->SetAttribute("grid",config->grid_config);
	}
	
	xml_settings.SaveFile();
	if( xml_settings.Error() ) std::cout << "Error saving configuration file: "  << config->file << std::endl;
	
}


int main(int argc, char* argv[]) {
	
	reactivision_settings config;
	sprintf(config.file,"none");

	std::cout << "reacTIVision 1.4 (" << __DATE__ << ")"<< std::endl;

	if (argc>1) { 
		if (strcmp( argv[1], "-h" ) == 0 ) {
			printUsage();
			return 0;
		} else if( strcmp( argv[1], "-c" ) == 0 ) {
			if (argc==3) sprintf(config.file,"%s",argv[2]);
			else {
				printUsage();
				return 0;
			}
		} else if( strcmp( argv[1], "-l" ) == 0 ) {
			if (argc==3) {
				if ( strcmp( argv[2], "midi" ) == 0 ) { MidiServer::listDevices(); return 0; }
				else if ( strcmp( argv[2], "video" ) == 0 ) { CameraTool::listDevices(); return 0; }
			} else {
				printUsage();
				return 0;
			}
		} else {
			printUsage();
			return 0;
		}
	}

#ifndef WIN32
	signal(SIGINT,terminate);
	signal(SIGHUP,terminate);
	signal(SIGQUIT,terminate);
	signal(SIGTERM,terminate);
#endif

	readSettings(&config);
	
	engine = new PortVideoSDL("reacTIVision", config.background, config.camera_config);
	switch (config.display_mode) {
		case 0: engine->setDisplayMode(engine->NO_DISPLAY); break;
		case 1: engine->setDisplayMode(engine->SOURCE_DISPLAY); break;
		case 2: engine->setDisplayMode(engine->DEST_DISPLAY); break;
	}
		
	MessageServer  *server		= NULL;
	FrameProcessor *fiducialfinder	= NULL;
	FrameProcessor *thresholder	= NULL;
	FrameProcessor *equalizer	= NULL;
	FrameProcessor *calibrator	= NULL;

	if(config.midi) server = new MidiServer(config.midi_config);
	else server = new TuioServer(config.host,config.port);
	server->setInversion(config.invert_x, config.invert_y, config.invert_a);

	if (!config.dtouch) {
		equalizer = new FrameEqualizer();
		thresholder = new FrameThresholder(config.gradient_gate);
		if (config.amoeba) fiducialfinder = new FidtrackFinder(server, config.tree_config, config.grid_config, config.finger_size, config.finger_sensitivity);
		else if (config.classic) fiducialfinder = new FidtrackFinderClassic(server, config.grid_config);
		engine->addFrameProcessor(equalizer);
		engine->addFrameProcessor(thresholder);
	} else {
		fiducialfinder = new DtouchFinder(server, config.grid_config);
	} 
		
	engine->addFrameProcessor(fiducialfinder);
		
	calibrator = new CalibrationEngine(config.grid_config);
	//if (config.amoeba) ((CalibrationEngine*)calibrator)->activateAutoCalibration((FidtrackFinder*)fiducialfinder);
	engine->addFrameProcessor(calibrator);

	engine->run();
		
	config.display_mode = engine->getDisplayMode();
	
	engine->removeFrameProcessor(calibrator);
	delete calibrator;

	if (config.amoeba) {
		config.finger_size = ((FidtrackFinder*)fiducialfinder)->getFingerSize();
		config.finger_sensitivity = ((FidtrackFinder*)fiducialfinder)->getFingerSensitivity();
	}
	
	engine->removeFrameProcessor(fiducialfinder);
	if (!config.dtouch) {
		config.gradient_gate = ((FrameThresholder*)thresholder)->getGradientGate();
		engine->removeFrameProcessor(thresholder);
		config.background = ((FrameEqualizer*)equalizer)->getState();
		engine->removeFrameProcessor(equalizer);
		delete thresholder;
		delete equalizer;
	}

	config.invert_x = server->getInvertX();
	config.invert_y = server->getInvertY();
	config.invert_a = server->getInvertA();
	
	delete fiducialfinder;
	delete engine;
	delete server;
	
	writeSettings(&config);
	return 0;
}
