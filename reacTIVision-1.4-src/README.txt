reacTIVision 1.4
----------------
(c) 2005-2009 by Martin Kaltenbrunner <mkalten@iua.upf.edu>
http://reactivision.sourceforge.net/

reacTIVision is an open source, cross-platform computer vision
framework for the fast and robust tracking of fiducial markers
attached onto physical objects, as well as for multi-touch
finger tracking. It was mainly designed as a toolkit for the 
rapid development of table-based tangible user interfaces (TUI)
and multi-touch interactive surfaces. This framework has been 
developed by Martin Kaltenbrunner and Ross Bencina at the Music 
Technology Group at the Universitat Pompeu Fabra in Barcelona, Spain
as part of the the reacTable project, a novel electronic music 
instrument with a table-top multi-touch tangible user interface. 

reacTIVision is a standalone application, which sends OpenSound
Control messages via a UDP network socket to any connected
client application. It implements the TUIO protocol, which was
specially designed for transmitting the state of tangible objects
and multi-touch events on a table surface. 
As an alternative reacTIVision can also send MIDI messages.

The framework includes a set of example client projects
for various programming languages, which serve as a base for
the easy development of tangible user interface or multi-touch
applications. Example projects are available for languages such as:
C/C++, Java, C#, Processing, Pure Data, Max/MSP, Flash and Quartz Composer. 

The reacTIVision application runs and compiles under the 
following operating systems: Windows, Mac OS X and Linux
Under Windows it suports any camera with a proper WDM driver,
such as USB, USB2, FireWire and DV cameras. Under Mac OS X
all FireWire cameras and any camera supported by QuickTime 
will work in reacTIVision. Under Linux FireWire cameras are 
best supported as well many V4L2 USB and USB2 cameras.

Fiducial Symbols
----------------
This application was designed to track specially designed fiducial
markers. You will find the default "amoeba" set of 216 fiducials within
the document "default.pdf" within the symbols folder. Print this
document and attach the labels to any object you want to track.
The default fiducial tracking engine is using Ross Bencina's
fidtrack library which is basically a newer high-performance
implementation of Enrico Costanza's d-touch concept. Alternatively
you can use the "classic" or the original "dtouch" sets.
See below how to configure the application to use these older symbol sets.

reacTIVision detects the ID, position and rotation angle of fiducial 
markers in the video image and transmits these values to the client
application via TUIO, a protocol based on Open Sound Control.
TUIO assigns a session ID to each object in the scene and transmits this 
session ID along with the fiducial ID. This allows the identification 
and tracking of several objects with the same ID.

Finger Tracking
---------------
reacTIVision also allows multi-touch finger tracking,
which is basically interpreting any round white region 
of a given size as a finger that is touching the surface.
Finger tracking is turned off by default and can be enabled by  
pressing the 'F' key and adjusting the average finger size, 
which is given in pixels. The general recognition sensitivity
can also be adjusted, where the value is given as a percentage.
80 would be less sensitive and 120 more sensitive, which means
that also less probable regions are interpreted as a finger.
The finger tracking should work with DI (diffuse illumination) 
as well as with FTIR solutions.

Application Handling
--------------------
Before starting the reacTIVision application make sure you have
a supported camera connected to your system. The application will
obviously not work at all without a camera. When running the application 
will show a single video window with the current camera image in B&W.

Pressing the 'S' key will show to the original source image.
Pressing 'T' will show the binary tresholded image, pressing the
'N' key will turn the display off, which reduces the CPU usage.

The thresholder gradient gate can be adjusted by hitting the 'G' key.
Lowering the value can improve the thresholder performance in low
light conditions with insufficient finger contrast for example.
You can gradually lower the value just before noise appears in the image.

The camera options can be adjusted by pressing the 'O' key. 
On Windows and Mac OS this will show a system dialog that allows 
the adjustment of the available camera parameters. On Linux (Mac OS X
when using IEEE1394 cameras), the available camera settings can be
adjusted with a simple on screen display. 

