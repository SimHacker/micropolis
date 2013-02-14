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

class TrafficGen
{
	Micropolis engine;
	int mapX;
	int mapY;
	Micropolis.ZoneType sourceZone;

	int lastdir;
	Stack<Position> positions = new Stack<>();

	static final int MAX_TRAFFIC_DISTANCE = 30;

	public TrafficGen(Micropolis engine)
	{
		this.engine = engine;
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
			Position pos = positions.pop();
			mapX = pos.x;
			mapY = pos.y;
			assert engine.testBounds(mapX, mapY);

			int tile = engine.getTile(mapX, mapY) & LOMASK;
			if (tile >= ROADBASE && tile < POWERBASE)
			{
				// check for rail
				int z = engine.trfDensity[mapY/2][mapX/2];
				z += 50;

				if (z > 240 && engine.PRNG.nextInt(6) == 0)
				{
					z = 240;
					engine.trafficMaxLocationX = mapX;
					engine.trafficMaxLocationY = mapY;

					HelicopterSprite copter = (HelicopterSprite) engine.getSprite(SpriteKind.COP);
					if (copter != null) {
						copter.destX = mapX;
						copter.destY = mapY;
					}
				}

				engine.trfDensity[mapY/2][mapX/2] = z;
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

			if (engine.testBounds(tx, ty)
				&& roadTest(tx, ty))
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
		char c = engine.getTile(tx, ty);
		c &= LOMASK;

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
		int rdir = engine.PRNG.nextInt(4);

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
					positions.push(new Position(mapX, mapY));
				}

				return true;
			}
		}

		return false;
	}

	static class Position
	{
		int x;
		int y;
		Position(int x, int y)
		{
			this.x = x;
			this.y = y;
		}
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
			int tile = engine.getTile(mapX, mapY-1) & LOMASK;
			if (tile >= low && tile <= high)
				return true;
		}
		if (mapX + 1 < engine.getWidth())
		{
			int tile = engine.getTile(mapX + 1, mapY) & LOMASK;
			if (tile >= low && tile <= high)
				return true;
		}
		if (mapY + 1 < engine.getHeight())
		{
			int tile = engine.getTile(mapX, mapY + 1) & LOMASK;
			if (tile >= low && tile <= high)
				return true;
		}
		if (mapX > 0)
		{
			int tile = engine.getTile(mapX - 1, mapY) & LOMASK;
			if (tile >= low && tile <= high)
				return true;
		}
		return false;
	}
}
