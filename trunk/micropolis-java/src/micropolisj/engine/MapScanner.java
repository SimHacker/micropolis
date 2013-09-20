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

	static enum TileBehavior
	{
		FIRE,
		FLOOD,
		RADIOACTIVE,
		ROAD,
		RAIL,
		EXPLOSION,
		RESIDENTIAL,
		HOSPITAL_CHURCH,
		COMMERCIAL,
		INDUSTRIAL,
		COAL,
		NUCLEAR,
		FIRESTATION,
		POLICESTATION,
		STADIUM_EMPTY,
		STADIUM_FULL,
		AIRPORT,
		SEAPORT;
	}

	/**
	 * Activate the tile identified by xpos and ypos properties.
	 */
	public void scanTile()
	{
		cchr9 = (char) (cchr & LOMASK);

		String behaviorStr = getTileBehavior(cchr);
		if (behaviorStr == null) {
			return;
		}

		switch (TileBehavior.valueOf(behaviorStr)) {
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
			// clear AniRubble
			city.setTile(xpos, ypos, (char)(RUBBLE + PRNG.nextInt(4) + BULLBIT));
			return;
		case RESIDENTIAL:
			doResidential();
			return;
		case HOSPITAL_CHURCH:
			doHospitalChurch();
			return;
		case COMMERCIAL:
			doCommercial();
			return;
		case INDUSTRIAL:
			doIndustrial();
			return;
		case COAL:
			doCoalPower();
			return;
		case NUCLEAR:
			doNuclearPower();
			return;
		case FIRESTATION:
			doFireStation();
			return;
		case POLICESTATION:
			doPoliceStation();
			return;
		case STADIUM_EMPTY:
			doStadiumEmpty();
			return;
		case STADIUM_FULL:
			doStadiumFull();
			return;
		case AIRPORT:
			doAirport();
			return;
		case SEAPORT:
			doSeaport();
			return;
		default:
			throw new Error("Unknown behavior: "+behaviorStr);
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
				if (!isConductive(cchr))
				{
					if (city.roadEffect < PRNG.nextInt(32))
					{
						if (isOverWater(cchr))
							city.setTile(xpos, ypos, RIVER);
						else
							city.setTile(xpos, ypos, (char)(RUBBLE + PRNG.nextInt(4) + BULLBIT));
						return;
					}
				}
			}
		}

		if (!isCombustible(cchr)) //bridge
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
			z += cchr & ALLBITS;

			city.setTile(xpos, ypos, (char) z);
		}
	}

	/**
	 * Called when the current tile is an active fire.
	 */
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

				int c = city.map[ytem][xtem];
				if (isCombustible(c)) {
					if (isZoneCenter(c)) {
						city.killZone(xtem, ytem, c);
						if ((c & LOMASK) > IZB) { //explode
							city.makeExplosion(xtem, ytem);
						}
					}
					city.setTile(xtem, ytem, (char)(FIRE + PRNG.nextInt(4)));
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
						if (isCombustible(c) || c == DIRT ||
							(t >= WOODS5 && t < FLOOD))
						{
							if (isZoneCenter(c)) {
								city.killZone(xx, yy, c);
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
				if (!isConductive(cchr)) {
					if (city.roadEffect < PRNG.nextInt(32)) {
						if (isOverWater(cchr)) {
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
	//FIXME- a closed bridge with traffic on it is not
	// correctly handled by this subroutine, because the
	// the tiles representing traffic on a bridge do not match
	// the expected tile values of fromTab

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

	boolean checkZonePower()
	{
		boolean zonePwrFlag = setZonePower();

		if (zonePwrFlag)
		{
			city.poweredZoneCount++;
		}
		else
		{
			city.unpoweredZoneCount++;
		}

		return zonePwrFlag;
	}

	boolean setZonePower()
	{
		// refresh cchr, cchr9, since this can get called after the
		// tile's been changed
		cchr = city.map[ypos][xpos];
		cchr9 = (char) (cchr & LOMASK);

		boolean oldPower = (cchr & PWRBIT) == PWRBIT;
		boolean newPower = (
			cchr9 == NUCLEAR ||
			cchr9 == POWERPLANT ||
			city.hasPower(xpos,ypos)
			);

		if (newPower && !oldPower)
		{
			city.setTile(xpos, ypos, (char) (cchr | PWRBIT));
			city.powerZone(xpos, ypos, getZoneSizeFor(cchr));
		}
		else if (!newPower && oldPower)
		{
			city.setTile(xpos, ypos, (char) (cchr & (~PWRBIT)));
			city.shutdownZone(xpos, ypos, getZoneSizeFor(cchr));
		}

		return newPower;
	}

	/**
	 * Place a 3x3 zone on to the map, centered on the current location.
	 * Note: nothing is done if part of this zone is off the edge
	 * of the map or is being flooded or radioactive.
	 *
	 * @param base The "zone" tile value for this zone.
	 * @return true iff the zone was actually placed.
	 */
	boolean zonePlop(int base)
	{
		assert isZoneCenter(base);

		TileSpec.BuildingInfo bi = Tiles.get(base).getBuildingInfo();
		assert bi != null;
		if (bi == null)
			return false;

		for (int y = ypos-1; y < ypos-1+bi.height; y++)
		{
			for (int x = xpos-1; x < xpos-1+bi.width; x++)
			{
				if (!city.testBounds(x, y)) {
					return false;
				}
				if (isIndestructible2(city.getTile(x,y))) {
					// radioactive, on fire, or flooded
					return false;
				}
			}
		}

		assert bi.members.length == bi.width * bi.height;
		int i = 0;
		for (int y = ypos-1; y < ypos-1+bi.height; y++)
		{
			for (int x = xpos-1; x < xpos-1+bi.width; x++)
			{
				city.setTile(x, y, (char)(bi.members[i] | (x == xpos && y == ypos ? BULLBIT : 0)));
				i++;
			}
		}

		setZonePower();
		return true;
	}

	void doCoalPower()
	{
		boolean powerOn = checkZonePower();
		city.coalCount++;
		if ((city.cityTime % 8) == 0) {
			repairZone(POWERPLANT, 4);
		}

		city.powerPlants.add(new CityLocation(xpos,ypos));
	}

	void doNuclearPower()
	{
		boolean powerOn = checkZonePower();
		if (!city.noDisasters && PRNG.nextInt(city.MltdwnTab[city.gameLevel]+1) == 0) {
			city.doMeltdown(xpos, ypos);
			return;
		}

		city.nuclearCount++;
		if ((city.cityTime % 8) == 0) {
			repairZone(NUCLEAR, 4);
		}

		city.powerPlants.add(new CityLocation(xpos, ypos));
	}

	void doFireStation()
	{
		boolean powerOn = checkZonePower();
		city.fireStationCount++;
		if ((city.cityTime % 8) == 0) {
			repairZone(FIRESTATION, 3);
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
	}

	void doPoliceStation()
	{
		boolean powerOn = checkZonePower();
		city.policeCount++;
		if ((city.cityTime % 8) == 0) {
			repairZone(POLICESTATION, 3);
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
	}

	void doStadiumEmpty()
	{
		boolean powerOn = checkZonePower();
		city.stadiumCount++;
		if ((city.cityTime % 16) == 0) {
			repairZone(STADIUM, 4);
		}

		if (powerOn)
		{
			if (((city.cityTime + xpos + ypos) % 32) == 0) {
				drawStadium(FULLSTADIUM);
				city.setTile(xpos+1,ypos, (char)(FOOTBALLGAME1));
				city.setTile(xpos+1,ypos+1,(char)(FOOTBALLGAME2));
			}
		}
	}

	void doStadiumFull()
	{
		boolean powerOn = checkZonePower();
		city.stadiumCount++;
		if (((city.cityTime + xpos + ypos) % 8) == 0) {
			drawStadium(STADIUM);
		}
	}

	void doAirport()
	{
		boolean powerOn = checkZonePower();
		city.airportCount++;
		if ((city.cityTime % 8) == 0) {
			repairZone(AIRPORT, 6);
		}

		if (powerOn) {

			if (PRNG.nextInt(6) == 0) {
				city.generatePlane(xpos, ypos);
			}

			if (PRNG.nextInt(13) == 0) {
				city.generateCopter(xpos, ypos);
			}
		}
	}

	void doSeaport()
	{
		boolean powerOn = checkZonePower();
		city.seaportCount++;
		if ((city.cityTime % 16) == 0) {
			repairZone(PORT, 4);
		}

		if (powerOn && !city.hasSprite(SpriteKind.SHI)) {
			city.generateShip();
		}
	}

	/**
	 * Place hospital or church if needed.
	 */
	void makeHospital()
	{
		if (city.needHospital > 0)
		{
			zonePlop(HOSPITAL);
			city.needHospital = 0;
		}

//FIXME- should be 'else if'
		if (city.needChurch > 0)
		{
			zonePlop(CHURCH);
			city.needChurch = 0;
		}
	}

	/**
	 * Called when the current tile is the key tile of a
	 * hospital or church.
	 */
	void doHospitalChurch()
	{
		boolean powerOn = checkZonePower();
		if (cchr9 == HOSPITAL)
		{
			city.hospitalCount++;

			if (city.cityTime % 16 == 0)
			{
				repairZone(HOSPITAL, 3);
			}
			if (city.needHospital == -1)  //too many hospitals
			{
				if (PRNG.nextInt(21) == 0)
				{
					zonePlop(RESCLR);
				}
			}
		}
		else if (cchr9 == CHURCH)
		{
			city.churchCount++;

			if (city.cityTime % 16 == 0)
			{
				repairZone(CHURCH, 3);
			}
			if (city.needChurch == -1) //too many churches
			{
				if (PRNG.nextInt(21) == 0)
				{
					zonePlop(RESCLR);
				}
			}
		}
	}

	/**
	 * Regenerate the tiles that make up the zone, repairing from
	 * fire, etc.
	 * Only tiles that are not rubble, radioactive, flooded, or
	 * on fire will be regenerated.
	 * @param zoneCenter the tile value for the "center" tile of the zone
	 * @param zoneSize integer (3-6) indicating the width/height of
	 * the zone.
	 */
	void repairZone(char zoneCenter, int zoneSize)
	{
		// from the given center tile, figure out what the
		// northwest tile should be
		int zoneBase = zoneCenter - 1 - zoneSize;

		for (int y = 0; y < zoneSize; y++)
		{
			for (int x = 0; x < zoneSize; x++, zoneBase++)
			{
				int xx = xpos - 1 + x;
				int yy = ypos - 1 + y;

				if (city.testBounds(xx, yy))
				{
					int thCh = city.map[yy][xx];
					if (isZoneCenter(thCh)) {
						continue;
					}

					if (isAnimated(thCh))
						continue;

					if (!isIndestructible(thCh))
					{  //not rubble, radiactive, on fire or flooded

						city.setTile(xx,yy,(char) zoneBase);
					}
				}
			}
		}
	}

	/**
	 * Called when the current tile is the key tile of a commercial
	 * zone.
	 */
	void doCommercial()
	{
		boolean powerOn = checkZonePower();
		city.comZoneCount++;

		int tpop = commercialZonePop(cchr);
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
			int value = getCRValue();
			doCommercialOut(tpop, value);
			return;
		}

		if (PRNG.nextInt(8) == 0)
		{
			int locValve = evalCommercial(trafficGood);
			int zscore = city.comValve + locValve;

			if (!powerOn)
				zscore = -500;

			if (trafficGood != 0 &&
				zscore > -350 &&
				zscore - 26380 > (PRNG.nextInt(0x10000)-0x8000))
			{
				int value = getCRValue();
				doCommercialIn(tpop, value);
				return;
			}

			if (zscore < 350 && zscore + 26380 < (PRNG.nextInt(0x10000)-0x8000))
			{
				int value = getCRValue();
				doCommercialOut(tpop, value);
			}
		}
	}

	/**
	 * Called when the current tile is the key tile of an
	 * industrial zone.
	 */
	void doIndustrial()
	{
		boolean powerOn = checkZonePower();
		city.indZoneCount++;

		int tpop = industrialZonePop(cchr);
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
			doIndustrialOut(tpop, PRNG.nextInt(2));
			return;
		}

		if (PRNG.nextInt(8) == 0)
		{
			int locValve = evalIndustrial(trafficGood);
			int zscore = city.indValve + locValve;

			if (!powerOn)
				zscore = -500;

			if (zscore > -350 &&
				zscore - 26380 > (PRNG.nextInt(0x10000)-0x8000))
			{
				int value = PRNG.nextInt(2);
				doIndustrialIn(tpop, value);
				return;
			}

			if (zscore < 350 && zscore + 26380 < (PRNG.nextInt(0x10000)-0x8000))
			{
				int value = PRNG.nextInt(2);
				doIndustrialOut(tpop, value);
			}
		}
	}

	/**
	 * Called when the current tile is the key tile of a
	 * residential zone.
	 */
	void doResidential()
	{
		boolean powerOn = checkZonePower();
		city.resZoneCount++;

		int tpop; //population of this zone
		if (cchr9 == RESCLR)
		{
			tpop = city.doFreePop(xpos, ypos);
		}
		else
		{
			tpop = residentialZonePop(cchr);
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
			int value = getCRValue();
			doResidentialOut(tpop, value);
			return;
		}

		if (cchr9 == RESCLR || PRNG.nextInt(8) == 0)
		{
			int locValve = evalResidential(trafficGood);
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

				int value = getCRValue();
				doResidentialIn(tpop, value);
				return;
			}

			if (zscore < 350 && zscore + 26380 < (PRNG.nextInt(0x10000)-0x8000))
			{
				int value = getCRValue();
				doResidentialOut(tpop, value);
			}
		}
	}

	/**
	 * Consider the value of building a single-lot house at certain
	 * coordinates.
	 * @return integer; positive number indicates good place for
	 * house to go; zero or a negative number indicates a bad place.
	 */
	int evalLot(int x, int y)
	{
		// test for clear lot
		int tile = city.getTile(x,y);
		if (tile != DIRT && !isResidentialClear(tile)) {
			return -1;
		}

		int score = 1;

		final int [] DX = { 0, 1, 0, -1 };
		final int [] DY = { -1, 0, 1, 0 };
		for (int z = 0; z < 4; z++)
		{
			int xx = x + DX[z];
			int yy = y + DY[z];

			// look for road
			if (city.testBounds(xx, yy)) {
				int tmp = city.getTile(xx, yy);
				if (isRoadAny(tmp) || isRail(tmp))
				{
					score++;
				}
			}
		}

		return score;
	}

	/**
	 * Build a single-lot house on the current residential zone.
	 */
	private void buildHouse(int value)
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
			city.setTile(xx, yy, (char)((HOUSE + houseNumber) | BULLBIT));
		}
	}

	private void doCommercialIn(int pop, int value)
	{
		int z = city.landValueMem[ypos/2][xpos/2] / 32;
		if (pop > z)
			return;

		if (pop < 5)
		{
			comPlop(pop, value);
			adjustROG(8);
		}
	}

	private void doIndustrialIn(int pop, int value)
	{
		if (pop < 4)
		{
			indPlop(pop, value);
			adjustROG(8);
		}
	}

	private void doResidentialIn(int pop, int value)
	{
		assert value >= 0 && value <= 3;

		int z = city.pollutionMem[ypos/2][xpos/2];
		if (z > 128)
			return;

		int cchr9 = cchr & LOMASK;
		if (cchr9 == RESCLR)
		{
			if (pop < 8)
			{
				buildHouse(value);
				adjustROG(1);
				return;
			}

			if (city.getPopulationDensity(xpos, ypos) > 64)
			{
				residentialPlop(0, value);
				adjustROG(8);
				return;
			}
			return;
		}

		if (pop < 40)
		{
			residentialPlop(pop / 8 - 1, value);
			adjustROG(8);
		}
	}

	void comPlop(int density, int value)
	{
		int base = (value * 5 + density) * 9 + CZB;
		zonePlop(base);
	}

	void indPlop(int density, int value)
	{
		int base = (value * 4 + density) * 9 + IZB;
		zonePlop(base);
	}

	void residentialPlop(int density, int value)
	{
		int base = (value * 4 + density) * 9 + RZB;
		zonePlop(base);
	}

	private void doCommercialOut(int pop, int value)
	{
		if (pop > 1)
		{
			comPlop(pop-2, value);
			adjustROG(-8);
		}
		else if (pop == 1)
		{
			zonePlop(COMCLR);
			adjustROG(-8);
		}
	}

	private void doIndustrialOut(int pop, int value)
	{
		if (pop > 1)
		{
			indPlop(pop-2, value);
			adjustROG(-8);
		}
		else if (pop == 1)
		{
			zonePlop(INDCLR);
			adjustROG(-8);
		}
	}

	private void doResidentialOut(int pop, int value)
	{
		assert value >= 0 && value < 4;

		final char [] Brdr = { 0, 3, 6, 1, 4, 7, 2, 5, 8 };

		if (pop == 0)
			return;

		if (pop > 16)
		{
			// downgrade to a lower-density full-size residential zone
			residentialPlop((pop-24) / 8, value);
			adjustROG(-8);
			return;
		}

		if (pop == 16)
		{
			// downgrade from full-size zone to 8 little houses

			int pwrBit = (cchr & PWRBIT);
			city.setTile(xpos, ypos, (char)(RESCLR | BULLBIT | pwrBit));
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
							city.setTile(x, y, (char) ((HOUSE + houseNumber) | BULLBIT));
						}
					}
				}
			}

			adjustROG(-8);
			return;
		}

		if (pop < 16)
		{
			// remove one little house
			adjustROG(-1);
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
							city.setTile(x, y, (char)((Brdr[z] + RESCLR - 4) | BULLBIT));
							return;
						}
					}
					z++;
				}
			}
		}
	}

	/**
	 * Evaluates the zone value of the current commercial zone location.
	 * @return an integer between -3000 and 3000
	 * Same meaning as evalResidential.
	 */
	int evalCommercial(int traf)
	{
		if (traf < 0)
			return -3000;

		return city.comRate[ypos/8][xpos/8];
	}

	/**
	 * Evaluates the zone value of the current industrial zone location.
	 * @return an integer between -3000 and 3000.
	 * Same meaning as evalResidential.
	 */
	int evalIndustrial(int traf)
	{
		if (traf < 0)
			return -1000;
		else
			return 0;
	}

	/**
	 * Evaluates the zone value of the current residential zone location.
	 * @return an integer between -3000 and 3000. The higher the
	 * number, the more likely the zone is to GROW; the lower the
	 * number, the more likely the zone is to SHRINK.
	 */
	int evalResidential(int traf)
	{
		if (traf < 0)
			return -3000;

		int value = city.landValueMem[ypos/2][xpos/2];
		value -= city.pollutionMem[ypos/2][xpos/2];

		if (value < 0)
			value = 0;    //cap at 0
		else
			value *= 32;

		if (value > 6000)
			value = 6000; //cap at 6000

		return value - 3000;
	}

	/**
	 * Gets the land-value class (0-3) for the current
	 * residential or commercial zone location.
	 * @return integer from 0 to 3, 0 is the lowest-valued
	 * zone, and 3 is the highest-valued zone.
	 */
	int getCRValue()
	{
		int lval = city.landValueMem[ypos/2][xpos/2];
		lval -= city.pollutionMem[ypos/2][xpos/2];

		if (lval < 30)
			return 0;

		if (lval < 80)
			return 1;

		if (lval < 150)
			return 2;

		return 3;
	}

	/**
	 * Record a zone's population change to the rate-of-growth
	 * map.
	 * An adjustment of +/- 1 corresponds to one little house.
	 * An adjustment of +/- 8 corresponds to a full-size zone.
	 *
	 * @param amount the positive or negative adjustment to record.
	 */
	void adjustROG(int amount)
	{
		city.rateOGMem[ypos/8][xpos/8] += 4*amount;
	}

	/**
	 * Place tiles for a stadium (full or empty).
	 * @param zoneCenter either STADIUM or FULLSTADIUM
	 */
	void drawStadium(int zoneCenter)
	{
		int zoneBase = zoneCenter - 1 - 4;

		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++, zoneBase++)
			{
				city.setTile(xpos - 1 + x, ypos - 1 + y,
					(char) (zoneBase | (x == 1 && y == 1 ? (PWRBIT) : 0)));
			}
		}
	}

}
