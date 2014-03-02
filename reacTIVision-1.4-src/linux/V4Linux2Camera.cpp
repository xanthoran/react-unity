/*  v4l2 camera engine
    Copyright (C) 2005-2009 Martin Kaltenbrunner <mkalten@iua.upf.edu>
    Copyright (C) 2007 Peter Eschler <peschler@gmail.com>
      
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

#include "V4Linux2Camera.h"

V4Linux2Camera::V4Linux2Camera(const char* cfg) : CameraEngine(cfg)
{
    cameraID = -1;
  
    buffer = NULL;
    running = false;    
    buffers_initialized = false;
}

V4Linux2Camera::~V4Linux2Camera(void)
{    
    if (buffer!=NULL) delete []buffer;
}

bool V4Linux2Camera::initCamera() {

    if (cameraID < 0) return false;

    if (width==SETTING_MAX) width = 640;
    if (height==SETTING_MAX) height = 480;
    
    if (!requestBuffers()) {
        printf("Error requesting buffers.\n");
        return false;	
    }
          
    if (!mapBuffers()) {
        printf("Unable to mmap buffers.\n");
        return false;
    }

    buffers_initialized = true;
    
    v4l2_form.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == ioctl (cameraID, VIDIOC_G_FMT, &v4l2_form)) {
        printf("Unable to get pixel format: %s\n" , strerror(errno));
        return false;
    }

    /*printf("initial pixelformat: ");
    if (v4l2_form.fmt.pix.pixelformat==V4L2_PIX_FMT_UYVY) printf("V4L2_PIX_FMT_UYVY\n");
    else if (v4l2_form.fmt.pix.pixelformat==V4L2_PIX_FMT_YUYV) printf("V4L2_PIX_FMT_YUYV\n");
    else if (v4l2_form.fmt.pix.pixelformat==V4L2_PIX_FMT_YUV420) printf("V4L2_PIX_FMT_YUV420 \n");
    else if (v4l2_form.fmt.pix.pixelformat==V4L2_PIX_FMT_YUV410) printf("V4L2_PIX_FMT_YUV410\n");
    else if (v4l2_form.fmt.pix.pixelformat==V4L2_PIX_FMT_YVU420) printf("V4L2_PIX_FMT_YVU420\n");
    else if (v4l2_form.fmt.pix.pixelformat==V4L2_PIX_FMT_YVU410) printf("V4L2_PIX_FMT_YVU410\n");
    else printf("other\n");*/

    // try to set the desired dimensions
    v4l2_form.fmt.pix.width  = width; 
    v4l2_form.fmt.pix.height = height;
	
   // check the default pixel formats
   if ((v4l2_form.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) && (v4l2_form.fmt.pix.pixelformat != V4L2_PIX_FMT_UYVY) && (v4l2_form.fmt.pix.pixelformat != V4L2_PIX_FMT_YUV420)) {

	v4l2_form.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	ioctl (cameraID, VIDIOC_S_FMT, &v4l2_form);
	ioctl (cameraID, VIDIOC_G_FMT, &v4l2_form);

		if (v4l2_form.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
			v4l2_form.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
			ioctl (cameraID, VIDIOC_S_FMT, &v4l2_form);
			ioctl (cameraID, VIDIOC_G_FMT, &v4l2_form);

			if (v4l2_form.fmt.pix.pixelformat != V4L2_PIX_FMT_UYVY) {
				v4l2_form.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
				ioctl (cameraID, VIDIOC_S_FMT, &v4l2_form);
				ioctl (cameraID, VIDIOC_G_FMT, &v4l2_form);
			}
		}
    }  else {
	   ioctl (cameraID, VIDIOC_S_FMT, &v4l2_form);
	   ioctl (cameraID, VIDIOC_G_FMT, &v4l2_form);
   }
	
    // use the settings we got from the driver            
    pixelformat = v4l2_form.fmt.pix.pixelformat;
    width = v4l2_form.fmt.pix.width;
    height = v4l2_form.fmt.pix.height;
    fps = 0;      

	if ((v4l2_form.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) && (v4l2_form.fmt.pix.pixelformat != V4L2_PIX_FMT_UYVY) && (v4l2_form.fmt.pix.pixelformat != V4L2_PIX_FMT_YUV420)) {
		printf("unsupported pixelformat: ");
		if (v4l2_form.fmt.pix.pixelformat==V4L2_PIX_FMT_YUV410) printf("V4L2_PIX_FMT_YUV410\n");
		else if (v4l2_form.fmt.pix.pixelformat==V4L2_PIX_FMT_YVU420) printf("V4L2_PIX_FMT_YVU420\n");
		else if (v4l2_form.fmt.pix.pixelformat==V4L2_PIX_FMT_YVU410) printf("V4L2_PIX_FMT_YVU410\n");
		else printf("other\n");
	}
         
    applyCameraSettings();
         
    buffer = new unsigned char[width*height*bytes];
    return true;
}


