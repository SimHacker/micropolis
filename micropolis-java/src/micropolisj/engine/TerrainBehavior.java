// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

import static micropolisj.engine.TileConstants.*;

class TerrainBehavior extends TileBehavior
{
	final B behavior;

	TerrainBehavior(Micropolis city, B behavior)
	{
		super(city);
		this.behavior = behavior;
	}

	static enum B
	{
		FIRE,
		FLOOD,
		RADIOACTIVE,
		ROAD,
		RAIL,
		EXPLOSION;
	}

	@Override
	public void apply()
	{
		switch (behavior) {
		case FIRE:
			doFire();
			return;
		case FLOOD:
			doFlood();
			return;
		case RADIOACTIVE:
			doRadioactiveTile();
			return;
		case ROAD:
			doRoad();
			return;
		case RAIL:
			doRail();
			return;
		case EXPLOSION:
			doExplosion();
			return;
		default:
			assert false;
		}
	}

	void doFire()
	{
		city.firePop++;

		// one in four times
		if (PRNG.nextInt(4) != 0) {
			return;
		}

		final int [] DX = { 0, 1, 0, -1 };
		final int [] DY = { -1, 0, 1, 0 };

		for (int dir = 0; dir < 4; dir++)
		{
			if (PRNG.nextInt(8) == 0)
			{
				int xtem = xpos + DX[dir];
				int ytem = ypos + DY[dir];
				if (!city.testBounds(xtem, ytem))
					continue;

				int c = city.getTile(xtem, ytem);
				if (isCombustible(c)) {
					if (isZoneCenter(c)) {
						city.killZone(xtem, ytem, c);
						if (c > IZB) { //explode
							city.makeExplosion(xtem, ytem);
						}
					}
					city.setTile(xtem, ytem, (char)(FIRE + PRNG.nextInt(4)));
				}
			}
		}

		int cov = city.getFireStationCoverage(xpos, ypos);
		int rate = cov > 100 ? 1 :
			cov > 20 ? 2 :
			cov != 0 ? 3 : 10;

		if (PRNG.nextInt(rate+1) == 0) {
			city.setTile(xpos, ypos, (char)(RUBBLE + PRNG.nextInt(4)));
		}
	}

	/**
	 * Called when the current tile is a flooding tile.
	 */
	void doFlood()
	{
		final int [] DX = { 0, 1, 0, -1 };
		final int [] DY = { -1, 0, 1, 0 };

		if (city.floodCnt != 0)
		{
			for (int z = 0; z < 4; z++)
			{
				if (PRNG.nextInt(8) == 0) {
					int xx = xpos + DX[z];
					int yy = ypos + DY[z];
					if (city.testBounds(xx, yy)) {
						int t = city.getTile(xx, yy);
						if (isCombustible(t)
							|| t == DIRT
							|| (t >= WOODS5 && t < FLOOD))
						{
							if (isZoneCenter(t)) {
								city.killZone(xx, yy, t);
							}
							city.setTile(xx, yy, (char)(FLOOD + PRNG.nextInt(3)));
						}
					}
				}
			}
		}
		else {
			if (PRNG.nextInt(16) == 0) {
				city.setTile(xpos, ypos, DIRT);
			}
		}
	}

	/**
	 * Called when the current tile is a radioactive tile.
	 */
	void doRadioactiveTile()
	{
		if (PRNG.nextInt(4096) == 0)
		{
			// radioactive decay
			city.setTile(xpos, ypos, DIRT);
		}
	}

	static int [] TRAFFIC_DENSITY_TAB = { ROADBASE, LTRFBASE, HTRFBASE };

	/**
	 * Called when the current tile is a road tile.
	 */
	void doRoad()
	{
		city.roadTotal++;

		if (city.roadEffect < 30)
		{
			// deteriorating roads
			if (PRNG.nextInt(512) == 0)
			{
				if (!isConductive(tile))
				{
					if (city.roadEffect < PRNG.nextInt(32))
					{
						if (isOverWater(tile))
							city.setTile(xpos, ypos, RIVER);
						else
							city.setTile(xpos, ypos, (char)(RUBBLE + PRNG.nextInt(4)));
						return;
					}
				}
			}
		}

		if (!isCombustible(tile)) //bridge
		{
			city.roadTotal += 4;
			if (doBridge())
				return;
		}

		int tden;
		if (tile < LTRFBASE)
			tden = 0;
		else if (tile < HTRFBASE)
			tden = 1;
		else {
			city.roadTotal++;
			tden = 2;
		}

		int trafficDensity = city.getTrafficDensity(xpos, ypos);
		int newLevel = trafficDensity < 64 ? 0 :
			trafficDensity < 192 ? 1 : 2;
		
		assert newLevel >= 0 && newLevel < TRAFFIC_DENSITY_TAB.length;

		if (tden != newLevel)
		{
			int z = ((tile - ROADBASE) & 15) + TRAFFIC_DENSITY_TAB[newLevel];
			city.setTile(xpos, ypos, (char) z);
		}
	}

