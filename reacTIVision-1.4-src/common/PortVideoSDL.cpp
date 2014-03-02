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

#include "PortVideoSDL.h"

// the thread function which contantly retrieves the latest frame
int getFrameFromCamera(void *obj) {

		PortVideoSDL *engine = (PortVideoSDL *)obj;
		
		unsigned char *cameraBuffer = NULL;
		unsigned char *cameraWriteBuffer = NULL;
		
		while(engine->running_) {
			if(!engine->pause_) {
				//long start_time = PortVideoSDL::currentTime();
				cameraBuffer = engine->camera_->getFrame();
				if (cameraBuffer!=NULL) {
					cameraWriteBuffer = engine->ringBuffer->getNextBufferToWrite();
					if (cameraWriteBuffer!=NULL) {
						memcpy(cameraWriteBuffer,cameraBuffer,engine->ringBuffer->size());
						engine->framenumber_++;
						engine->ringBuffer->writeFinished();
						//long driver_time = PortVideoSDL::currentTime() - start_time;
						//std::cout << "camera latency: " << (driver_time/100)/10.0f << "ms" << std::endl;
					}
					SDL_Delay(1);
				} else {
					if (!engine->camera_->stillRunning()) {
						engine->running_=false;
						engine->error_=true;
					} else SDL_Delay(1);
				}
			} else SDL_Delay(5);
		}
		return(0);
}

#ifndef NDEBUG
void PortVideoSDL::saveBuffer(unsigned char* buffer) {

	int zerosize = 16-(int)floor(log10((float)framenumber_));
	if (zerosize<0) zerosize = 0;
	char zero[zerosize+1];
	zero[zerosize]=0;
	for (int i=0;i<(zerosize);i++) zero[i]=48;

	char fileName[256];
#ifdef WIN32
	sprintf(fileName,".\recording\%s%ld.pgm",zero,framenumber_);
#else
	sprintf(fileName,"./recording/%s%ld.pgm",zero,framenumber_);
#endif
	FILE*  imagefile=fopen(fileName, "w");
	fprintf(imagefile,"P5\n%u %u 255\n", width_, height_);
	fwrite((const char *)buffer, 1,  width_*height_, imagefile);
	fclose(imagefile);
}
#endif

void PortVideoSDL::stop() {
	running_=false;
	error_=false;
}

// the principal program sequence
void PortVideoSDL::run() {

	if( !setupCamera() ) {
		if( !setupWindow() ) return;
		showError("No camera found!");
		teardownWindow();
		return;
	}

	if( !setupWindow() ) return;

	allocateBuffers();
	initFrameProcessors();

	bool success = camera_->startCamera();

	if( success ){
		SDL_FillRect(window_,0,0);
		SDL_Flip(window_);
	
		// add the help message from all FrameProcessors
		for (frame = processorList.begin(); frame!=processorList.end(); frame++) {
                        std::vector<std::string> processor_text = (*frame)->getOptions();
			if (processor_text.size()>0) help_text.push_back("");
			for(std::vector<std::string>::iterator processor_line = processor_text.begin(); processor_line!=processor_text.end(); processor_line++) {
				help_text.push_back(*processor_line);
			} 
		}
		
		//print the help message
		for(std::vector<std::string>::iterator help_line = help_text.begin(); help_line!=help_text.end(); help_line++) {
			std::cout << *help_line << std::endl;
		} std::cout << std::endl;

		running_=true;
		cameraThread = SDL_CreateThread(getFrameFromCamera,this);
		mainLoop();
		
		SDL_KillThread(cameraThread);
		teardownCamera();
	} else {
		showError("Could not start camera!");
	}	

	teardownWindow();
	freeBuffers();
	
}

