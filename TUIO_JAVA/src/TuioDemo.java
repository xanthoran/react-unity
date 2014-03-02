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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import java.awt.image.*;
import java.util.*;
import javax.swing.*;
import TUIO.*;

public class TuioDemo  {

	private final int window_width  = 640;
	private final int window_height = 480;

	private boolean fullscreen = false;
	
	private TuioDemoComponent demo;
	private JFrame frame;
	private GraphicsDevice device;
	
	public TuioDemo() {
		demo = new TuioDemoComponent();
		device = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();
		setupWindow();
		showWindow();
	}
	
	public TuioListener getTuioListener() {
		return demo;
	}
	
	public void setupWindow() {
	
		frame = new JFrame();
		frame.add(demo);

		frame.setTitle("TuioDemo");
		frame.setResizable(false);

		frame.addWindowListener( new WindowAdapter() { public void windowClosing(WindowEvent evt) {
				System.exit(0);
			} });
		
		frame.addKeyListener( new KeyAdapter() { public void keyPressed(KeyEvent evt) {
			if (evt.getKeyCode()==KeyEvent.VK_ESCAPE) System.exit(0);
			else if (evt.getKeyCode()==KeyEvent.VK_F1) {
				destroyWindow();
				setupWindow();
				fullscreen = !fullscreen;
				showWindow();
			}
			else if (evt.getKeyCode()==KeyEvent.VK_V) demo.verbose=!demo.verbose;
		} });
	}
	
	public void destroyWindow() {
	
		frame.setVisible(false);
		if (fullscreen) {
			device.setFullScreenWindow(null);		
		}
		frame = null;
	}
	
	public void showWindow() {
	
		if (fullscreen) {
			int width  = (int)Toolkit.getDefaultToolkit().getScreenSize().getWidth();
			int height = (int)Toolkit.getDefaultToolkit().getScreenSize().getHeight();
			demo.setSize(width,height);

			frame.setSize(width,height);
			frame.setUndecorated(true);
			device.setFullScreenWindow(frame);		
		} else {
			int width  = window_width;
			int height = window_height;
			demo.setSize(width,height);
			
			frame.pack();
			Insets insets = frame.getInsets();			
			frame.setSize(width,height +insets.top);
			
		}
		
		frame.setVisible(true);
		frame.repaint();

	}
	
	public static void main(String argv[]) {
		
		TuioDemo demo = new TuioDemo();
		TuioClient client = null;
 
		switch (argv.length) {
			case 1:
				try { 
					client = new TuioClient( Integer.parseInt(argv[0])); 
				} catch (Exception e) {
					System.out.println("usage: java TuioDemo [port]");
					System.exit(0);
				}
				break;
			case 0:
				client = new TuioClient();
				break;
			default: 
				System.out.println("usage: java TuioDemo [port]");
				System.exit(0);
				break;
		}
		
		if (client!=null) {
			client.addTuioListener(demo.getTuioListener());
			client.connect();
		} else {
			System.out.println("usage: java TuioDemo [port]");
			System.exit(0);
		}
	}
	
}
