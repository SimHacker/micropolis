// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

import java.util.*;
import static micropolisj.engine.TileConstants.*;

public class ToolPreview implements ToolEffectIfc
{
	public int offsetX;
	public int offsetY;
	public short [][] tiles;
	public int cost;
	public ToolResult toolResult;
	public List<SoundInfo> sounds;

	public static class SoundInfo
	{
		public int x;
		public int y;
		public Sound sound;

		SoundInfo(int x, int y, Sound sound)
		{
			this.x = x;
			this.y = y;
			this.sound = sound;
		}
	}

	ToolPreview()
	{
		this.tiles = new short[0][0];
		this.sounds = new ArrayList<SoundInfo>();
		this.toolResult = ToolResult.NONE;
	}

	//implements ToolEffectIfc
	public int getTile(int dx, int dy)
	{
		if (inRange(dx, dy)) {
			return tiles[offsetY+dy][offsetX+dx];
		}
		else {
			return CLEAR;
		}
	}

	public CityRect getBounds()
	{
		return new CityRect(
			-offsetX,
			-offsetY,
			getWidth(),
			getHeight()
			);
	}

	int getWidth()
	{
		return tiles.length != 0 ? tiles[0].length : 0;
	}

	int getHeight()
	{
		return tiles.length;
	}

	boolean inRange(int dx, int dy)
	{
		return offsetY+dy >= 0 &&
			offsetY+dy < getHeight() &&
			offsetX+dx >= 0 &&
			offsetX+dx < getWidth();
	}

	void expandTo(int dx, int dy)
	{
		if (tiles == null || tiles.length == 0) {
			tiles = new short[1][1];
			tiles[0][0] = CLEAR;
			offsetX = -dx;
			offsetY = -dy;
			return;
		}

		// expand each existing row as needed
		for (int i = 0; i < tiles.length; i++) {
			short[] A = tiles[i];
			if (offsetX+dx >= A.length) {
				int newLen = offsetX+dx+1;
				short[] AA = new short[newLen];
				System.arraycopy(A, 0, AA, 0, A.length);
				Arrays.fill(AA, A.length, newLen, CLEAR);
				tiles[i] = AA;
			}
			else if (offsetX+dx < 0) {
				int addl = -(offsetX+dx);
				int newLen = A.length + addl;
				short[] AA = new short[newLen];
				System.arraycopy(A, 0, AA, addl, A.length);
				Arrays.fill(AA, 0, addl, CLEAR);
				tiles[i] = AA;
			}
		}

		if (offsetX+dx < 0) {
			int addl = -(offsetX+dx);
			offsetX += addl;
		}

		int width = tiles[0].length;
		if (offsetY+dy >= tiles.length) {
			int newLen = offsetY+dy+1;
			short[][] newTiles = new short[newLen][width];
			System.arraycopy(tiles, 0, newTiles, 0, tiles.length);
			for (int i = tiles.length; i < newLen; i++) {
				Arrays.fill(newTiles[i], CLEAR);
			}
			tiles = newTiles;
		}
		else if (offsetY+dy < 0) {
			int addl = -(offsetY+dy);
			int newLen = tiles.length + addl;
			short[][] newTiles = new short[newLen][width];
			System.arraycopy(tiles, 0, newTiles, addl, tiles.length);
			for (int i = 0; i < addl; i++) {
				Arrays.fill(newTiles[i], CLEAR);
			}
			tiles = newTiles;

			offsetY += addl;
		}
	}

	//implements ToolEffectIfc
	public void makeSound(int dx, int dy, Sound sound)
	{
		sounds.add(new SoundInfo(dx, dy, sound));
	}

	//implements ToolEffectIfc
	public void setTile(int dx, int dy, int tileValue)
	{
		expandTo(dx, dy);
		tiles[offsetY+dy][offsetX+dx] = (short)tileValue;
	}

	//implements ToolEffectIfc
	public void spend(int amount)
	{
		cost += amount;
	}

	//implements ToolEffectIfc
	public void toolResult(ToolResult tr)
	{
		this.toolResult = tr;
	}
}
