// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

/**
 * Specifies a rectangular area in the city's coordinate space.
 * This class is functionally equivalent to Java AWT's Rectangle
 * class, but is portable to Java editions that do not contain AWT.
 */
public class CityRect
{
	/** The X coordinate of the upper-left corner of the rectangle. */
	public int x;

	/** The Y coordinate of the upper-left corner of the rectangle. */
	public int y;

	/** The width of the rectangle. */
	public int width;

	/** The height of the rectangle. */
	public int height;

	public CityRect()
	{
	}

	public CityRect(int x, int y, int width, int height)
	{
		this.x = x;
		this.y = y;
		this.width = width;
		this.height = height;
	}

	@Override
	public boolean equals(Object obj)
	{
		if (obj instanceof CityRect) {
			CityRect rhs = (CityRect)obj;
			return this.x == rhs.x &&
				this.y == rhs.y &&
				this.width == rhs.width &&
				this.height == rhs.height;
		}
		else {
			return false;
		}
	}

	@Override
	public String toString()
	{
		return "["+x+","+y+","+width+"x"+height+"]";
	}
}
