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
import static micropolisj.engine.Micropolis.ZoneType;
import static micropolisj.engine.Animate.Smoke;

/**
 * Process individual tiles of the map for each cycle.
 * In each sim cycle each tile will get activated, and this
 * class contains the activation code.
 */
class MapScanner
{
	final Micropolis city;
	final Random PRNG;
	int xpos;
	int ypos;
	char cchr;
	char cchr9;

	MapScanner(Micropolis city)
	{
		this.city = city;
		this.PRNG = city.PRNG;
	}

	/**
	 * Activate the tile identified by xpos and ypos properties.
	 */
	public void scanTile()
	{
		cchr9 = (char) (cchr & LOMASK);

		if (cchr9 >= FLOOD)
		{
			if (cchr9 < ROADBASE)
			{
				if (cchr9 >= FIREBASE)
				{
					city.firePop++;
					if (PRNG.nextInt(4) == 0)
					{
						// one in four times
						doFire();
					}
					return;
				}

				if (cchr9 < RADTILE)
				{
					doFlood();
				}
				else
				{
					doRadioactiveTile();
				}
				return;
			}

			if (city.newPower && ((cchr & CONDBIT) != 0))
			{
				setZonePower();
			}

			if (cchr9 >= ROADBASE && cchr9 < POWERBASE)
			{
				doRoad();
				return;
			}

			if ((cchr & ZONEBIT) != 0)
			{
				doZone();
				return;
			}

			if (cchr9 >= RAILBASE && cchr9 < RESBASE)
			{
				doRail();
				return;
			}

			if (cchr9 >= SOMETINYEXP && cchr9 <= LASTTINYEXP)
			{
				// clear AniRubble
				city.setTile(xpos, ypos, (char)(RUBBLE + PRNG.nextInt(4) + BULLBIT));
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
				if ((cchr & CONDBIT) == 0)
				{
					if (city.roadEffect < PRNG.nextInt(32))
					{
						if ((cchr & 15) < 2 || (cchr & 15) == 15)
							city.setTile(xpos, ypos, RIVER);
						else
							city.setTile(xpos, ypos, (char)(RUBBLE + PRNG.nextInt(4) + BULLBIT));
						return;
					}
				}
			}
		}

		if ((cchr & BURNBIT) == 0) //bridge
		{
			city.roadTotal += 4;
			if (doBridge())
				return;
		}

		int tden;
		if ((cchr & LOMASK) < LTRFBASE)
			tden = 0;
		else if ((cchr & LOMASK) < HTRFBASE)
			tden = 1;
		else {
			city.roadTotal++;
			tden = 2;
		}

		int trafficDensity = city.trfDensity[ypos/2][xpos/2];
		int newLevel = trafficDensity < 64 ? 0 :
			trafficDensity < 192 ? 1 : 2;
		
		assert newLevel >= 0 && newLevel < TRAFFIC_DENSITY_TAB.length;

		if (tden != newLevel)
		{
			int z = (((cchr & LOMASK) - ROADBASE) & 15) + TRAFFIC_DENSITY_TAB[newLevel];
			z += cchr & (ALLBITS - ANIMBIT);

			if (newLevel != 0)
				z |= ANIMBIT;

			city.setTile(xpos, ypos, (char) z);
		}
	}