bool V4Linux2Camera::findCamera() {

    readSettings();
    if (config.device<0) config.device=0;

    char v4l2_device[128];
    sprintf(v4l2_device,"/dev/video%d",config.device);

    int fd = open(v4l2_device, O_RDWR);
    if (fd < 0) {
        printf("no v4linux2 camera found at %s\n", v4l2_device);
        return false;
    }

    if (ioctl(fd, VIDIOC_QUERYCAP, &v4l2_caps) < 0) {        
        sprintf(cameraName, "unknown camera");
    } else sprintf(cameraName, "%s", v4l2_caps.driver);

    if ((v4l2_caps.capabilities & V4L2_CAP_STREAMING) == 0) {
        printf("This device does not support memory streaming capture.\n");
	close(fd);
        return false;
    }

    cameraID = fd;
    return true;
}

bool V4Linux2Camera::startCamera() {
	  
    for(unsigned int i = 0; i < v4l2_reqbuffers.count; ++i) {
        memset(&v4l2_buf, 0, sizeof(v4l2_buf));
        v4l2_buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory      = V4L2_MEMORY_MMAP;
        v4l2_buf.index       = i;
        if (-1 == ioctl (cameraID, VIDIOC_QBUF, &v4l2_buf)) {
            printf("Error queuing buffer: %s\n",  strerror(errno));
            exit(0);            
        }
    }
  
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(cameraID, VIDIOC_STREAMON, &type) < 0) {
        printf("Cannot start camera: %s\n", strerror(errno));
        return false;
    }
    
    running = true;
    return true;
}

unsigned char* V4Linux2Camera::getFrame()  {

    if (cameraID==-1) return NULL;
    memset (&v4l2_buf, 0, sizeof (v4l2_buf));
    v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_buf.memory = V4L2_MEMORY_MMAP; 
  
    if (-1 == ioctl (cameraID, VIDIOC_DQBUF, &v4l2_buf)) {
	stopCamera();
	closeCamera();
    	return NULL;
    }
    cam_buffer = (unsigned char*)buffers[v4l2_buf.index].start;
    
    if(colour) {
        //ccvt_yuyv_rgb24(this->width, this->height, cam_buffer, buffer);
    } else {
	if (pixelformat==V4L2_PIX_FMT_YUYV) yuyv2gray(width, height, cam_buffer, buffer);
        else if (pixelformat==V4L2_PIX_FMT_UYVY) uyvy2gray(width, height, cam_buffer, buffer);
        else if (pixelformat==V4L2_PIX_FMT_YUV420) memcpy(buffer,cam_buffer,width*height);
	else return NULL;
    }

    if (-1 == ioctl (cameraID, VIDIOC_QBUF, &v4l2_buf)) {
        printf("cannot unqueue buffer: %s\n", strerror(errno));
        return NULL;
    }    
    
    return buffer;
}

bool V4Linux2Camera::stopCamera() {
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(cameraID, VIDIOC_STREAMOFF, &type) < 0) {
        printf("cannot stop camera: %s\n", strerror(errno));
        return false;
    }
  
    running = false;
    return true;
}

bool V4Linux2Camera::stillRunning() {
 	  return running;
}

bool V4Linux2Camera::resetCamera()
{
    return (stopCamera() && startCamera());
}


bool V4Linux2Camera::closeCamera() {

    if (cameraID >= 0) {
        if (buffers_initialized) unmapBuffers();
        close(cameraID);
        cameraID = -1;

        return true;
    } else return false;

}