	/**
	 * Called when the current tile is railroad.
	 */
	void doRail()
	{
		city.railTotal++;
		city.generateTrain(xpos, ypos);

		if (city.roadEffect < 30) { // deteriorating rail
			if (PRNG.nextInt(512) == 0) {
				if (!isConductive(tile)) {
					if (city.roadEffect < PRNG.nextInt(32)) {
						if (isOverWater(tile)) {
							city.setTile(xpos,ypos,RIVER);
						} else {
							city.setTile(xpos,ypos,(char)(RUBBLE + PRNG.nextInt(4)));
						}
					}
				}
			}
		}
	}

	/**
	 * Called when the current tile is a road bridge over water.
	 * Handles the draw bridge. For the draw bridge to appear,
	 * there must be a boat on the water, the boat must be
	 * within a certain distance of the bridge, it must be where
	 * the map generator placed 'channel' tiles (these are tiles
	 * that look just like regular river tiles but have a different
	 * numeric value), and you must be a little lucky.
	 *
	 * @return true if the draw bridge is open; false otherwise
	 */
	boolean doBridge()
	{
		final int HDx[] = { -2,  2, -2, -1,  0,  1,  2 };
		final int HDy[] = { -1, -1,  0,  0,  0,  0,  0 };
		final char HBRTAB[] = {
			HBRDG1,       HBRDG3,
			HBRDG0,       RIVER,
			BRWH,         RIVER,
			HBRDG2 };
		final char HBRTAB2[] = {
			RIVER,        RIVER,
			HBRIDGE,      HBRIDGE,
			HBRIDGE,      HBRIDGE,
			HBRIDGE };

		final int VDx[] = {  0,  1,  0,  0,  0,  0,  1 };
		final int VDy[] = { -2, -2, -1,  0,  1,  2,  2 };
		final char VBRTAB[] = {
			VBRDG0,       VBRDG1,
			RIVER,        BRWV,
			RIVER,        VBRDG2,
			VBRDG3 };
		final char VBRTAB2[] = {
			VBRIDGE,      RIVER,
			VBRIDGE,      VBRIDGE,
			VBRIDGE,      VBRIDGE,
			RIVER };

		if (tile == BRWV) {
			// vertical bridge, open
			if (PRNG.nextInt(4) == 0 && getBoatDis() > 340/16) {
				//close the bridge
				applyBridgeChange(VDx, VDy, VBRTAB, VBRTAB2);
			}
			return true;
		}
		else if (tile == BRWH) {
			// horizontal bridge, open
			if (PRNG.nextInt(4) == 0 && getBoatDis() > 340/16) {
				// close the bridge
				applyBridgeChange(HDx, HDy, HBRTAB, HBRTAB2);
			}
			return true;
		}

		if (getBoatDis() < 300/16 && PRNG.nextInt(8) == 0) {
			if ((tile & 1) != 0) {
				// vertical bridge
				if (xpos < city.getWidth()-1) {
					// look for CHANNEL tile to right of
					// bridge. the CHANNEL tiles are only
					// found in the very center of the
					// river
					if (city.getTile(xpos+1,ypos) == CHANNEL) {
						// vertical bridge, open it up
						applyBridgeChange(VDx, VDy, VBRTAB2, VBRTAB);
						return true;
					}
				}
				return false;
			}
			else {
				// horizontal bridge
				if (ypos > 0) {
					// look for CHANNEL tile just above
					// bridge. the CHANNEL tiles are only
					// found in the very center of the
					// river
					if (city.getTile(xpos, ypos-1) == CHANNEL) {
						// open it up
						applyBridgeChange(HDx, HDy, HBRTAB2, HBRTAB);
						return true;
					}
				}
				return false;
			}
		}

		return false;
	}

	/**
	 * Helper function for doBridge- it toggles the draw-bridge.
	 */
	private void applyBridgeChange(int [] Dx, int [] Dy, char [] fromTab, char [] toTab)
	{
	//FIXME- a closed bridge with traffic on it is not
	// correctly handled by this subroutine, because the
	// the tiles representing traffic on a bridge do not match
	// the expected tile values of fromTab

		for (int z = 0; z < 7; z++) {
			int x = xpos + Dx[z];
			int y = ypos + Dy[z];
			if (city.testBounds(x,y)) {
				if ((city.getTile(x,y) == fromTab[z]) ||
					(city.getTile(x,y) == CHANNEL)
					) {
					city.setTile(x, y, toTab[z]);
				}
			}
		}
	}

	/**
	 * Calculate how far away the boat currently is from the
	 * current tile.
	 */
	int getBoatDis()
	{
		int dist = 99999;
		for (Sprite s : city.sprites)
		{
			if (s.isVisible() && s.kind == SpriteKind.SHI)
			{
				int x = s.x / 16;
				int y = s.y / 16;
				int d = Math.abs(xpos-x) + Math.abs(ypos-y);
				dist = Math.min(d, dist);
			}
		}
		return dist;
	}

	void doExplosion()
	{
		// clear AniRubble
		city.setTile(xpos, ypos, (char)(RUBBLE + PRNG.nextInt(4)));
	}
}