void PortVideoSDL::showError(const char* error)
{
	SDL_FillRect(window_,0,0);
	SDL_Rect *image_rect = new SDL_Rect();
	image_rect->x = (width_-camera_rect.w)/2-47;
	image_rect->y = (height_-camera_rect.h)/2-39;
	image_rect->w = camera_rect.w;
	image_rect->h = camera_rect.h;
	SDL_BlitSurface(getCamera(), &camera_rect, window_, image_rect);
	delete image_rect;
	
	std::string error_message = "Press any key to exit "+app_name_+" ...";
	FontTool::drawText((width_- FontTool::getTextWidth(error))/2,height_/2+60,error,window_);
	FontTool::drawText((width_- FontTool::getTextWidth(error_message.c_str()))/2,height_/2+80,error_message.c_str(),window_);
	SDL_Flip(window_);

	error_=true;
	while(error_) {
		process_events();
		SDL_Delay(1);
	}

}

void PortVideoSDL::drawHelp()
{
	int y = FontTool::getFontHeight()-3;

	for(std::vector<std::string>::iterator help_line = help_text.begin(); help_line!=help_text.end(); help_line++) {
		if (strcmp(help_line->c_str(), "") == 0) y+=5;
		else {
			FontTool::drawText(FontTool::getFontHeight(),y,help_line->c_str(),displayImage_);
			y+=FontTool::getFontHeight()-3;
		}
	}
}

// does what its name suggests
void PortVideoSDL::mainLoop()
{
	unsigned char* cameraReadBuffer = NULL;

	while(running_) {
		
		// do nothing if paused
		if (pause_){
			process_events();
			SDL_Delay(50);
			continue;
		}


		long start_time = currentTime();
		cameraReadBuffer = ringBuffer->getNextBufferToRead();
		// loop until we get access to a frame
		while (cameraReadBuffer==NULL) {
			cameraReadBuffer = ringBuffer->getNextBufferToRead();
			//if (cameraReadBuffer!=NULL) break;
			SDL_Delay(1);
			process_events();
			if(!running_) { 
				endLoop(); 
				return;
			}
		}
		long camera_time = currentTime();

		
		//memcpy(sourceBuffer_,cameraReadBuffer,ringBuffer->size());
		//ringBuffer->readFinished();
		
		// try again if we can get a more recent frame
		/*do {
			memcpy(sourceBuffer_,cameraReadBuffer,ringBuffer->size());
			ringBuffer->readFinished();
			
			cameraReadBuffer = ringBuffer->getNextBufferToRead();
		} while( cameraReadBuffer != NULL );*/
		
		// do the actual image processing job
		for (frame = processorList.begin(); frame!=processorList.end(); frame++)
			(*frame)->process(cameraReadBuffer,destBuffer_,displayImage_);
		long processing_time = currentTime();
		
		// update display
		switch( displayMode_ ) {
			case NO_DISPLAY:
				break;
			case SOURCE_DISPLAY: {
				memcpy(sourceBuffer_,cameraReadBuffer,ringBuffer->size());
				SDL_BlitSurface(sourceImage_, NULL, window_, NULL);
				if (help_) drawHelp();
				camera_->drawGUI(displayImage_);
				SDL_BlitSurface(displayImage_, NULL, window_, NULL);
				SDL_FillRect(displayImage_, NULL, 0 );
				SDL_Flip(window_);
				break;
			}			
			case DEST_DISPLAY: {
				SDL_BlitSurface(destImage_, NULL, window_, NULL);
				if (help_) drawHelp();
				camera_->drawGUI(displayImage_);
				SDL_BlitSurface(displayImage_, NULL, window_, NULL);
				SDL_FillRect(displayImage_, NULL, 0 );
				SDL_Flip(window_);
				break;
			}
		}

#ifndef NDEBUG
		if (recording_) {
			if (displayMode_!=SOURCE_DISPLAY) memcpy(sourceBuffer_,cameraReadBuffer,ringBuffer->size());
			saveBuffer(sourceBuffer_);
		}
#endif
		
		ringBuffer->readFinished();
		if (!recording_) frameStatistics(camera_time-start_time,processing_time-camera_time, currentTime()-start_time);
		process_events();
	}
	
	endLoop(); 
}

void PortVideoSDL::endLoop() {
	// finish all FrameProcessors
	for (frame = processorList.begin(); frame!=processorList.end(); frame++)
		(*frame)->finish();

	if(error_) showError("Camera disconnected!");
}

