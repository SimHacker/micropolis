// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

public class CityLocation
{
	public int x;
	public int y;

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
