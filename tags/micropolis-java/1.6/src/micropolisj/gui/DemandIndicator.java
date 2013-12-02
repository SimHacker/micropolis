// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.gui;

import java.awt.*;
import java.awt.image.*;
import java.net.URL;
import javax.swing.*;

import micropolisj.engine.*;

public class DemandIndicator extends JComponent
	implements Micropolis.Listener
{
	Micropolis engine;

	public DemandIndicator()
	{
	}

	public void setEngine(Micropolis newEngine)
	{
		if (engine != null) { //old engine
			engine.removeListener(this);
		}

		engine = newEngine;

		if (engine != null) { //new engine
			engine.addListener(this);
		}
		repaint();
	}

	static final BufferedImage backgroundImage = loadImage("/demandg.png");
	static BufferedImage loadImage(String resourceName)
	{
		URL iconUrl = MicropolisDrawingArea.class.getResource(resourceName);
		Image refImage = new ImageIcon(iconUrl).getImage();

		BufferedImage bi = new BufferedImage(refImage.getWidth(null), refImage.getHeight(null),
					BufferedImage.TYPE_INT_RGB);
		Graphics2D gr = bi.createGraphics();
		gr.drawImage(refImage, 0, 0, null);

		return bi;
	}

	static final Dimension MY_SIZE = new Dimension(
				backgroundImage.getWidth(),
				backgroundImage.getHeight()
				);

	@Override
	public Dimension getMinimumSize()
	{
		return MY_SIZE;
	}

	@Override
	public Dimension getPreferredSize()
	{
		return MY_SIZE;
	}

	@Override
	public Dimension getMaximumSize()
	{
		return MY_SIZE;
	}

	static final int UPPER_EDGE = 19;
	static final int LOWER_EDGE = 28;
	static final int MAX_LENGTH = 16;
	static final int RES_LEFT = 8;
	static final int COM_LEFT = 17;
	static final int IND_LEFT = 26;
	static final int BAR_WIDTH = 6;

	public void paintComponent(Graphics gr1)
	{
		Graphics2D gr = (Graphics2D) gr1;
		gr.drawImage(backgroundImage, 0, 0, null);

		if (engine == null)
			return;

		int resValve = engine.getResValve();
		int ry0 = resValve <= 0 ? LOWER_EDGE : UPPER_EDGE;
		int ry1 = ry0 - resValve/100;

		if (ry1 - ry0 > MAX_LENGTH) { ry1 = ry0 + MAX_LENGTH; }
		if (ry1 - ry0 < -MAX_LENGTH) { ry1 = ry0 - MAX_LENGTH; }

		int comValve = engine.getComValve();
		int cy0 = comValve <= 0 ? LOWER_EDGE : UPPER_EDGE;
		int cy1 = cy0 - comValve/100;

		int indValve = engine.getIndValve();
		int iy0 = indValve <= 0 ? LOWER_EDGE : UPPER_EDGE;
		int iy1 = iy0 - indValve/100;

		if (ry0 != ry1)
		{
			Rectangle resRect = new Rectangle(RES_LEFT, Math.min(ry0,ry1), BAR_WIDTH, Math.abs(ry1-ry0));
			gr.setColor(Color.GREEN);
			gr.fill(resRect);
			gr.setColor(Color.BLACK);
			gr.draw(resRect);
		}

		if (cy0 != cy1)
		{
			Rectangle comRect = new Rectangle(COM_LEFT, Math.min(cy0,cy1), BAR_WIDTH, Math.abs(cy1-cy0));
			gr.setColor(Color.BLUE);
			gr.fill(comRect);
			gr.setColor(Color.BLACK);
			gr.draw(comRect);
		}

		if (iy0 != iy1)
		{
			Rectangle indRect = new Rectangle(IND_LEFT, Math.min(iy0,iy1), BAR_WIDTH, Math.abs(iy1-iy0));
			gr.setColor(Color.YELLOW);
			gr.fill(indRect);
			gr.setColor(Color.BLACK);
			gr.draw(indRect);
		}
	}

	//implements Micropolis.Listener
	public void demandChanged()
	{
		repaint();
	}

	//implements Micropolis.Listener
	public void cityMessage(MicropolisMessage m, CityLocation p) { }
	public void citySound(Sound sound, CityLocation p) { }
	public void censusChanged() { }
	public void evaluationChanged() { }
	public void fundsChanged() { }
	public void optionsChanged() { }
}