void PortVideoSDL::frameStatistics(long cameraTime, long processingTime, long totalTime) {

	frames_++;
	cameraTime_+=cameraTime;
	processingTime_+=processingTime;
	totalTime_+=totalTime;

	time_t currentTime;
	time(&currentTime);
	long diffTime = (long)( currentTime - lastTime_ );
	
	if (diffTime >= 1) {
		current_fps = (int)floor( (frames_ / diffTime) + 0.5f );
		char caption[24] = "";
		sprintf(caption,"%s - %d FPS",app_name_.c_str(),current_fps);
		if (!calibrate_) SDL_WM_SetCaption(caption, NULL );
		/*std::cout 	<< floor((cameraTime_/frames_)/1000.0f) << " " 
				<< floor((processingTime_/frames_)/1000.0f) << " " 
				<< floor((totalTime_/frames_)/1000.0f) << std::endl;*/
		//std::cout << "frame latency: " << ((processingTime_/frames_)/100)/10.0f << " ms" << std::endl;

		frames_ = 0;
		cameraTime_ = processingTime_ = totalTime_ = 0.0f;
		//std::cout << current_fps << std::endl;
		lastTime_ = (long)currentTime;

		if (!calibrated_) {
			calibrated_ = true;
			for (frame = processorList.begin(); frame!=processorList.end(); frame++)
                 		(*frame)->toggleFlag(' ');
		}
    	}
}

bool PortVideoSDL::setupWindow() {

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		printf("SDL could not be initialized: %s\n", SDL_GetError());
		return false;
	}
	
	window_ = SDL_SetVideoMode(width_, height_, 32, SDL_HWSURFACE);
	if ( window_ == NULL ) {
		printf("Could not open window: %s\n", SDL_GetError());
		SDL_Quit();
		return false;
	}

	iconImage_ = getIcon();
	#ifndef __APPLE__
	SDL_WM_SetIcon(iconImage_, getMask());
	#endif

	FontTool::init();
	SDL_EnableKeyRepeat(200, 10);
	SDL_WM_SetCaption(app_name_.c_str(), NULL);

	return true;
}

void PortVideoSDL::teardownWindow()
{
	FontTool::close();
	SDL_Quit();
}

