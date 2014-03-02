# README - OSCulator Plug-in for Logic Pro


## Description

The OSCulator Plug-in for Logic Pro is a "Control Surface Plug-in" allowing control of Logic Pro using the Open Sound Control protocol (OSC).

The aim is to create a clean and documented API so OSC applications are easier to interface with Logic Pro.

Please see disclaimer, change log and copyright at the end of the document.


## Compatibility

The plug-in is compatible with:

* Mac OS X 10.6.8 or later.
* Logic Pro 9.1.2 or later.
* Logic Pro Express 9.1.2 or later.
* Requires OSCulator 2.12 or later.


## Installation

Copy the file "OSCulator.bundle" into one of the following directories:

* ~/Library/Application Support/MIDI Device Plug-ins
* /Library/Application Support/MIDI Device Plug-ins



## Logic Configuration

+ Launch Logic.
+ Launch OSCulator.
+ Logic should then display a prompt:

  « Logic Pro has detected a new OSC device named "computer:8000 (OSCulator)", which is supported by the control surface plug-in "OSCulator. »

  Click Add, OSCulator is now ready to communicate with Logic.

## OSCulator configuration

If you want to send OSC messages to Logic Pro, it must be configured as an OSC target.  The OSC target is automatically added in OSCulator 2.12 and later, however you will need to add it manually in previous versions. 

In OSCulator, open the Parameters Window, and navigate to the OSC Routing.  Logic publishes a OSC service whose name is merely the name of your computer.  If your computer's name is "tortilla" then that is what you should see in the OSC target list.

If it is not in the list, click on the gear button and pick the service from there.

Finally, make sure that the target is not the default target, otherwise the service will be replaced by another one next time OSCulator detects a OSC device on the network.

### Attention TouchOSC users

If you have already setup TouchOSC as a control surface in Logic, but would like to use OSCulator to act as a bridge, then you should uninstall the TouchOSC control surface from Logic Pro.


## How to stop using the OSCulator Plug-in for Logic Pro

+ In Logic Pro, open the "Logic Pro" menu, then "Preferences", "Control Surfaces" and "Setup…".
+ Select the OSCulator control surface shown as a big OSCulator icon.
+ Hit backspace to remove it from the configuration.

As long as the plug-in is not uninstalled (see next section), this is only temporary and can be reconfigured again later.


## Complete uninstallation

+ Quit Logic Pro.
+ Remove the OSCulator.bundle file from the following locations:
	* ~/Library/Application Support/MIDI Device Plug-ins
 	* /Library/Application Support/MIDI Device Plug-ins


## Example file: "Logic Pro - TouchOSC (using LogicTouch layout)"

The purpose of this example is to show how to use OSCulator as a bridge between TouchOSC and Logic Pro.  We use the LogicTouch layout (bundled with the TouchOSC app), that was designed to work directly with Logic.  If we don't use TouchOSC as a control surface, but instead use OSCulator, then we have access to the OSC messages that are exchanged between TouchOSC and Logic and can use OSCulator instead.

The example has been made with the iPhone version of this template.  Adapting the example to the iPad version is left as an exercise for the reader.


## OSC dictionary

The file "Logic Pro OSCulator Dictionary.oroutes" is a dictionary of OSC messages and provided to make it easier to assign mappings in OSCulator, or to explore the possible OSC calls.

To use the dictionary, in OSCulator, open the Parameters Window, and go to the OSC Routing tab.  Click on the gear button and then "Import…".  Choose the file named "Logic Pro OSCulator Dictionary.oroutes".


## Troubleshooting

- Launch Logic before OSCulator.

- It seems that in this early version of the plug-in communication can be lost or become out of sync.  To remedy this problem quickly, press the Pause button in OSCulator's toolbar and press Start again.  Logic will re-send every control values, thus re-syncing the two applications.


## Debugging

OSCulator has a debugging mode where all OSC input and output is logged to the console.  In order to enable the debugging mode, type the following line in a Terminal window:

    defaults write net.osculator.OSCulator ShowDebuggingTools YES

and restart OSCulator.  The menu item "Log OSC input" will become available in the Routings menu.

To remove debugging support, type in a Terminal window:

    defaults delete net.osculator.OSCulator ShowDebuggingTools



## OSC API Description

Unless specified, all communications are bi-directional.

(a) unidirectional message: sent from OSCulator to Logic Pro.

(b) unidirectional message: received by OSCulator from Logic Pro.


### Transport

* /logic/transport/play
* /logic/transport/stop
* /logic/transport/rewind
* /logic/transport/forward
* /logic/transport/record
* /logic/transport/cycle
* /logic/transport/click
* /logic/transport/replace
* /logic/transport/bymarker
* /logic/transport/time/smpte (b)
* /logic/transport/time/beats (b)

