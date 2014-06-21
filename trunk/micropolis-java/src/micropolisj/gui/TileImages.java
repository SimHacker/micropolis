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

		this.images = loadTileImages("/"+size+"x"+size+"/tiles.png", size);
		loadSpriteImages();
	}

	static Map<Integer,TileImages> savedInstances = new HashMap<Integer,TileImages>();

	public static TileImages getInstance(int size)
	{
		if (!savedInstances.containsKey(size)) {
			savedInstances.put(size, new TileImages(size));
		}
		return savedInstances.get(size);
	}

	public Image getTileImage(int cell)
	{
		int tile = (cell & LOMASK) % images.length;
		return images[tile];
	}

	private Image [] loadTileImages(String resourceName, int srcSize)
	{
		URL iconUrl = TileImages.class.getResource(resourceName);
		Image refImage = new ImageIcon(iconUrl).getImage();

		GraphicsEnvironment env = GraphicsEnvironment.getLocalGraphicsEnvironment();
		GraphicsDevice dev = env.getDefaultScreenDevice();
		GraphicsConfiguration conf = dev.getDefaultConfiguration();

		Image [] images = new Image[refImage.getHeight(null) / srcSize];
		for (int i = 0; i < images.length; i++)
		{
			BufferedImage bi = conf.createCompatibleImage(TILE_WIDTH, TILE_HEIGHT, Transparency.OPAQUE);
			Graphics2D gr = bi.createGraphics();
			gr.drawImage(refImage, 0, 0, TILE_WIDTH, TILE_HEIGHT,
				0, i * srcSize,
				0 + srcSize, i * srcSize + srcSize,
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

	Image loadSpriteImage(SpriteKind kind, int frameNo)
	{
		String resourceName = "/obj"+kind.objectId+"-"+frameNo;

		// first, try to load specific size image
		URL iconUrl = TileImages.class.getResource(resourceName+"_"+TILE_WIDTH+"x"+TILE_HEIGHT+".png");
		if (iconUrl != null) {
			return new ImageIcon(iconUrl).getImage();
		}

		iconUrl = TileImages.class.getResource(resourceName+".png");
		if (iconUrl == null)
			return null;

		if (TILE_WIDTH==16 && TILE_HEIGHT==16) {
			return new ImageIcon(iconUrl).getImage();
		}

		// scale the image ourselves
		ImageIcon ii = new ImageIcon(iconUrl);
		int destWidth = ii.getIconWidth() * TILE_WIDTH / 16;
		int destHeight = ii.getIconHeight() * TILE_HEIGHT / 16;

		GraphicsEnvironment env = GraphicsEnvironment.getLocalGraphicsEnvironment();
		GraphicsDevice dev = env.getDefaultScreenDevice();
		GraphicsConfiguration conf = dev.getDefaultConfiguration();
		BufferedImage bi = conf.createCompatibleImage(destWidth, destHeight, Transparency.TRANSLUCENT);
		Graphics2D gr = bi.createGraphics();

		gr.drawImage(ii.getImage(),
			0, 0, destWidth, destHeight,
			0, 0,
			ii.getIconWidth(), ii.getIconHeight(),
			null);
		return bi;
	}

}
