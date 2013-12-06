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
 * Contains the code for generating a random map terrain.
 */
public class MapGenerator
{
	Micropolis engine;
	char [][] map;
	Random PRNG;

	/**
	 * Three settings on whether to generate a new map as an island.
	 */
	static enum CreateIsland
	{
		NEVER,
		ALWAYS,
		SELDOM;   // seldom == 10% of the time
	}
	CreateIsland createIsland = CreateIsland.SELDOM;

	public MapGenerator(Micropolis engine)
	{
		assert engine != null;
		this.engine = engine;
		this.map = engine.map;
	}

	private int getWidth()
	{
		return map[0].length;
	}

	private int getHeight()
	{
		return map.length;
	}

	/**
	 * Generate a random map terrain.
	 */
	public void generateNewCity()
	{
		long r = Micropolis.DEFAULT_PRNG.nextLong();
		generateSomeCity(r);
	}

	public void generateSomeCity(long r)
	{
		generateMap(r);
		engine.fireWholeMapChanged();
	}

	/**
	 * Level for tree creation.
	 * If positive, this is (roughly) the number of trees to randomly place.
	 * If negative, then the number of trees is randomly chosen.
	 * If zero, then no trees are generated.
	 */
	int treeLevel = -1; //level for tree creation

	int curveLevel = -1; //level for river curviness; -1==auto, 0==none, >0==level

	int lakeLevel = -1; //level for lake creation; -1==auto, 0==none, >0==level

	void generateMap(long r)
	{
		PRNG = new Random(r);

		if (createIsland == CreateIsland.SELDOM)
		{
			if (PRNG.nextInt(100) < 10) //chance that island is generated
			{
				makeIsland();
				return;
			}
		}

		if (createIsland == CreateIsland.ALWAYS)
		{
			makeNakedIsland();
		}
		else
		{
			clearMap();
		}

		getRandStart();

		if (curveLevel != 0)
		{
			doRivers();
		}

		if (lakeLevel != 0)
		{
			makeLakes();
		}

		smoothRiver();

		if (treeLevel != 0)
		{
			doTrees();
		}
	}

	private void makeIsland()
	{
		makeNakedIsland();
		smoothRiver();
		doTrees();
	}

	private int erand(int limit)
	{
		return Math.min(
			PRNG.nextInt(limit),
			PRNG.nextInt(limit)
			);
	}

	private void makeNakedIsland()
	{
		final int ISLAND_RADIUS = 18;
		final int WORLD_X = getWidth();
		final int WORLD_Y = getHeight();

		for (int y = 0; y < WORLD_Y; y++)
		{
			for (int x = 0; x < WORLD_X; x++)
			{
				map[y][x] = RIVER;
			}
		}

		for (int y = 5; y < WORLD_Y - 5; y++)
		{
			for (int x = 5; x < WORLD_X - 5; x++)
			{
				map[y][x] = DIRT;
			}
		}

		for (int x = 0; x < WORLD_X - 5; x += 2)
		{
			mapX = x;
			mapY = erand(ISLAND_RADIUS+1);
			BRivPlop();
			mapY = (WORLD_Y - 10) - erand(ISLAND_RADIUS+1);
			BRivPlop();
			mapY = 0;
			SRivPlop();
			mapY = WORLD_Y - 6;
			SRivPlop();
		}

		for (int y = 0; y < WORLD_Y - 5; y += 2)
		{
			mapY = y;
			mapX = erand(ISLAND_RADIUS+1);
			BRivPlop();
			mapX = (WORLD_X - 10) - erand(ISLAND_RADIUS+1);
			BRivPlop();
			mapX = 0;
			SRivPlop();
			mapX = (WORLD_X - 6);
			SRivPlop();
		}
	}

	private void clearMap()
	{
		for (int y = 0; y < map.length; y++)
		{
			for (int x = 0; x < map[y].length; x++)
			{
				map[y][x] = DIRT;
			}
		}
	}

	int xStart;
	int yStart;
	int mapX;
	int mapY;
	int dir;
	int lastDir;

	private void getRandStart()
	{
		xStart = 40 + PRNG.nextInt(getWidth() - 79);
		yStart = 33 + PRNG.nextInt(getHeight() - 66);

		mapX = xStart;
		mapY = yStart;
	}