### Mixer

* /logic/mixer/solo-reset (a)
* /logic/mixer/solo-any (b)

Tracks are grouped by banks of 8 tracks.  In the following messages:

* /logic/mixer/bank+ (a)
* /logic/mixer/bank- (a)


Tracks:

* `track_index` is a number between 1 and 8 or the string 'master'.
* `send_index` is a number between 1 and 3.
* /logic/mixer/track/`track_index`/id (b)
* /logic/mixer/track/`track_index`/name (b)
* /logic/mixer/track/`track_index`/level
* /logic/mixer/track/`track_index`/level/string (b)
* /logic/mixer/track/`track_index`/pan
* /logic/mixer/track/`track_index`/pan/string (b)
* /logic/mixer/track/`track_index`/mute
* /logic/mixer/track/`track_index`/solo
* /logic/mixer/track/`track_index`/focus
* /logic/mixer/track/`track_index`/send/`send_index`/name (b)
* /logic/mixer/track/`track_index`/send/`send_index`/level
* /logic/mixer/track/`track_index`/send/`send_index`/level/string (b)

AUX:

* `aux_index` is a number between 1 and 4.
* /logic/mixer/aux/`aux_index`/id (b)
* /logic/mixer/aux/`aux_index`/name (b)
* /logic/mixer/aux/`aux_index`/level
* /logic/mixer/aux/`aux_index`/level/string (b)
* /logic/mixer/aux/`aux_index`/pan
* /logic/mixer/aux/`aux_index`/pan/string (b)
* /logic/mixer/aux/`aux_index`/mute
* /logic/mixer/aux/`aux_index`/solo

## Track

* /logic/track/track+ (a)
* /logic/track/track- (a)

* /logic/track/id (b)
* /logic/track/name (b)
* /logic/track/level
* /logic/track/level/string (b)
* /logic/track/pan
* /logic/track/pan/string (b)
* /logic/track/mute
* /logic/track/solo
* /logic/track/input-monitoring
* /logic/track/rec-enable

Sends:

* `send_index` is a number between 1 and 5.
* /logic/track/send/`send_index`/name (b)
* /logic/track/send/`send_index`/level
* /logic/track/send/`send_index`/level/string (b)

Inserts:

* `insert_index` is a number between 1 and 12.
* /logic/track/insert+ (a)
* /logic/track/insert- (a)
* /logic/track/insert/`insert_index`/name (b)
* /logic/track/insert/`insert_index`/bypass
* /logic/track/insert/`insert_index`/focus

* `param_index` is a number between 1 and 16.
* /logic/insert/page+ (a)
* /logic/insert/page- (a)
* /logic/insert/page/id (b)
* /logic/insert/param/`param_index`/name (b)
* /logic/insert/param/`param_index`/name/value
* /logic/insert/param/`param_index`/name/value/string (b)

EQ:

* `eq_index` is a number between 1 and 8.
* /logic/track/eq/`eq_index`/bypass
* /logic/track/eq/`eq_index`/gain
* /logic/track/eq/`eq_index`/gain/string (b)
* /logic/track/eq/`eq_index`/gain/+ (a)
* /logic/track/eq/`eq_index`/gain/- (a)
* /logic/track/eq/`eq_index`/freq
* /logic/track/eq/`eq_index`/freq/string (b)
* /logic/track/eq/`eq_index`/freq/+ (a)
* /logic/track/eq/`eq_index`/freq/- (a)
* /logic/track/eq/`eq_index`/q
* /logic/track/eq/`eq_index`/q/string (b)
* /logic/track/eq/`eq_index`/q/+ (a)
* /logic/track/eq/`eq_index`/q/- (a)

Automation:

* /logic/track/automation/off
* /logic/track/automation/read
* /logic/track/automation/latched
* /logic/track/automation/touch
* /logic/track/automation/write

Instrument:

* `param_index` is a number between 1 and 24.
* /logic/track/instrument/page+ (a)
* /logic/track/instrument/page- (a)
* /logic/track/instrument/param/`param_index`/name (b)
* /logic/track/instrument/param/`param_index`/value
* /logic/track/instrument/param/`param_index`/value/string (b)



## Support

For problems, bugs or requests, please post a report on the OSCulator Forum:

	http://www.osculator.net/forum


## Change log

* 06/15/2012:
  Updated the example file to disable Logic feedback messages when a fader or rotary is touched. 
  
* 05/29/2012:
  Initial version


## Disclaimer and copyright

This is a work in progress, and as such the following disclaimer applies:

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Copyright &copy; 2012, Wildora, all right reserved.
