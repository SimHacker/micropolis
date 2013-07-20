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
import java.util.*;
import javax.swing.*;

import micropolisj.engine.*;
import static micropolisj.engine.TileConstants.*;

public class TileImages
{
	final int TILE_WIDTH;
	final int TILE_HEIGHT;
	Image [] images;
	Map<SpriteKind, Map<Integer, Image> > spriteImages;

	private TileImages(int size)
	{
		this.TILE_WIDTH = size;
		this.TILE_HEIGHT = size;
		this.images = loadTileImages("/tiles.png");
	}

	public static TileImages getInstance(int size)
	{
		return new TileImages(size);
	}

	public Image getTileImage(int cell)
	{
		int tile = (cell & LOMASK) % images.length;
		return images[tile];
	}

	private Image [] loadTileImages(String resourceName)
	{
		URL iconUrl = TileImages.class.getResource(resourceName);
		Image refImage = new ImageIcon(iconUrl).getImage();

		GraphicsEnvironment env = GraphicsEnvironment.getLocalGraphicsEnvironment();
		GraphicsDevice dev = env.getDefaultScreenDevice();
		GraphicsConfiguration conf = dev.getDefaultConfiguration();

		Image [] images = new Image[refImage.getHeight(null) / TILE_HEIGHT];
		for (int i = 0; i < images.length; i++)
		{
			BufferedImage bi = conf.createCompatibleImage(TILE_WIDTH, TILE_HEIGHT, Transparency.OPAQUE);
			Graphics2D gr = bi.createGraphics();
			gr.drawImage(refImage, 0, 0, TILE_WIDTH, TILE_HEIGHT,
				0, i * TILE_HEIGHT,
				TILE_WIDTH, (i+1)*TILE_HEIGHT,
				null);
			
			images[i] = bi;
		}
		return images;
	}

	public Image getSpriteImage(SpriteKind kind, int frameNumber)
	{
		return spriteImages.get(kind).get(frameNumber);
	}

	private void loadSpriteImages()
	{
		spriteImages = new EnumMap<SpriteKind, Map<Integer,Image> >(SpriteKind.class);
		for (SpriteKind kind : SpriteKind.values())
		{
			HashMap<Integer,Image> imgs = new HashMap<Integer,Image>();
			for (int i = 0; i < kind.numFrames; i++) {
				Image img = loadSpriteImage(kind, i);
				if (img != null) {
					imgs.put(i, img);
				}
			}
			spriteImages.put(kind, imgs);
		}
	}

	static Image loadSpriteImage(SpriteKind kind, int frameNo)
	{
		String resourceName = "/obj"+kind.objectId+"-"+frameNo+".png";
		URL iconUrl = TileImages.class.getResource(resourceName);
		if (iconUrl == null)
			return null;

		return new ImageIcon(iconUrl).getImage();
	}

}