	/**
	 * Called when the current tile is an active fire.
	 */
	void doFire()
	{
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

				int c = city.map[ytem][xtem];
				if ((c & BURNBIT) != 0) {
					if ((c & ZONEBIT) != 0) {
						city.fireZone(xtem, ytem, c);
						if ((c & LOMASK) > IZB) { //explode
							city.makeExplosion(xtem, ytem);
						}
					}
					city.setTile(xtem, ytem, (char)(FIRE + PRNG.nextInt(4) + ANIMBIT));
				}
			}
		}

		int cov = city.fireRate[ypos/8][xpos/8];  //fire station coverage
		int rate = cov > 100 ? 1 :
			cov > 20 ? 2 :
			cov != 0 ? 3 : 10;

		if (PRNG.nextInt(rate+1) == 0) {
			city.setTile(xpos, ypos, (char)(RUBBLE + PRNG.nextInt(4) + BULLBIT));
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
						int c = city.getTile(xx, yy);
						int t = c & LOMASK;
						if (((c & BURNBIT) != 0) || c == DIRT ||
							(t >= WOODS5 && t < FLOOD))
						{
							if ((c & ZONEBIT) != 0) {
								city.fireZone(xx, yy, c);
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
	 * Called when the current tile is railroad.
	 */
	void doRail()
	{
		city.railTotal++;
		city.generateTrain(xpos, ypos);

		if (city.roadEffect < 30) { // deteriorating rail
			if (PRNG.nextInt(512) == 0) {
				if ((cchr & CONDBIT) == 0) {
					if (city.roadEffect < PRNG.nextInt(32)) {
						if (cchr9 < RAILBASE+2) {
							city.setTile(xpos,ypos,RIVER);
						} else {
							city.setTile(xpos,ypos,(char)(RUBBLE + PRNG.nextInt(4)+BULLBIT));
						}
					}
				}
			}
		}
	}

	/**
	 * Called when the current tile is a road bridge over water.
	 */
	boolean doBridge()
	{
		final int HDx[] = { -2,  2, -2, -1,  0,  1,  2 };
		final int HDy[] = { -1, -1,  0,  0,  0,  0,  0 };
		final char HBRTAB[] = {
			HBRDG1 | BULLBIT,       HBRDG3 | BULLBIT,
			HBRDG0 | BULLBIT,       RIVER,
			BRWH | BULLBIT,         RIVER,
			HBRDG2 | BULLBIT };
		final char HBRTAB2[] = {
			RIVER,                  RIVER,
			HBRIDGE | BULLBIT,      HBRIDGE | BULLBIT,
			HBRIDGE | BULLBIT,      HBRIDGE | BULLBIT,
			HBRIDGE | BULLBIT };

		final int VDx[] = {  0,  1,  0,  0,  0,  0,  1 };
		final int VDy[] = { -2, -2, -1,  0,  1,  2,  2 };
		final char VBRTAB[] = {
			VBRDG0 | BULLBIT,       VBRDG1 | BULLBIT,
			RIVER,                  BRWV | BULLBIT,
			RIVER,                  VBRDG2 | BULLBIT,
			VBRDG3 | BULLBIT };
		final char VBRTAB2[] = {
			VBRIDGE | BULLBIT,      RIVER,
			VBRIDGE | BULLBIT,      VBRIDGE | BULLBIT,
			VBRIDGE | BULLBIT,      VBRIDGE | BULLBIT,
			RIVER };

		if (cchr9 == BRWV) {
			// vertical bridge, open
			if (PRNG.nextInt(4) == 0 && getBoatDis() > 340/16) {
				//close the bridge
				applyBridgeChange(VDx, VDy, VBRTAB, VBRTAB2);
			}
			return true;
		}
		else if (cchr9 == BRWH) {
			// horizontal bridge, open
			if (PRNG.nextInt(4) == 0 && getBoatDis() > 340/16) {
				// close the bridge
				applyBridgeChange(HDx, HDy, HBRTAB, HBRTAB2);
			}
			return true;
		}

		if (getBoatDis() < 300/16 && PRNG.nextInt(8) == 0) {
			if ((cchr & 1) != 0) {
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
		for (int z = 0; z < 7; z++) {
			int x = xpos + Dx[z];
			int y = ypos + Dy[z];
			if (city.testBounds(x,y)) {
				if ((city.map[y][x] & LOMASK) == (fromTab[z] & LOMASK) ||
					(city.map[y][x] == CHANNEL)
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

	/**
	 * Called when the current tile is the key tile of an airport.
	 */
	void doAirport()
	{
		if (PRNG.nextInt(6) == 0) {
			city.generatePlane(xpos, ypos);
		}

		if (PRNG.nextInt(13) == 0) {
			city.generateCopter(xpos, ypos);
		}
	}

	/**
	 * Called when the current tile is the key tile of any zone.
	 */
	void doZone()
	{
		// set power bit in map, from powermap
		boolean zonePwrFlag = setZonePower();

		if (zonePwrFlag)
		{
			city.poweredZoneCount++;
		}
		else
		{
			city.unpoweredZoneCount++;
		}

		if (cchr9 > PORTBASE)
		{
			doSpecialZone(zonePwrFlag);
			return;
		}

		if (cchr9 < HOSPITAL)
		{
			doResidential(zonePwrFlag);
			return;
		}

		if (cchr9 < COMBASE)
		{
			doHospitalChurch(zonePwrFlag);
			return;
		}

		if (cchr9 < INDBASE)
		{
			doCommercial(zonePwrFlag);
			return;
		}

		doIndustrial(zonePwrFlag);
		return;
	}

	boolean setZonePower()
	{
		// refresh cchr, cchr9, since this can get called after the
		// tile's been changed
		cchr = city.map[ypos][xpos];
		cchr9 = (char) (cchr & LOMASK);

		if (cchr9 == NUCLEAR ||
			cchr9 == POWERPLANT ||
			city.hasPower(xpos,ypos))
		{
			city.setTile(xpos, ypos, (char) (cchr | PWRBIT));
			return true;
		}
		else
		{
			city.setTile(xpos, ypos, (char) (cchr & (~PWRBIT)));
			return false;
		}
	}

	/**
	 * Place a 3x3 zone on to the map.
	 */
	boolean zonePlop(int xpos, int ypos, int base)
	{
		if (!city.testBounds(xpos-1, ypos-1))
			return false;
		if (!city.testBounds(xpos+1, ypos+1))
			return false;

		for (int y = ypos-1; y <= ypos+1; y++)
		{
			for (int x = xpos-1; x <= xpos+1; x++)
			{
				char c = (char)(city.map[y][x] & LOMASK);
				if (c >= FLOOD && c < ROADBASE)
					return false;
			}
		}

		for (int y = ypos-1; y <= ypos+1; y++)
		{
			for (int x = xpos-1; x <= xpos+1; x++)
			{
				city.setTile(x, y, (char)(base | BNCNBIT | (x == xpos && y == ypos ? ZONEBIT + BULLBIT : 0)));
				base++;
			}
		}

		setZonePower();
		return true;
	}

	/**
	 * Called when the current tile is the key tile of a "special" zone.
	 * @param powerOn indicates whether the building has power
	 */
	void doSpecialZone(boolean powerOn)
	{
		switch (cchr9)
		{
		case POWERPLANT:
			city.coalCount++;
			if ((city.cityTime % 8) == 0) {
				repairZone(xpos, ypos, POWERPLANT, 4);
			}

			city.powerPlants.add(new CityLocation(xpos,ypos));
			coalSmoke(xpos, ypos);
			return;

		case NUCLEAR:
			if (!city.noDisasters && PRNG.nextInt(city.MltdwnTab[city.gameLevel]+1) == 0) {
				city.doMeltdown(xpos, ypos);
				return;
			}

			city.nuclearCount++;
			if ((city.cityTime % 8) == 0) {
				repairZone(xpos, ypos, NUCLEAR, 4);
			}

			city.powerPlants.add(new CityLocation(xpos, ypos));
			return;

		case FIRESTATION:
		{
			city.fireStationCount++;
			if ((city.cityTime % 8) == 0) {
				repairZone(xpos, ypos, FIRESTATION, 3);
			}

			int z;
			if (powerOn) {
				z = city.fireEffect;  //if powered, get effect
			} else {
				z = city.fireEffect/2; // from the funding ratio
			}

			city.traffic.mapX = xpos;
			city.traffic.mapY = ypos;
			if (!city.traffic.findPerimeterRoad()) {
				z /= 2;
			}

			city.fireStMap[ypos/8][xpos/8] += z;
			return;
		}

		case POLICESTATION:
		{
			city.policeCount++;
			if ((city.cityTime % 8) == 0) {
				repairZone(xpos, ypos, POLICESTATION, 3);
			}

			int z;
			if (powerOn) {
				z = city.policeEffect;
			} else {
				z = city.policeEffect / 2;
			}

			city.traffic.mapX = xpos;
			city.traffic.mapY = ypos;
			if (!city.traffic.findPerimeterRoad()) {
				z /= 2;
			}

			city.policeMap[ypos/8][xpos/8] += z;
			return;
		}

		case STADIUM:
			city.stadiumCount++;
			if ((city.cityTime % 16) == 0) {
				repairZone(xpos, ypos, STADIUM, 4);
			}

			if (powerOn)
			{
				if (((city.cityTime + xpos + ypos) % 32) == 0) {
					drawStadium(xpos, ypos, FULLSTADIUM);
					city.setTile(xpos+1,ypos, (char)(FOOTBALLGAME1 | ANIMBIT));
					city.setTile(xpos+1,ypos+1,(char)(FOOTBALLGAME2 | ANIMBIT));
				}
			}
			return;

		case FULLSTADIUM:
			city.stadiumCount++;
			if (((city.cityTime + xpos + ypos) % 8) == 0) {
				drawStadium(xpos, ypos, STADIUM);
			}
			return;

		case AIRPORT:
			city.airportCount++;
			if ((city.cityTime % 8) == 0) {
				repairZone(xpos, ypos, AIRPORT, 6);
			}

			if (powerOn)
			{
				if ((city.map[ypos-1][xpos+1] & LOMASK) == RADAR) {
					city.setTile(xpos+1,ypos-1, (char)
					(RADAR + ANIMBIT + CONDBIT + BURNBIT)
					);
				}
			}
			else
			{
				city.setTile(xpos+1,ypos-1,(char)(RADAR + CONDBIT + BURNBIT));
			}

			if (powerOn) {
				doAirport();
			}
			return;

		case PORT:
			city.seaportCount++;
			if ((city.cityTime % 16) == 0) {
				repairZone(xpos, ypos, PORT, 4);
			}

			if (powerOn && !city.hasSprite(SpriteKind.SHI)) {
				city.generateShip();
			}
			return;

		default:
			// should not happen
			assert false;
		}
	}

	/**
	 * Place hospital or church if needed.
	 */
	void makeHospital()
	{
		if (city.needHospital > 0)
		{
			zonePlop(xpos, ypos, HOSPITAL - 4);
			city.needHospital = 0;
		}

		if (city.needChurch > 0)
		{
			zonePlop(xpos, ypos, CHURCH - 4);
			city.needChurch = 0;
		}
	}

	/**
	 * Called when the current tile is the key tile of a
	 * hospital or church.
	 * @param powerOn indicates whether the building has power
	 */
	void doHospitalChurch(boolean powerOn)
	{
		if (cchr9 == HOSPITAL)
		{
			city.hospitalCount++;

			if (city.cityTime % 16 == 0)
			{
				repairZone(xpos, ypos, HOSPITAL, 3);
			}
			if (city.needHospital == -1)  //too many hospitals
			{
				if (PRNG.nextInt(21) == 0)
				{
					zonePlop(xpos, ypos, RESBASE);
				}
			}
		}
		else if (cchr9 == CHURCH)
		{
			city.churchCount++;

			if (city.cityTime % 16 == 0)
			{
				repairZone(xpos, ypos, CHURCH, 3);
			}
			if (city.needChurch == -1) //too many churches
			{
				if (PRNG.nextInt(21) == 0)
				{
					zonePlop(xpos, ypos, RESBASE);
				}
			}
		}
	}

	void repairZone(int xpos, int ypos, char zoneCenter, int zoneSize)
	{
		int cnt=0;
		for (int y = 0; y < zoneSize; y++)
		{
			for (int x = 0; x < zoneSize; x++)
			{
				int xx = xpos - 1 + x;
				int yy = ypos - 1 + y;

				cnt++;

				if (city.testBounds(xx, yy))
				{
					int thCh = city.map[yy][xx];
					if ((thCh & ZONEBIT) != 0)
						continue;

					if ((thCh & ANIMBIT) != 0)
						continue;

					thCh &= LOMASK;
					if (thCh < RUBBLE || thCh >= ROADBASE)
					{  //not rubble, radiactive, on fire or flooded

						city.setTile(xx,yy,(char)
						(zoneCenter-2-zoneSize+cnt+CONDBIT+BURNBIT)
						);
					}
				}
			}
		}
	}

	/**
	 * Called when the current tile is the key tile of a commercial
	 * zone.
	 * @param powerOn indicates whether the building has power
	 */
	void doCommercial(boolean powerOn)
	{
		city.comZoneCount++;

		int tpop = city.commercialZonePop(cchr9);
		city.comPop += tpop;

		int trafficGood;
		if (tpop > PRNG.nextInt(6))
		{
			trafficGood = city.makeTraffic(xpos, ypos, ZoneType.COMMERCIAL);
		}
		else
		{
			trafficGood = 1;
		}

		if (trafficGood == -1)
		{
			int value = getCRValue(xpos, ypos);
			doCommercialOut(xpos, ypos, tpop, value);
			return;
		}

		if (PRNG.nextInt(8) == 0)
		{
			int locValve = evalCommercial(xpos, ypos, trafficGood);
			int zscore = city.comValve + locValve;

			if (!powerOn)
				zscore = -500;

			if (trafficGood != 0 &&
				zscore > -350 &&
				zscore - 26380 > (PRNG.nextInt(0x10000)-0x8000))
			{
				int value = getCRValue(xpos, ypos);
				doCommercialIn(xpos, ypos, tpop, value);
				return;
			}

			if (zscore < 350 && zscore + 26380 < (PRNG.nextInt(0x10000)-0x8000))
			{
				int value = getCRValue(xpos, ypos);
				doCommercialOut(xpos, ypos, tpop, value);
			}
		}
	}

	/*
	 * @param powerOn indicates whether the building has power
	 */
	void setSmoke(int xpos, int ypos, boolean powerOn)
	{
		int cchr9 = city.map[ypos][xpos] & LOMASK;

		if (cchr9 < IZB)
			return;

		int z = ((cchr9 - IZB) / 8) % 8;
		if (Smoke.AniThis[z] != 0)
		{
			int xx = xpos + Smoke.DX1[z];
			int yy = ypos + Smoke.DY1[z];

			if (city.testBounds(xx, yy))
			{
				int t = city.map[yy][xx] & LOMASK;

				if (powerOn) {
					if (t == Smoke.AniTabC[z]) //expected non-animated tile
					{
						city.setTile(xx,yy,(char)(Smoke.ASCBIT | (SMOKEBASE + Smoke.AniTabA[z])));
					}
				}
				else {
					if (t > Smoke.AniTabC[z]) {
						city.setTile(xx,yy,(char)(Smoke.REGBIT | Smoke.AniTabC[z]));
					}
				}
			}

			xx = xpos + Smoke.DX2[z];
			yy = ypos + Smoke.DY2[z];
			if (city.testBounds(xx, yy) && !(Smoke.DX1[z] == Smoke.DX2[z] && Smoke.DY1[z] == Smoke.DY2[z]))
			{
				int t = city.map[yy][xx] & LOMASK;
				if (powerOn) {
					if (t == Smoke.AniTabD[z]) {
						city.setTile(xx,yy,(char)(Smoke.ASCBIT | (SMOKEBASE + Smoke.AniTabB[z])));
					}
				}
				else {
					if (t > Smoke.AniTabD[z]) {
						city.setTile(xx,yy,(char)(Smoke.REGBIT | Smoke.AniTabD[z]));
					}
				}
			}
		}
	}

	/**
	 * Called when the current tile is the key tile of an
	 * industrial zone.
	 * @param powerOn indicates whether the building has power
	 */
	void doIndustrial(boolean powerOn)
	{
		city.indZoneCount++;
		setSmoke(xpos, ypos, powerOn);

		int tpop = city.industrialZonePop(cchr9);
		city.indPop += tpop;

		int trafficGood;
		if (tpop > PRNG.nextInt(6))
		{
			trafficGood = city.makeTraffic(xpos, ypos, ZoneType.INDUSTRIAL);
		}
		else
		{
			trafficGood = 1;
		}

		if (trafficGood == -1)
		{
			doIndustrialOut(xpos, ypos, tpop, PRNG.nextInt(2));
			return;
		}

		if (PRNG.nextInt(8) == 0)
		{
			int locValve = evalIndustrial(xpos, ypos, trafficGood);
			int zscore = city.indValve + locValve;

			if (!powerOn)
				zscore = -500;

			if (zscore > -350 &&
				zscore - 26380 > (PRNG.nextInt(0x10000)-0x8000))
			{
				int value = PRNG.nextInt(2);
				doIndustrialIn(xpos, ypos, tpop, value);
				return;
			}

			if (zscore < 350 && zscore + 26380 < (PRNG.nextInt(0x10000)-0x8000))
			{
				int value = PRNG.nextInt(2);
				doIndustrialOut(xpos, ypos, tpop, value);
			}
		}
	}

	/**
	 * Called when the current tile is the key tile of a
	 * residential zone.
	 * @param powerOn indicates whether the building has power
	 */
	void doResidential(boolean powerOn)
	{
		city.resZoneCount++;

		int tpop; //population of this zone
		if (cchr9 == FREEZ)
		{
			tpop = city.doFreePop(xpos, ypos);
		}
		else
		{
			tpop = city.residentialZonePop(cchr9);
		}

		city.resPop += tpop;

		int trafficGood;
		if (tpop > PRNG.nextInt(36))
		{
			trafficGood = city.makeTraffic(xpos, ypos, ZoneType.RESIDENTIAL);
		}
		else
		{
			trafficGood = 1;
		}

		if (trafficGood == -1)
		{
			int value = getCRValue(xpos, ypos);
			doResidentialOut(xpos, ypos, tpop, value);
			return;
		}

		if (cchr9 == FREEZ || PRNG.nextInt(8) == 0)
		{
			int locValve = evalResidential(xpos, ypos, trafficGood);
			int zscore = city.resValve + locValve;

			if (!powerOn)
				zscore = -500;

			if (zscore > -350 && zscore - 26380 > (PRNG.nextInt(0x10000)-0x8000))
			{
				if (tpop == 0 && PRNG.nextInt(4) == 0)
				{
					makeHospital();
					return;
				}

				int value = getCRValue(xpos, ypos);
				doResidentialIn(xpos, ypos, tpop, value);
				return;
			}

			if (zscore < 350 && zscore + 26380 < (PRNG.nextInt(0x10000)-0x8000))
			{
				int value = getCRValue(xpos, ypos);
				doResidentialOut(xpos, ypos, tpop, value);
			}
		}
	}

	int evalLot(int x, int y)
	{
		// test for clear lot
		int tmp = city.getTile(x,y) & LOMASK;

		if (tmp != DIRT && (tmp < RESBASE || tmp > RESBASE+8))
		{
			return -1;
		}

		int score = 1;

		final int [] DX = { 0, 1, 0, -1 };
		final int [] DY = { -1, 0, 1, 0 };
		for (int z = 0; z < 4; z++)
		{
			int xx = x + DX[z];
			int yy = y + DY[z];

			if (city.testBounds(xx, yy) &&
				city.map[yy][xx] != DIRT &&
				((city.map[yy][xx] & LOMASK) <= LASTROAD)) //look for road
			{
				score++;
			}
		}

		return score;
	}

	private void buildHouse(int xpos, int ypos, int value)
	{
		assert value >= 0 && value <= 3;

		final int [] ZeX = { 0, -1, 0, 1, -1, 1, -1, 0, 1 };
		final int [] ZeY = { 0, -1, -1, -1, 0, 0, 1, 1, 1 };

		int bestLoc = 0;
		int hscore = 0;

		for (int z = 1; z < 9; z++)
		{
			int xx = xpos + ZeX[z];
			int yy = ypos + ZeY[z];

			if (city.testBounds(xx, yy))
			{
				int score = evalLot(xx, yy);

				if (score != 0)
				{
					if (score > hscore)
					{
						hscore = score;
						bestLoc = z;
					}

					if ((score == hscore) && PRNG.nextInt(8) == 0)
					{
						bestLoc = z;
					}
				}
			}
		}

		if (bestLoc != 0)
		{
			int xx = xpos + ZeX[bestLoc];
			int yy = ypos + ZeY[bestLoc];
			int houseNumber = value * 3 + PRNG.nextInt(3);
			assert houseNumber >= 0 && houseNumber < 12;

			assert city.testBounds(xx, yy);
			city.setTile(xx, yy, (char)(HOUSE + houseNumber + BLBNCNBIT));
		}
	}

	private void doCommercialIn(int xpos, int ypos, int pop, int value)
	{
		int z = city.landValueMem[ypos/2][xpos/2] / 32;
		if (pop > z)
			return;

		if (pop < 5)
		{
			comPlop(xpos, ypos, pop, value);
			incrementROG(xpos, ypos, 8);
		}
	}

	private void doIndustrialIn(int xpos, int ypos, int pop, int value)
	{
		if (pop < 4)
		{
			indPlop(xpos, ypos, pop, value);
			incrementROG(xpos, ypos, 8);
		}
	}

	private void doResidentialIn(int xpos, int ypos, int pop, int value)
	{
		assert value >= 0 && value <= 3;

		int z = city.pollutionMem[ypos/2][xpos/2];
		if (z > 128)
			return;

		char cchr9 = (char)(city.map[ypos][xpos] & LOMASK);
		if (cchr9 == FREEZ)
		{
			if (pop < 8)
			{
				buildHouse(xpos, ypos, value);
				incrementROG(xpos, ypos, 1);
				return;
			}

			if (city.getPopulationDensity(xpos, ypos) > 64)
			{
				residentialPlop(xpos, ypos, 0, value);
				incrementROG(xpos, ypos, 8);
				return;
			}
			return;
		}

		if (pop < 40)
		{
			residentialPlop(xpos, ypos, pop / 8 - 1, value);
			incrementROG(xpos, ypos, 8);
		}
	}

	void comPlop(int xpos, int ypos, int density, int value)
	{
		int base = (value * 5 + density) * 9 + CZB - 4;
		zonePlop(xpos, ypos, base);
	}

	void indPlop(int xpos, int ypos, int density, int value)
	{
		int base = (value * 4 + density) * 9 + (IZB - 4);
		zonePlop(xpos, ypos, base);
	}

	void residentialPlop(int xpos, int ypos, int density, int value)
	{
		int base = (value * 4 + density) * 9 + RZB - 4;
		zonePlop(xpos, ypos, base);
	}

	private void doCommercialOut(int xpos, int ypos, int pop, int value)
	{
		if (pop > 1)
		{
			comPlop(xpos, ypos, pop-2, value);
			incrementROG(xpos, ypos, -8);
		}
		else if (pop == 1)
		{
			zonePlop(xpos, ypos, COMBASE);
			incrementROG(xpos, ypos, -8);
		}
	}

	private void doIndustrialOut(int xpos, int ypos, int pop, int value)
	{
		if (pop > 1)
		{
			indPlop(xpos, ypos, pop-2, value);
			incrementROG(xpos, ypos, -8);
		}
		else if (pop == 1)
		{
			zonePlop(xpos, ypos, INDCLR-4);
			incrementROG(xpos, ypos, -8);
		}
	}

	private void doResidentialOut(int xpos, int ypos, int pop, int value)
	{
		assert value >= 0 && value < 4;

		final char [] Brdr = { 0, 3, 6, 1, 4, 7, 2, 5, 8 };

		if (pop == 0)
			return;

		if (pop > 16)
		{
			// downgrade to a lower-density full-size residential zone
			residentialPlop(xpos, ypos, (pop-24) / 8, value);
			incrementROG(xpos, ypos, -8);
			return;
		}

		if (pop == 16)
		{
			// downgrade from full-size zone to 8 little houses

			city.setTile(xpos, ypos, (char)(FREEZ | BLBNCNBIT | ZONEBIT));
			for (int x = xpos-1; x <= xpos+1; x++)
			{
				for (int y = ypos-1; y <= ypos+1; y++)
				{
					if (city.testBounds(x,y))
					{
						if (!(x == xpos && y == ypos))
						{
							// pick a random small house
							int houseNumber = value * 3 + PRNG.nextInt(3);
							city.setTile(x, y, (char) (HOUSE + houseNumber + BLBNCNBIT));
						}
					}
				}
			}

			incrementROG(xpos, ypos, -8);
			return;
		}

		if (pop < 16)
		{
			// remove one little house
			incrementROG(xpos, ypos, -1);
			int z = 0;

			for (int x = xpos-1; x <= xpos+1; x++)
			{
				for (int y = ypos-1; y <= ypos+1; y++)
				{
					if (city.testBounds(x,y))
					{
						int loc = city.map[y][x] & LOMASK;
						if (loc >= LHTHR && loc <= HHTHR)
						{ //little house
							city.setTile(x, y, (char)(Brdr[z] + BLBNCNBIT + FREEZ - 4));
							return;
						}
					}
					z++;
				}
			}
		}
	}

	// returns integer between -3000 and 3000
	int evalCommercial(int x, int y, int traf)
	{
		if (traf < 0)
			return -3000;

		return city.comRate[y/8][x/8];
	}

	// returns integer between -3000 and 3000
	int evalIndustrial(int x, int y, int traf)
	{
		if (traf < 0)
			return -1000;
		else
			return 0;
	}

	// returns integer between -3000 and 3000
	int evalResidential(int x, int y, int traf)
	{
		if (traf < 0)
			return -3000;

		int value = city.landValueMem[y/2][x/2];
		value -= city.pollutionMem[y/2][x/2];

		if (value < 0)
			value = 0;    //cap at 0
		else
			value *= 32;

		if (value > 6000)
			value = 6000; //cap at 6000

		return value - 3000;
	}

	// applies to commercial and residential zones?
	// returns integer from 0 to 3
	int getCRValue(int x, int y)
	{
		int lval = city.landValueMem[y/2][x/2];
		lval -= city.pollutionMem[y/2][x/2];

		if (lval < 30)
			return 0;

		if (lval < 80)
			return 1;

		if (lval < 150)
			return 2;

		return 3;
	}

	void coalSmoke(int mx, int my)
	{
		final int [] SmTb = { COALSMOKE1, COALSMOKE2, COALSMOKE3, COALSMOKE4 };
		final int [] dx = { 1, 2, 1, 2 };
		final int [] dy = { -1, -1, 0, 0 };

		for (int z = 0; z < 4; z++) {
			city.setTile(mx + dx[z], my + dy[z],
			(char) (SmTb[z] | ANIMBIT | CONDBIT | PWRBIT | BURNBIT)
			);
		}
	}

	//TODO- rename to adjustROG
	void incrementROG(int xpos, int ypos, int amount)
	{
		city.rateOGMem[ypos/8][xpos/8] += 4*amount;
	}

	void drawStadium(int mapx, int mapy, int z)
	{
		z -= 5;

		for (int y = mapy-1; y < mapy+3; y++) {
			for (int x = mapx-1; x < mapx+3; x++) {
				city.setTile(x, y, (char) (z | BNCNBIT | (x == mapx && y == mapy ? (ZONEBIT|PWRBIT) : 0)));
				z++;
			}
		}
	}

}
