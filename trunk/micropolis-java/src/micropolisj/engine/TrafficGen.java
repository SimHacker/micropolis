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

/**
 * Contains the code for generating city traffic.
 */
class TrafficGen
{
	final Micropolis city;
	int mapX;
	int mapY;
	ZoneType sourceZone;

	int lastdir;
	Stack<CityLocation> positions = new Stack<CityLocation>();

	static final int MAX_TRAFFIC_DISTANCE = 30;

	public TrafficGen(Micropolis city)
	{
		this.city = city;
	}

	int makeTraffic()
	{
		if (findPerimeterRoad()) //look for road on this zone's perimeter
		{
			if (tryDrive())  //attempt to drive somewhere
			{
				// success; incr trafdensity
				setTrafficMem();
				return 1;
			}

			return 0;
		}
		else
		{
			// no road found
			return -1;
		}
	}

	void setTrafficMem()
	{
		while (!positions.isEmpty())
		{
			CityLocation pos = positions.pop();
			mapX = pos.x;
			mapY = pos.y;
			assert city.testBounds(mapX, mapY);

			// check for road/rail
			int tile = city.getTile(mapX, mapY);
			if (tile >= ROADBASE && tile < POWERBASE)
			{
				city.addTraffic(mapX, mapY, 50);
			}
		}
	}

	static final int [] PerimX = { -1, 0, 1,  2, 2, 2,  1, 0,-1, -2,-2,-2 };
	static final int [] PerimY = { -2,-2,-2, -1, 0, 1,  2, 2, 2,  1, 0,-1 };
	boolean findPerimeterRoad()
	{
		for (int z = 0; z < 12; z++)
		{
			int tx = mapX + PerimX[z];
			int ty = mapY + PerimY[z];

			if (roadTest(tx, ty))
			{
				mapX = tx;
				mapY = ty;
				return true;
			}
		}
		return false;
	}

	boolean roadTest(int tx, int ty)
	{
		if (!city.testBounds(tx, ty)) {
			return false;
		}

		char c = city.getTile(tx, ty);

		if (c < ROADBASE)
			return false;
		else if (c > LASTRAIL)
			return false;
		else if (c >= POWERBASE && c < LASTPOWER)
			return false;
		else
			return true;
	}

	boolean tryDrive()
	{
		lastdir = 5;
		positions.clear();

		for (int z = 0; z < MAX_TRAFFIC_DISTANCE; z++) //maximum distance to try
		{
			if (tryGo(z))
			{
				// got a road
				if (driveDone())
				{
					// destination reached
					return true;
				}
			}
			else
			{
				// deadend, try backing up
				if (!positions.isEmpty())
				{
					positions.pop();
					z += 3;
				}
				else
				{
					return false;
				}
			}
		}

		// gone maxdis
		return false;
	}

	static final int [] DX = { 0, 1, 0, -1 };
	static final int [] DY = { -1, 0, 1, 0 };
	boolean tryGo(int z)
	{
		// random starting direction
		int rdir = city.PRNG.nextInt(4);

		for (int d = rdir; d < rdir + 4; d++)
		{
			int realdir = d % 4;
			if (realdir == lastdir)
				continue;

			if (roadTest(mapX + DX[realdir], mapY + DY[realdir]))
			{
				mapX += DX[realdir];
				mapY += DY[realdir];
				lastdir = (realdir + 2) % 4;

				if (z % 2 == 1)
				{
					// save pos every other move
					positions.push(new CityLocation(mapX, mapY));
				}

				return true;
			}
		}

		return false;
	}

	boolean driveDone()
	{
		int low, high;
		switch (sourceZone)
		{
		case RESIDENTIAL:
			low = COMBASE;
			high = NUCLEAR;
			break;
		case COMMERCIAL:
			low = LHTHR;
			high = PORT;
			break;
		case INDUSTRIAL:
			low = LHTHR;
			high = COMBASE;
			break;
		default:
			throw new Error("unreachable");
		}

		if (mapY > 0)
		{
			int tile = city.getTile(mapX, mapY-1);
			if (tile >= low && tile <= high)
				return true;
		}
		if (mapX + 1 < city.getWidth())
		{
			int tile = city.getTile(mapX + 1, mapY);
			if (tile >= low && tile <= high)
				return true;
		}
		if (mapY + 1 < city.getHeight())
		{
			int tile = city.getTile(mapX, mapY + 1);
			if (tile >= low && tile <= high)
				return true;
		}
		if (mapX > 0)
		{
			int tile = city.getTile(mapX - 1, mapY);
			if (tile >= low && tile <= high)
				return true;
		}
		return false;
	}

	/**
	 * The three main types of zones found in Micropolis.
	 */
	static enum ZoneType
	{
		RESIDENTIAL, COMMERCIAL, INDUSTRIAL;
	}
}