bool V4Linux2Camera::requestBuffers() {
    // Request mmap buffers
    memset (&v4l2_reqbuffers, 0, sizeof (v4l2_reqbuffers));
    v4l2_reqbuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_reqbuffers.memory = V4L2_MEMORY_MMAP;
    v4l2_reqbuffers.count = nr_of_buffers;

    if (-1 == ioctl (cameraID, VIDIOC_REQBUFS, &v4l2_reqbuffers)) {
        if (errno == EINVAL)
            printf ("video capturing or mmap-streaming is not supported\n");
        else
            perror ("VIDIOC_REQBUFS");
                
        return false;
    }
    
    if (v4l2_reqbuffers.count < 1) {
        /* You may need to free the buffers here. */                
        printf("Error requesting buffers.\n");
        return false;
    }
    
    return true;
}

bool V4Linux2Camera::mapBuffers() {

    for(unsigned int i=0; i<v4l2_reqbuffers.count; i++) {
  
        memset (&v4l2_buf, 0, sizeof (v4l2_buf));
        v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory = V4L2_MEMORY_MMAP;
        v4l2_buf.index = i;
      
        if (-1 == ioctl (cameraID, VIDIOC_QUERYBUF, &v4l2_buf)) {
            printf("Unable to query buffer: %s\n",  strerror(errno));               
            return false;
        }
      
        buffers[i].length = v4l2_buf.length; /* remember for munmap() */   
        buffers[i].start = mmap (NULL, v4l2_buf.length,
              PROT_READ | PROT_WRITE, /* recommended */
              MAP_SHARED,             /* recommended */
              cameraID, v4l2_buf.m.offset);
      
        if (MAP_FAILED == buffers[i].start) {
            /* If you do not exit here you should unmap() and free()
            the buffers mapped so far. */
            printf("Unable to map buffer %d.\n",  i);
            return false;    
        }
    }
    
    return true;
}

bool V4Linux2Camera::unmapBuffers() {
    for(unsigned int i=0; i<v4l2_reqbuffers.count; i++) {
        if (-1 == munmap(buffers[i].start, buffers[i].length)) {
            printf("Error unmapping buffer %d.\n", i);
            return false;
        }
    }         
    return true;
}


bool V4Linux2Camera::setCameraSettingAuto(int mode, bool flag) {

	v4l2_ctrl.id = -1;
	v4l2_ctrl.value = flag;

	switch (mode) {
		case GAIN: v4l2_ctrl.id = V4L2_CID_AUTOGAIN; break;
#ifdef V4L2_CID_EXPOSURE_AUTO
		case EXPOSURE: v4l2_ctrl.id = V4L2_CID_EXPOSURE_AUTO; break;
#endif
	} 

	if (v4l2_ctrl.id<0) return false;

	if ((ioctl(cameraID, VIDIOC_S_CTRL, &v4l2_ctrl)) < 0) {
	    return false;
	} return true;

}


bool V4Linux2Camera::setCameraSetting(int mode, int setting) {

	int current_setting = getCameraSetting(mode);
	if (setting==current_setting) return true;
	setCameraSettingAuto(mode,false);

	v4l2_ctrl.id = -1;
	v4l2_ctrl.value = setting;

	switch (mode) {
		case BRIGHTNESS: v4l2_ctrl.id = V4L2_CID_BRIGHTNESS; config.brightness = setting; break;
		case GAIN: v4l2_ctrl.id = V4L2_CID_GAIN; config.gain = setting; break;
		case EXPOSURE: v4l2_ctrl.id = V4L2_CID_EXPOSURE; config.exposure = setting; break;
#ifdef V4L2_CID_SHARPNESS
		case SHARPNESS: v4l2_ctrl.id = V4L2_CID_SHARPNESS; config.sharpness = setting; break;
#endif
		case GAMMA: v4l2_ctrl.id = V4L2_CID_GAMMA; config.gamma = setting; break;
	} 

	if (v4l2_ctrl.id<0) return false;

	if ((ioctl(cameraID, VIDIOC_S_CTRL, &v4l2_ctrl)) < 0) {
	    return false;
	} return true;
}