	private void makeLakes()
	{
		int lim1;
		if (lakeLevel < 0)
			lim1 = PRNG.nextInt(11);
		else
			lim1 = lakeLevel / 2;

		for (int t = 0; t < lim1; t++)
		{
			int x = PRNG.nextInt(getWidth() - 20) + 10;
			int y = PRNG.nextInt(getHeight() - 19) + 10;
			int lim2 = PRNG.nextInt(13) + 2;

			for (int z = 0; z < lim2; z++)
			{
				mapX = x - 6 + PRNG.nextInt(13);
				mapY = y - 6 + PRNG.nextInt(13);

				if (PRNG.nextInt(5) != 0)
					SRivPlop();
				else
					BRivPlop();
			}
		}
	}

	private void doRivers()
	{
		dir = lastDir = PRNG.nextInt(4);
		doBRiv();

		mapX = xStart;
		mapY = yStart;
		dir = lastDir = lastDir ^ 4;
		doBRiv();

		mapX = xStart;
		mapY = yStart;
		lastDir = PRNG.nextInt(4);
		doSRiv();
	}

	private void doBRiv()
	{
		int r1, r2;
		if (curveLevel < 0)
		{
			r1 = 100;
			r2 = 200;
		}
		else
		{
			r1 = curveLevel + 10;
			r2 = curveLevel + 100;
		}

		while (engine.testBounds(mapX + 4, mapY + 4))
		{
			BRivPlop();
			if (PRNG.nextInt(r1+1) < 10)
			{
				dir = lastDir;
			}
			else
			{
				if (PRNG.nextInt(r2+1) > 90)
				{
					dir++;
				}
				if (PRNG.nextInt(r2+1) > 90)
				{
					dir--;
				}
			}
			moveMap(dir);
		}
	}

	private void doSRiv()
	{
		int r1, r2;
		if (curveLevel < 0)
		{
			r1 = 100;
			r2 = 200;
		}
		else
		{
			r1 = curveLevel + 10;
			r2 = curveLevel + 100;
		}

		while (engine.testBounds(mapX + 3, mapY + 3))
		{
			SRivPlop();
			if (PRNG.nextInt(r1+1) < 10)
			{
				dir = lastDir;
			}
			else
			{
				if (PRNG.nextInt(r2+1) > 90)
				{
					dir++;
				}
				if (PRNG.nextInt(r2+1) > 90)
				{
					dir--;
				}
			}
			moveMap(dir);
		}
	}

	static final char [][] BRMatrix = new char[][] {
			{ 0, 0, 0, 3, 3, 3, 0, 0, 0 },
			{ 0, 0, 3, 2, 2, 2, 3, 0, 0 },
			{ 0, 3, 2, 2, 2, 2, 2, 3, 0 },
			{ 3, 2, 2, 2, 2, 2, 2, 2, 3 },
			{ 3, 2, 2, 2, 4, 2, 2, 2, 3 },
			{ 3, 2, 2, 2, 2, 2, 2, 2, 3 },
			{ 0, 3, 2, 2, 2, 2, 2, 3, 0 },
			{ 0, 0, 3, 2, 2, 2, 3, 0, 0 },
			{ 0, 0, 0, 3, 3, 3, 0, 0, 0 }
			};

	private void BRivPlop()
	{
		for (int x = 0; x < 9; x++)
		{
			for (int y = 0; y < 9; y++)
			{
				putOnMap(BRMatrix[y][x], x, y);
			}
		}
	}

	static final char [][] SRMatrix = new char[][] {
			{ 0, 0, 3, 3, 0, 0 },
			{ 0, 3, 2, 2, 3, 0 },
			{ 3, 2, 2, 2, 2, 3 },
			{ 3, 2, 2, 2, 2, 3 },
			{ 0, 3, 2, 2, 3, 0 },
			{ 0, 0, 3, 3, 0, 0 }
			};

	private void SRivPlop()
	{
		for (int x = 0; x < 6; x++)
		{
			for (int y = 0; y < 6; y++)
			{
				putOnMap(SRMatrix[y][x], x, y);
			}
		}
	}

	private void putOnMap(char mapChar, int xoff, int yoff)
	{
		if (mapChar == 0)
			return;

		int xloc = mapX + xoff;
		int yloc = mapY + yoff;

		if (!engine.testBounds(xloc, yloc))
			return;

		char tmp = map[yloc][xloc];
		if (tmp != DIRT)
		{
			tmp &= LOMASK;
			if (tmp == RIVER && mapChar != CHANNEL)
				return;
			if (tmp == CHANNEL)
				return;
		}
		map[yloc][xloc] = mapChar;
	}