void PortVideoSDL::process_events()
{
    SDL_Event event;
    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
		case SDL_KEYDOWN:
			if (error_) { error_ = false; return; }
			//printf("%d\n",event.key.keysym.sym);
			if( event.key.keysym.sym == SDLK_n ){
				displayMode_ = NO_DISPLAY;
				// turn the display black
				SDL_FillRect(window_,0,0);
				SDL_Flip(window_);
			} else if( event.key.keysym.sym == SDLK_s ){
				SDL_FillRect(displayImage_, NULL, 0 );
				displayMode_ = SOURCE_DISPLAY;
			} else if( event.key.keysym.sym == SDLK_t ){
				SDL_FillRect(displayImage_, NULL, 0 );
				displayMode_ = DEST_DISPLAY;
			} else if( event.key.keysym.sym == SDLK_o ){
				camera_->showSettingsDialog(); 
			} else if( event.key.keysym.sym == SDLK_v ){
				if (verbose_) {
					verbose_=false;
				} else {
					help_ = false;
					verbose_=true;
				}
			} 
#ifndef NDEBUG
			  else if( event.key.keysym.sym == SDLK_m ){
					if (recording_) {
						recording_ = false;
						char caption[24] = "";
						sprintf(caption,"%s - %d FPS",app_name_.c_str(),current_fps);
						SDL_WM_SetCaption( caption, NULL );
					} else {
						struct stat info;
#ifdef WIN32			
						if (stat(".\recording",&info)!=0) {
							std::string dir(".\recording");
							LPSECURITY_ATTRIBUTES attr = NULL;
							CreateDirectory(dir.c_str(),attr);
						}
#else
						if (stat("./recording",&info)!=0) mkdir("./recording",0777);
#endif
						recording_ = true;
						std::string caption = app_name_ + " - recording";
						SDL_WM_SetCaption( caption.c_str(), NULL );
					}
			} else if( event.key.keysym.sym == SDLK_b ){
				struct stat info;
#ifdef WIN32			
				if (stat(".\recording",&info)!=0) {
					std::string dir(".\recording");
					LPSECURITY_ATTRIBUTES attr = NULL;
					CreateDirectory(dir.c_str(),attr);
				}
#else
				if (stat("./recording",&info)!=0) mkdir("./recording",0777);
#endif
				if (displayMode_==DEST_DISPLAY) saveBuffer(destBuffer_);
				else saveBuffer(sourceBuffer_);
			} 
#endif
			else if( event.key.keysym.sym == SDLK_p ) {
				if (pause_) {
					pause_=false;
					char caption[24] = "";
					sprintf(caption,"%s - %d FPS",app_name_.c_str(),current_fps);
					SDL_WM_SetCaption( caption, NULL );
					
				} else {
					pause_=true;
					std::string caption = app_name_ + " - paused";
					SDL_WM_SetCaption( caption.c_str(), NULL );
					// turn the display black
					SDL_FillRect(window_,0,0);
					SDL_Flip(window_);
				}				
			} else if( event.key.keysym.sym == SDLK_c ) {
				if (calibrate_) {
					calibrate_=false;
					char caption[24] = "";
					sprintf(caption,"%s - %d FPS",app_name_.c_str(),current_fps);
					SDL_WM_SetCaption( caption, NULL );
					
				} else {
					calibrate_=true;
					std::string caption = app_name_ + " - calibration";
					SDL_WM_SetCaption( caption.c_str(), NULL );
				}				
			} else if( event.key.keysym.sym == SDLK_h ){
				help_=!help_;
				verbose_=false;
			} else if( event.key.keysym.sym == SDLK_ESCAPE ){
				running_=false;
				return;
			}

			for (frame = processorList.begin(); frame!=processorList.end(); frame++)
				(*frame)->toggleFlag(event.key.keysym.sym);
			camera_->control(event.key.keysym.sym);

			break;
		case SDL_QUIT:
			running_ = false;
			error_ = false;
			break;
        }
    }
}

bool PortVideoSDL::setupCamera() {

	camera_ = CameraTool::findCamera(camera_config);	
	if (camera_ == NULL) return false;
	
	bool success = camera_->initCamera();
	
	if(success) {
		width_ = camera_->getWidth();
		height_ = camera_->getHeight();
		fps_ = camera_->getFps();
					
		printf("camera: %s\n",camera_->getName());
		if (fps_>0) printf("format: %dx%d, %dfps\n\n",width_,height_,fps_);
		else printf("format: %dx%d\n\n",width_,height_);
	
		return true;
	} else {
		printf("could not initialize camera\n");
		camera_->closeCamera();
		delete camera_;
		return false;
	}
}

void PortVideoSDL::teardownCamera()
{
	camera_->stopCamera();
	camera_->closeCamera();
	delete camera_;
}

void PortVideoSDL::allocateBuffers()
{
	bytesPerSourcePixel_ = sourceDepth_/8;	
	bytesPerDestPixel_ = destDepth_/8;
	sourceBuffer_ = new unsigned char[width_*height_*bytesPerSourcePixel_];
	destBuffer_ = new unsigned char[width_*height_*bytesPerDestPixel_];
	displayBuffer_ = new unsigned char[width_*height_*3];
	cameraBuffer_ = NULL;
		
	sourceImage_ = SDL_CreateRGBSurfaceFrom(sourceBuffer_, width_, height_, sourceDepth_, width_*bytesPerSourcePixel_, 0 , 0, 0, 0);
	if (sourceDepth_==8)
		SDL_SetPalette(sourceImage_, SDL_LOGPAL|SDL_PHYSPAL, palette_, 0, 256 );
	//SDL_DisplayFormat(sourceImage_);

	destImage_ = SDL_CreateRGBSurfaceFrom(destBuffer_, width_, height_, destDepth_, width_*bytesPerDestPixel_, 0 , 0, 0, 0);
	if (destDepth_==8)
		SDL_SetPalette(destImage_, SDL_LOGPAL|SDL_PHYSPAL, palette_, 0, 256 );
	//SDL_DisplayFormat(destImage_);

	displayImage_ = SDL_CreateRGBSurfaceFrom(displayBuffer_, width_, height_, 24, width_*3, 0, 0, 0, 0);
	SDL_SetColorKey(displayImage_, SDL_SRCCOLORKEY, SDL_MapRGB(displayImage_->format, 0, 0, 0));
	//SDL_DisplayFormat(displayImage_);
	
	ringBuffer = new RingBuffer(width_*height_*bytesPerSourcePixel_);
}

