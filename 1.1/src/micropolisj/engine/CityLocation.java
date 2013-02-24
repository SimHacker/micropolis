// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

/**
 * Coordinates of a location (x,y) in the city.
 */
public class CityLocation
{
	/** The X coordinate of this location.
	 * Increasing X coordinates correspond to East,
	 * and decreasing X coordinates correspond to West. */
	public int x;

	/** The Y coordinate of this location.
	 * Increasing Y coordinates correspond to South,
	 * and decreasing Y coordinates correspond to North. */
	public int y;

	/**
	 * Constructs and initializes city coordinates.
	 */
	public CityLocation(int x, int y)
	{
		this.x = x;
		this.y = y;
	}

	@Override
	public int hashCode()
	{
		return x*33+y;
	}

	@Override
	public boolean equals(Object obj)
	{
		if (obj instanceof CityLocation) {
			CityLocation rhs = (CityLocation)obj;
			return this.x == rhs.x && this.y == rhs.y;
		}
		else {
			return false;
		}
	}

	@Override
	public String toString()
	{
		return "("+x+","+y+")";
	}
}