int V4Linux2Camera::getCameraSetting(int mode) {

	v4l2_ctrl.id = -1;
	v4l2_ctrl.value = 0;

	switch (mode) {
		case BRIGHTNESS: v4l2_ctrl.id = V4L2_CID_BRIGHTNESS; break;
		case GAIN: v4l2_ctrl.id = V4L2_CID_GAIN; break;
		case EXPOSURE: v4l2_ctrl.id = V4L2_CID_EXPOSURE; break;
#ifdef V4L2_CID_SHARPNESS
		case SHARPNESS: v4l2_ctrl.id = V4L2_CID_SHARPNESS; break;
#endif
		case GAMMA: v4l2_ctrl.id = V4L2_CID_GAMMA; break;
	} if (v4l2_ctrl.id<0) return false;

	if ((ioctl(cameraID, VIDIOC_G_CTRL, &v4l2_ctrl)) < 0) {
	    return 0;
	} return v4l2_ctrl.value;
}

int V4Linux2Camera::getMaxCameraSetting(int mode) {

 	v4l2_query.id = -1;

	switch (mode) {
		case BRIGHTNESS: v4l2_query.id = V4L2_CID_BRIGHTNESS; break;
		case GAIN: v4l2_query.id = V4L2_CID_GAIN; break;
		case EXPOSURE: v4l2_query.id = V4L2_CID_EXPOSURE; break;
#ifdef V4L2_CID_SHARPNESS
		case SHARPNESS: v4l2_query.id = V4L2_CID_SHARPNESS; break;
#endif
		case GAMMA: v4l2_query.id = V4L2_CID_GAMMA; break;
	} if (v4l2_query.id<0) return 0;

	if (( ioctl(cameraID, VIDIOC_QUERYCTRL, &v4l2_query)) < 0) {
		return 0;
    	} else if (v4l2_query.flags & V4L2_CTRL_FLAG_DISABLED) {
		return 0;
    	} else if (v4l2_query.flags & V4L2_CTRL_TYPE_BOOLEAN) {
		return 1;
    	} else if (v4l2_query.type & V4L2_CTRL_TYPE_INTEGER) {
		return v4l2_query.maximum;
    	}

	return 0;
}

int V4Linux2Camera::getMinCameraSetting(int mode) {
 	v4l2_query.id = -1;

	switch (mode) {
		case BRIGHTNESS: v4l2_query.id = V4L2_CID_BRIGHTNESS; break;
		case GAIN: v4l2_query.id = V4L2_CID_GAIN; break;
		case EXPOSURE: v4l2_query.id = V4L2_CID_EXPOSURE; break;
#ifdef V4L2_CID_SHARPNESS
		case SHARPNESS: v4l2_query.id = V4L2_CID_SHARPNESS; break;
#endif
		case GAMMA: v4l2_query.id = V4L2_CID_GAMMA; break;
	} if (v4l2_query.id<0) return 0;

	if (( ioctl(cameraID, VIDIOC_QUERYCTRL, &v4l2_query)) < 0) {
		return 0;
    	} else if (v4l2_query.flags & V4L2_CTRL_FLAG_DISABLED) {
		return 0;
    	} else if (v4l2_query.flags & V4L2_CTRL_TYPE_BOOLEAN) {
		return 1;
    	} else if (v4l2_query.type & V4L2_CTRL_TYPE_INTEGER) {
		return v4l2_query.minimum;
    	}

	return 0;
}

int V4Linux2Camera::getCameraSettingStep(int mode) {
 	v4l2_query.id = -1;

	switch (mode) {
		case BRIGHTNESS: v4l2_query.id = V4L2_CID_BRIGHTNESS; break;
		case GAIN: v4l2_query.id = V4L2_CID_GAIN; break;
		case EXPOSURE: v4l2_query.id = V4L2_CID_EXPOSURE; break;
#ifdef V4L2_CID_SHARPNESS
		case SHARPNESS: v4l2_query.id = V4L2_CID_SHARPNESS; break;
#endif
		case GAMMA: v4l2_query.id = V4L2_CID_GAMMA; break;
	} if (v4l2_query.id<0) return 0;

	if (( ioctl(cameraID, VIDIOC_QUERYCTRL, &v4l2_query)) < 0) {
		return 0;
    	} else if (v4l2_query.flags & V4L2_CTRL_FLAG_DISABLED) {
		return 0;
    	} else if (v4l2_query.flags & V4L2_CTRL_TYPE_BOOLEAN) {
		return 1;
    	} else if (v4l2_query.type & V4L2_CTRL_TYPE_INTEGER) {
		return v4l2_query.step;
    	}

	return 0;
}

