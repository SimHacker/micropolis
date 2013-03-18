// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj;

import javax.swing.*;

import micropolisj.gui.MainWindow;

public class Main
{
	static void createAndShowGUI()
	{
		MainWindow win = new MainWindow();
		win.setVisible(true);
		win.doNewCity(true);
	}

	public static void main(String [] args)
	{
		SwingUtilities.invokeLater(new Runnable() {
		public void run() {
			createAndShowGUI();
		}});
	}
}
