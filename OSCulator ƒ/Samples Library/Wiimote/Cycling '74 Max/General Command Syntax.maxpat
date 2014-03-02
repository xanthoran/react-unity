{
	"patcher" : 	{
		"fileversion" : 1,
		"rect" : [ 130.0, 274.0, 936.0, 502.0 ],
		"bglocked" : 0,
		"defrect" : [ 130.0, 274.0, 936.0, 502.0 ],
		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"toolbarvisible" : 1,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"boxes" : [ 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Osculator General Commands syntax:\n          /osculator/<item> \"<command name>\"\n\nQueries and command can be iterated over a collection, eg:\n         /osculator/wii/* reset\n… will reset the settings for every Wiimotes.\n\nCommands ending with a question mark are queries.  A reply will be sent to the Default Host (see Parameters Window / OSC Routes tab).  For example:\n         /osculator/wii/1 battery?\n\nWill query the battery status on the first Wiimote.  A reply is sent to the Default Host, to the /osculator/wii/1 address, with the first arguments being the command name, without the question mark:\n          reply:  /osculator/wii/1 battery 0.87",
					"linecount" : 16,
					"presentation_linecount" : 16,
					"patching_rect" : [ 29.0, 25.0, 356.0, 227.0 ],
					"presentation" : 1,
					"id" : "obj-20",
					"fontname" : "Arial",
					"numinlets" : 1,
					"bgcolor" : [ 0.858824, 0.858824, 0.858824, 1.0 ],
					"fontsize" : 12.0,
					"numoutlets" : 0,
					"presentation_rect" : [ 8.0, 16.0, 375.0, 227.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Query the Wiimote for its battery level and connection status.  The reply contains a value betwen 0.0 and 1.0.\nIf the Wiimote is not connected, 'nil' is returned.",
					"linecount" : 7,
					"presentation_linecount" : 6,
					"patching_rect" : [ 683.0, 181.0, 214.0, 146.0 ],
					"presentation" : 1,
					"id" : "obj-18",
					"fontname" : "Arial",
					"numinlets" : 1,
					"bgcolor" : [ 0.858824, 0.858824, 0.858824, 1.0 ],
					"fontsize" : 17.307152,
					"numoutlets" : 0,
					"presentation_rect" : [ 80.0, 503.790436, 240.0, 126.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Reset the Wiimote address and settings to the default values.\nIf a Wiimote is already connected it will be disconnected.",
					"linecount" : 6,
					"presentation_linecount" : 6,
					"patching_rect" : [ 561.0, 16.0, 214.0, 126.0 ],
					"presentation" : 1,
					"id" : "obj-17",
					"fontname" : "Arial",
					"numinlets" : 1,
					"bgcolor" : [ 0.858824, 0.858824, 0.858824, 1.0 ],
					"fontsize" : 17.307152,
					"numoutlets" : 0,
					"presentation_rect" : [ 80.0, 350.790436, 241.0, 126.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "/osculator/wii/* connected?",
					"outlettype" : [ "" ],
					"patching_rect" : [ 512.0, 239.0, 153.0, 18.0 ],
					"presentation" : 1,
					"id" : "obj-12",
					"fontname" : "Arial",
					"numinlets" : 2,
					"fontsize" : 12.0,
					"numoutlets" : 1,
					"presentation_rect" : [ 339.0, 554.0, 163.0, 18.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "/osculator/wii/* battery?",
					"outlettype" : [ "" ],
					"patching_rect" : [ 530.0, 182.0, 135.0, 18.0 ],
					"presentation" : 1,
					"id" : "obj-10",
					"fontname" : "Arial",
					"numinlets" : 2,
					"fontsize" : 12.0,
					"numoutlets" : 1,
					"presentation_rect" : [ 339.0, 529.0, 142.0, 18.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "print",
					"patching_rect" : [ 93.0, 435.0, 34.0, 20.0 ],
					"id" : "obj-9",
					"fontname" : "Arial",
					"numinlets" : 1,
					"fontsize" : 12.0,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "/osculator/wii/1 reset",
					"outlettype" : [ "" ],
					"patching_rect" : [ 417.0, 120.0, 120.0, 18.0 ],
					"presentation" : 1,
					"id" : "obj-6",
					"fontname" : "Arial",
					"numinlets" : 2,
					"fontsize" : 12.0,
					"numoutlets" : 1,
					"presentation_rect" : [ 339.0, 381.0, 139.0, 18.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "/osculator/wii/* reset",
					"outlettype" : [ "" ],
					"patching_rect" : [ 417.0, 99.0, 118.0, 18.0 ],
					"presentation" : 1,
					"id" : "obj-11",
					"fontname" : "Arial",
					"numinlets" : 2,
					"fontsize" : 12.0,
					"numoutlets" : 1,
					"presentation_rect" : [ 339.0, 355.0, 142.0, 18.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "/osculator/wii/2 battery?",
					"outlettype" : [ "" ],
					"patching_rect" : [ 530.0, 205.0, 137.0, 18.0 ],
					"presentation" : 1,
					"id" : "obj-5",
					"fontname" : "Arial",
					"numinlets" : 2,
					"fontsize" : 12.0,
					"numoutlets" : 1,
					"presentation_rect" : [ 339.0, 505.0, 142.0, 18.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "udpreceive 9000",
					"outlettype" : [ "" ],
					"patching_rect" : [ 92.0, 389.0, 99.0, 20.0 ],
					"id" : "obj-2",
					"fontname" : "Arial",
					"numinlets" : 1,
					"fontsize" : 12.0,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "udpsend localhost 8000",
					"patching_rect" : [ 407.0, 296.0, 137.0, 20.0 ],
					"id" : "obj-1",
					"fontname" : "Arial",
					"numinlets" : 1,
					"fontsize" : 12.0,
					"numoutlets" : 0
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"source" : [ "obj-2", 0 ],
					"destination" : [ "obj-9", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-5", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-11", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-6", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-12", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
 ]
	}

}
