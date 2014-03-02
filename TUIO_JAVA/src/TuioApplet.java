/*
	 TUIO Java Demo - part of the reacTIVision project
	 http://reactivision.sourceforge.net/
	 
	 Copyright (c) 2005-2009 Martin Kaltenbrunner <mkalten@iua.upf.edu>
	 
	 This program is free software; you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation; either version 2 of the License, or
	 (at your option) any later version.
	 
	 This program is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	 GNU General Public License for more details.
	 
	 You should have received a copy of the GNU General Public License
	 along with this program; if not, write to the Free Software
	 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

import java.applet.*;
import java.awt.*;
import javax.swing.*;
import java.io.*;
import java.net.*;
import TUIO.*;

public class TuioApplet extends JApplet {
	
	TuioDemoComponent demo;
	TuioClient client;
	int port = 3333;
	
	public void init() {
		try {
			port = Integer.parseInt(getParameter("port"));
		} catch (Exception e) {}
		
		Dimension size = this.getSize();
		
		TuioDemoComponent demo = new TuioDemoComponent();
		demo.setSize(size.width,size.height);
	
		client = new TuioClient();
		client.addTuioListener(demo);
		
		add(demo);
		repaint();
	}

	public void start() {
		if (!client.isConnected()) client.connect();
	}

	public void stop() {
		if (client.isConnected()) client.disconnect();
	}

	public void destroy() {
		if (client.isConnected()) client.disconnect();
		client = null;
	}

	public void paint( Graphics g ) {
	}
}