void PortVideoSDL::freeBuffers()
{
	SDL_FreeSurface(sourceImage_);
	SDL_FreeSurface(destImage_);
	SDL_FreeSurface(displayImage_);
	SDL_FreeSurface(iconImage_);
	
	delete [] sourceBuffer_;
	delete [] destBuffer_;
	delete [] displayBuffer_;	
	delete ringBuffer;
}

void PortVideoSDL::addFrameProcessor(FrameProcessor *fp) {

	processorList.push_back(fp);
	fp->addMessageListener(this);
}


void PortVideoSDL::removeFrameProcessor(FrameProcessor *fp) {
	frame = std::find( processorList.begin(), processorList.end(), fp );
	if( frame != processorList.end() ) {
		processorList.erase( frame );
		fp->removeMessageListener(this);
	}
}

void PortVideoSDL::setMessage(std::string message)
{
	if (verbose_) {
		std::cout << message << std::endl;
		std::cout.flush();
	}
}

void PortVideoSDL::displayMessage(const char *message)
{
	SDL_FillRect(window_,0,0);
	FontTool::drawText((width_- FontTool::getTextWidth(message))/2,height_/2,message,window_);
	SDL_Flip(window_);
}

void PortVideoSDL::setDisplayMode(DisplayMode mode) {
	if ((window_!=NULL) && (mode==NO_DISPLAY)) {
		SDL_FillRect(window_,0,0);
		SDL_Flip(window_);
	}
	displayMode_ = mode;
}

void PortVideoSDL::initFrameProcessors() {
	for (frame = processorList.begin(); frame!=processorList.end(); ) {
		bool success = (*frame)->init(width_ , height_, bytesPerSourcePixel_, bytesPerDestPixel_);
		if(!success) {	
			processorList.erase( frame );
			printf("removed frame processor\n");
		} else frame++;
	}
}

unsigned int PortVideoSDL::current_fps = 0;
bool PortVideoSDL::display_lock = false;

PortVideoSDL::PortVideoSDL(const char* name, bool background, const char* cfg)
	: error_( false )
	, pause_( false )
	, calibrate_( false )
	, help_( false )
	, framenumber_( 0 )
	, frames_( 0 )
	, recording_( false )
	, width_( WIDTH )
	, height_( HEIGHT )
	, displayMode_( DEST_DISPLAY )
{
	cameraTime_ = processingTime_ = totalTime_ = 0.0f;
	calibrated_ = !background;
	camera_config = cfg;
	
	time_t start_time;
	time(&start_time);
	lastTime_ = (long)start_time;

	app_name_ = std::string(name);
	sourceDepth_ = 8;
	destDepth_   = 8;

	window_ = NULL;
	sourceBuffer_ = NULL;
	destBuffer_ = NULL;

	for(int i=0;i<256;i++){
		palette_[i].r=i;
		palette_[i].g=i;
		palette_[i].b=i;
	}
		
	help_text.push_back("display:");
 	help_text.push_back("   n - no image");
	help_text.push_back("   s - source image");
	help_text.push_back("   t - target image");	
	help_text.push_back("   h - toggle help text");
	help_text.push_back("");
	help_text.push_back("commands:");
	help_text.push_back("   ESC - quit " + app_name_);
	help_text.push_back("   v - verbose output");
	help_text.push_back("   o - camera options");
	help_text.push_back("   p - pause processing");
	#ifndef NDEBUG
	help_text.push_back("debug options:");
	help_text.push_back("   b - save buffer as PGM");
	help_text.push_back("   m - save buffer sequence");
	#endif

}