	static final char [] REdTab = new char[] {
		RIVEDGE + 8, RIVEDGE + 8, RIVEDGE + 12, RIVEDGE + 10,
		RIVEDGE + 0, RIVER,       RIVEDGE + 14, RIVEDGE + 12,
		RIVEDGE + 4, RIVEDGE + 6, RIVER,        RIVEDGE + 8,
		RIVEDGE + 2, RIVEDGE + 4, RIVEDGE + 0,  RIVER
		};

	private void smoothRiver()
	{
		for (int mapY = 0; mapY < map.length; mapY++)
		{
			for (int mapX = 0; mapX < map[mapY].length; mapX++)
			{
				if (map[mapY][mapX] == REDGE)
				{
					int bitindex = 0;

					for (int z = 0; z < 4; z++)
					{
						bitindex <<= 1;
						int xtem = mapX + DX[z];
						int ytem = mapY + DY[z];
						if (engine.testBounds(xtem, ytem) &&
							((map[ytem][xtem] & LOMASK) != DIRT) &&
							(((map[ytem][xtem] & LOMASK) < WOODS_LOW) ||
							((map[ytem][xtem] & LOMASK) > WOODS_HIGH)))
						{
							bitindex |= 1;
						}
					}

					char temp = REdTab[bitindex & 15];
					if ((temp != RIVER) && PRNG.nextInt(2) != 0)
						temp++;
					map[mapY][mapX] = temp;
				}
			}
		}
	}

	private void doTrees()
	{
		int amount;

		if (treeLevel < 0)
		{
			amount = PRNG.nextInt(101) + 50;
		}
		else
		{
			amount = treeLevel + 3;
		}

		for (int x = 0; x < amount; x++)
		{
			int xloc = PRNG.nextInt(getWidth());
			int yloc = PRNG.nextInt(getHeight());
			treeSplash(xloc, yloc);
		}

		smoothTrees();
		smoothTrees();
	}

	private void treeSplash(int xloc, int yloc)
	{
		int dis;
		if (treeLevel < 0)
		{
			dis = PRNG.nextInt(151) + 50;
		}
		else
		{
			dis = PRNG.nextInt(101 + (treeLevel*2)) + 50;
		}

		mapX = xloc;
		mapY = yloc;

		for (int z = 0; z < dis; z++)
		{
			int dir = PRNG.nextInt(8);
			moveMap(dir);

			if (!engine.testBounds(mapX, mapY))
				return;

			if ((map[mapY][mapX] & LOMASK) == DIRT)
			{
				map[mapY][mapX] = WOODS;
			}
		}
	}

	static final int [] DIRECTION_TABX = new int[] {  0,  1,  1,  1,  0, -1, -1, -1 };
	static final int [] DIRECTION_TABY = new int[] { -1, -1,  0,  1,  1,  1,  0, -1 };
	private void moveMap(int dir)
	{
		dir = dir & 7;
		mapX += DIRECTION_TABX[dir];
		mapY += DIRECTION_TABY[dir];
	}

	static final int [] DX = new int[] { -1, 0, 1, 0 };
	static final int [] DY = new int[] { 0, 1, 0, -1 };
	static final char [] TEdTab = new char[] {
			0, 0, 0, 34,
			0, 0, 36, 35,
			0, 32, 0, 33,
			30, 31, 29, 37
		};

	private void smoothTrees()
	{
		for (int mapY = 0; mapY < map.length; mapY++)
		{
			for (int mapX = 0; mapX < map[mapY].length; mapX++)
			{
				if (isTree(map[mapY][mapX]))
				{
					int bitindex = 0;
					for (int z = 0; z < 4; z++)
					{
						bitindex <<= 1;
						int xtem = mapX + DX[z];
						int ytem = mapY + DY[z];
						if (engine.testBounds(xtem, ytem) &&
							isTree(map[ytem][xtem]))
						{
							bitindex |= 1;
						}
					}
					char temp = TEdTab[bitindex & 15];
					if (temp != 0)
					{
						if (temp != WOODS)
						{
							if (((mapX + mapY) & 1) != 0)
							{
								temp -= 8;
							}
						}
						map[mapY][mapX] = temp;
					}
					else
					{
						map[mapY][mapX] = temp;
					}
				}
			}
		}
	}

}