In order to produce some more verbose debugging output, hitting
the 'V' will print the symbol and finger data to the console.
Pressing the 'H' key will display all these options on the screen.
The 'P' key pauses the image analysis completely, hitting 'ESC'
will quit the application. 

XML configuration file
----------------------

Common settings can be edited within the file "reacTIVision.xml" where 
all changes are stored automatically when closing the application.
Under Mac OS X the XML configuration file can be found within the 
application bundle's Resources folder. Select "Show Package Contents" 
from the application's context menu in order to access and edit the file. 

The reacTIVision application usually sends the TUIO messages 
to port 3333 on locahost (127.0.0.1)
You can change this setting by adding or editing the XML tag
<tuio host="127.0.0.1" port="3333"> to the configuration.

The <fiducial engine="amoeba" tree="default"/> XML tag lets you
select the fiducial engine or an alternative amoeba tree order.
The default engine is using the fastest and effective 'amoeba' 
fiducial set. Add the 'classic' option in order to use Ross' 
initial d-touch reimplementation. You can select Enrico's original 
d-touch engine by providing the 'dtouch' option.

The display attribute defines the default screen upon startup.
The <image display="dest" equalize="false" gradient="32" /> lets
you adjust the default gradient gate value.
reacTIVision comes with a background subtraction module,  which in
some cases can increase the recognition performance of both the finger
and fiducial tracking. Within the running application you can toggle
this with the 'E' key or recalculate the background subtraction 
by hitting the SPACE bar.

The overall camera settings can be configured within the camera.xml 
configuration file. Please note that for Windows and the Quicktime 
mode on Mac OS X this only allows the configuration of the image size.
For IEEE1394 cameras on Mac OS X as well as all camera types on Linux
all image parameters can be fully configured using configuration file.

TUIO vs. MIDI
-------------
The application can alternatively send  MIDI messages, which allows to map 
any object dimension (xpos, ypos, angle) to a MIDI control 
with an XML configuration file. Adding and removing objects
can be mapped to simple note ON/OFF events. Keep in mind though that MIDI 
has less bandwidth and data resolution compared to Open Sound Contol, 
so the new MIDI feature is meant as an convenient alternative in some cases, 
but TUIO still will be the primary messaging layer.

Adding <midi config="midi_demo.xml" /> to reacTIVision.xml switches to
MIDI mode and specifies the MIDI configuration files that contains the mappings
and MIDI device selection. An example configuration file along with an 
example PD patch can be found in the midi folder. 
You can list all available MIDI devices with the "-l midi" option.

Calibration and Distortion
--------------------------
Some tables, such as the reacTable are using a mirror in order
to increase the area visible to the camera at a minimal distance. 
This mirror as well as a fish-eye lens unfortunately distort the image.
reacTIVision can correct the distortion and alignment of the image.

For the calibration print and place the provided calibration sheet
on the table and adjust the grid points to the grid on the sheet.
To calibrate reacTIVision switch to calibration mode hitting 'C'

Use the keys A,D,W,X to move within grid,
moving with the cursor keys will adjust the grid point
'O' resets the whole calibration grid, 'U' resets the selected point
and 'K' reverts to the saved grid.

To check if the distortion is working properly press 'R'. This will show
the fully distorted live video image in the target window. Of course the
distortion algorithm only corrects the found fiducial positions instead 
of the full image.

Compilation
-----------
The source distribution includes projects for all three suppported
platforms and their respective build systems: Linux, Win32, MacOS X.

Win32:
A Visual Studio 2005 project as well as all the necessary libraries
and headers (SDL, DsVideoLib) are included. The project should build
right away without any additional configuration.

Mac OS X:
An Xcode project for Xcode version 2.4 or later is included.
The build will require an installed SDL framework in order to
compile properly.

Linux:
Call make to build the application, the distribution also
inludes a spec file for the creation of an RPM package.
Make sure you have the following libraries and headers installed:
libSDL 1.2 and libdc1394 2.0 or later. 

License
-------
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
