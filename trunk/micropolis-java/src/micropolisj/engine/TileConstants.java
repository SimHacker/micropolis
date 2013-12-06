// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

import java.util.Arrays;

/**
 * Contains symbolic names of certain tile values,
 * and helper functions to test tile attributes.
 * Attributes of tiles that are interesting:
 * <ul>
 * <li>ZONE - the special tile for a zone
 * <li>ANIM - the tile animates
 * <li>BULL - is bulldozable
 * <li>BURN - is combustible
 * <li>COND - can conduct power
 * <li>Road - traffic
 * <li>Rail - railroad
 * <li>Floodable - subject to floods
 * <li>Wet
 * <li>Rubble
 * <li>Tree
 * <li>OverWater
 * <li>Arsonable
 * <li>Vulnerable - vulnerable to earthquakes
 * <li>Bridge
 * <li>AutoDozeRRW - automatically bulldoze when placing Road/Rail/Wire
 * <li>AutoDozeZ - automatically bulldoze when placing Zone
 * </ul>
 */
public class TileConstants
{
	//
	// terrain mapping
	//
	public static final short CLEAR = -1;
	public static final char DIRT = 0;
	static final char RIVER = 2;
	static final char REDGE = 3;
	static final char CHANNEL = 4;
	static final char RIVEDGE = 5;
	static final char FIRSTRIVEDGE = 5;
	static final char LASTRIVEDGE = 20;
	static final char TREEBASE = 21;
	static final char WOODS_LOW = TREEBASE;
	static final char WOODS = 37;
	static final char WOODS_HIGH = 39;
	static final char WOODS2 = 40;
	static final char WOODS5 = 43;
	static final char RUBBLE = 44;
	static final char LASTRUBBLE = 47;
	static final char FLOOD = 48;
	static final char LASTFLOOD = 51;
	static final char RADTILE = 52;
	static final char FIRE = 56;
	static final char ROADBASE = 64;
	static final char HBRIDGE = 64;
	static final char VBRIDGE = 65;
	static final char ROADS = 66;
	static final char ROADS2 = 67;
	private static final char ROADS3 = 68;
	private static final char ROADS4 = 69;
	private static final char ROADS5 = 70;
	private static final char ROADS6 = 71;
	private static final char ROADS7 = 72;
	private static final char ROADS8 = 73;
	private static final char ROADS9 = 74;
	private static final char ROADS10 = 75;
	static final char INTERSECTION = 76;
	static final char HROADPOWER = 77;
	static final char VROADPOWER = 78;
	static final char BRWH = 79;       //horz bridge, open
	static final char LTRFBASE = 80;
	static final char BRWV = 95;       //vert bridge, open
	static final char HTRFBASE = 144;
	private static final char LASTROAD = 206;
	static final char POWERBASE = 208;
	static final char HPOWER = 208;    //underwater power-line
	static final char VPOWER = 209;
	static final char LHPOWER = 210;
	static final char LVPOWER = 211;
	static final char LVPOWER2 = 212;
	private static final char LVPOWER3 = 213;
	private static final char LVPOWER4 = 214;
	private static final char LVPOWER5 = 215;
	private static final char LVPOWER6 = 216;
	private static final char LVPOWER7 = 217;
	private static final char LVPOWER8 = 218;
	private static final char LVPOWER9 = 219;
	private static final char LVPOWER10 = 220;
	static final char RAILHPOWERV = 221;
	static final char RAILVPOWERH = 222;
	static final char LASTPOWER = 222;
	static final char RAILBASE = 224;
	static final char HRAIL = 224;     //underwater rail (horz)
	static final char VRAIL = 225;     //underwater rail (vert)
	static final char LHRAIL = 226;
	static final char LVRAIL = 227;
	static final char LVRAIL2 = 228;
	private static final char LVRAIL3 = 229;
	private static final char LVRAIL4 = 230;
	private static final char LVRAIL5 = 231;
	private static final char LVRAIL6 = 232;
	private static final char LVRAIL7 = 233;
	private static final char LVRAIL8 = 234;
	private static final char LVRAIL9 = 235;
	private static final char LVRAIL10 = 236;
	static final char HRAILROAD = 237;
	static final char VRAILROAD = 238;
	static final char LASTRAIL = 238;
	static final char RESBASE = 240;
	static final char RESCLR = 244;
	static final char HOUSE = 249;
	static final char LHTHR = 249;  //12 house tiles
	static final char HHTHR = 260;
	static final char RZB = 265; //residential zone base
	static final char HOSPITAL = 409;
	static final char CHURCH = 418;
	static final char COMBASE = 423;
	static final char COMCLR = 427;
	static final char CZB = 436; //commercial zone base
	static final char INDBASE = 612;
	static final char INDCLR = 616;
	static final char IZB = 625;
	static final char PORTBASE = 693;
	static final char PORT = 698;
	static final char AIRPORT = 716;
	static final char POWERPLANT = 750;
	static final char FIRESTATION = 765;
	static final char POLICESTATION = 774;
	static final char STADIUM = 784;
	static final char FULLSTADIUM = 800;
	static final char NUCLEAR = 816;
	static final char LASTZONE = 826;
	public static final char LIGHTNINGBOLT = 827;
	static final char HBRDG0 = 828;   //draw bridge tiles (horz)
	static final char HBRDG1 = 829;
	static final char HBRDG2 = 830;
	static final char HBRDG3 = 831;
	static final char FOUNTAIN = 840;
	static final char TINYEXP = 860;
	private static final char LASTTINYEXP = 867;
	static final char FOOTBALLGAME1 = 932;
	static final char FOOTBALLGAME2 = 940;
	static final char VBRDG0 = 948;   //draw bridge tiles (vert)
	static final char VBRDG1 = 949;
	static final char VBRDG2 = 950;
	static final char VBRDG3 = 951;
	public static final char LAST_TILE = 956;

	static final char [] RoadTable = new char[] {
		ROADS, ROADS2, ROADS, ROADS3,
		ROADS2, ROADS2, ROADS4, ROADS8,
		ROADS, ROADS6, ROADS, ROADS7,
		ROADS5, ROADS10, ROADS9, INTERSECTION
		};

	static final char [] RailTable = new char[] {
		LHRAIL, LVRAIL, LHRAIL, LVRAIL2,
		LVRAIL, LVRAIL, LVRAIL3, LVRAIL7,
		LHRAIL, LVRAIL5, LHRAIL, LVRAIL6,
		LVRAIL4, LVRAIL9, LVRAIL8, LVRAIL10
		};

	static final char [] WireTable = new char[] {
		LHPOWER, LVPOWER, LHPOWER, LVPOWER2,
		LVPOWER, LVPOWER, LVPOWER3, LVPOWER7,
		LHPOWER, LVPOWER5, LHPOWER, LVPOWER6,
		LVPOWER4, LVPOWER9, LVPOWER8, LVPOWER10
		};

	//
	// status bits
	//
	public static final char PWRBIT = 32768;  // bit 15 ... currently powered
	// bit 14 ... unused
	// bit 13 ... unused
	public static final char BULLBIT = 4096;  // bit 12 ... is bulldozable
	// bit 11 ... unused
	// bit 10 ... unused

	public static final char ALLBITS = 64512;   // mask for upper 6 bits
	public static final char LOMASK = 1023; //mask for low 10 bits

	private TileConstants() {}

	/**
	 * Checks whether the tile can be auto-bulldozed for
	 * placement of road, rail, or wire.
	 */
	public static boolean canAutoBulldozeRRW(int tileValue)
	{
		// can we autobulldoze this tile?
		return (
		(tileValue >= FIRSTRIVEDGE && tileValue <= LASTRUBBLE) ||
		(tileValue >= TINYEXP && tileValue <= LASTTINYEXP)
		);
	}

	/**
	 * Checks whether the tile can be auto-bulldozed for
	 * placement of a zone.
	 */
	public static boolean canAutoBulldozeZ(char tileValue)
	{
		//FIXME- what is significance of POWERBASE+2 and POWERBASE+12 ?

		// can we autobulldoze this tile?
		if ((tileValue >= FIRSTRIVEDGE && tileValue <= LASTRUBBLE) ||
			(tileValue >= POWERBASE + 2 && tileValue <= POWERBASE + 12) ||
			(tileValue >= TINYEXP && tileValue <= LASTTINYEXP))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//used by scanTile
	public static String getTileBehavior(int tile)
	{
		tile &= LOMASK;
		TileSpec ts = Tiles.get(tile);
		return ts != null ? ts.getAttribute("behavior") : null;
	}

	//used by queryZoneStatus
	public static int getDescriptionNumber(int tile)
	{
		tile &= LOMASK;
		TileSpec ts = Tiles.get(tile);
		if (ts != null) {
			return ts.getDescriptionNumber();
		}
		else {
			return -1;
		}
	}

	public static int getPollutionValue(int tile)
	{
		TileSpec spec = Tiles.get(tile & LOMASK);
		return spec != null ? spec.getPollutionValue() : 0;
	}

	public static boolean isAnimated(int tile)
	{
		TileSpec spec = Tiles.get(tile & LOMASK);
		return spec != null && spec.animNext != null;
	}

	//used by setFire()
	public static boolean isArsonable(int tile)
	{
		return (
			!isZoneCenter(tile) &&
			(tile & LOMASK) >= LHTHR &&
			(tile & LOMASK) <= LASTZONE
			);
	}

	//used by Sprite::destroyTile
	public static boolean isBridge(int tile)
	{
		return isRoad(tile) && !isCombustible(tile);
	}

	public static boolean isCombustible(int tile)
	{
		TileSpec spec = Tiles.get(tile & LOMASK);
		return spec != null && spec.canBurn;
	}

	public static boolean isConductive(int tile)
	{
		TileSpec spec = Tiles.get(tile & LOMASK);
		return spec != null && spec.canConduct;
	}

	/** Used in repairZone(). */
	public static boolean isIndestructible(int tile)
	{
		int tmp = tile & LOMASK;
		return tmp >= RUBBLE && tmp < ROADBASE;
	}

	/** Used in zonePlop(). */
	public static boolean isIndestructible2(int tile)
	{
		int tmp = tile & LOMASK;
		return tmp >= FLOOD && tmp < ROADBASE;
	}

	public static boolean isOverWater(int tile)
	{
		TileSpec spec = Tiles.get(tile & LOMASK);
		return spec != null && spec.overWater;
	}

	public static boolean isRubble(int cell)
	{
		return (((cell & LOMASK) >= RUBBLE) &&
			((cell & LOMASK) <= LASTRUBBLE));
	}

	public static boolean isTree(char cell)
	{
		return (((cell & LOMASK) >= WOODS_LOW) &&
			((cell & LOMASK) <= WOODS_HIGH));
	}

	//used by makeEarthquake
	public static boolean isVulnerable(int tile)
	{
		int tem2 = tile & LOMASK;
		if (tem2 < RESBASE ||
			tem2 > LASTZONE ||
			isZoneCenter(tile)
			) {
			return false;
		} else {
			return true;
		}
	}

	public static boolean checkWet(int tile)
	{
		int x = tile & LOMASK;
		return (x == POWERBASE ||
			x == POWERBASE+1 ||
			x == RAILBASE ||
			x == RAILBASE + 1 ||
			x == BRWH ||
			x == BRWV);
	}

	public static CityDimension getZoneSizeFor(int tile)
	{
		assert isZoneCenter(tile);

		TileSpec spec = Tiles.get(tile & LOMASK);
		return spec != null ? spec.getBuildingSize() : null;
	}

	public static boolean isConstructed(int tile)
	{
		return tile >= 0 && (tile & LOMASK) >= ROADBASE;
	}

	static boolean isRiverEdge(int tile)
	{
		tile &= LOMASK;
		return tile >= FIRSTRIVEDGE && tile <= LASTRIVEDGE;
	}

	public static boolean isDozeable(int tile)
	{
		return tile >= 0 && (tile & BULLBIT) != 0;
	}

	static boolean isFloodable(int tile)
	{
		return (tile == DIRT || ((tile & BULLBIT) != 0 && isCombustible(tile)));
	}

	/**
	 * Note: does not include rail/road tiles.
	 * @see #isRoadAny
	 */
	public static boolean isRoad(int tile)
	{
		int tmp = tile & LOMASK;
		return (tmp >= ROADBASE && tmp < POWERBASE);
	}

	public static boolean isRoadAny(int tile)
	{
		int tmp = tile & LOMASK;
		return (tmp >= ROADBASE && tmp < POWERBASE)
			|| (tmp == HRAILROAD)
			|| (tmp == VRAILROAD);
	}

	/**
	 * Checks whether the tile is a road that will automatically change to connect to
	 * neighboring roads.
	 */
	public static boolean isRoadDynamic(int tile)
	{
		int tmp = neutralizeRoad(tile);
		return (tmp >= ROADS && tmp <= INTERSECTION);
	}

	public static boolean roadConnectsEast(int tile)
	{
		tile = neutralizeRoad(tile);
		return (((tile == VRAILROAD) ||
			(tile >= ROADBASE && tile <= VROADPOWER)
			) &&
			(tile != VROADPOWER) &&
			(tile != HRAILROAD) &&
			(tile != VBRIDGE));
	}

	public static boolean roadConnectsNorth(int tile)
	{
		tile = neutralizeRoad(tile);
		return (((tile == HRAILROAD) ||
			(tile >= ROADBASE && tile <= VROADPOWER)
			) &&
			(tile != HROADPOWER) &&
			(tile != VRAILROAD) &&
			(tile != ROADBASE));
	}

	public static boolean roadConnectsSouth(int tile)
	{
		tile = neutralizeRoad(tile);
		return (((tile == HRAILROAD) ||
			(tile >= ROADBASE && tile <= VROADPOWER)
			) &&
			(tile != HROADPOWER) &&
			(tile != VRAILROAD) &&
			(tile != ROADBASE));
	}

	public static boolean roadConnectsWest(int tile)
	{
		tile = neutralizeRoad(tile);
		return (((tile == VRAILROAD) ||
			(tile >= ROADBASE && tile <= VROADPOWER)	
			) &&
			(tile != VROADPOWER) &&
			(tile != HRAILROAD) &&
			(tile != VBRIDGE));
	}

	public static boolean isRail(int tile)
	{
		int tmp = tile & LOMASK;
		return (tmp >= RAILBASE && tmp < RESBASE);
	}

	public static boolean isRailAny(int tile)
	{
		int tmp = tile & LOMASK;
		return (tmp >= RAILBASE && tmp < RESBASE)
			|| (tmp == RAILHPOWERV)
			|| (tmp == RAILVPOWERH);
	}

	public static boolean isRailDynamic(int tile)
	{
		int tmp = tile & LOMASK;
		return (tmp >= LHRAIL && tmp <= LVRAIL10);
	}

	public static boolean railConnectsEast(int tile)
	{
		tile = neutralizeRoad(tile);
		return (tile >= RAILHPOWERV && tile <= VRAILROAD &&
			tile != RAILVPOWERH &&
			tile != VRAILROAD &&
			tile != VRAIL);
	}

	public static boolean railConnectsNorth(int tile)
	{
		tile = neutralizeRoad(tile);
		return (tile >= RAILHPOWERV && tile <= VRAILROAD &&
			tile != RAILHPOWERV &&
			tile != HRAILROAD &&
			tile != HRAIL);
	}

	public static boolean railConnectsSouth(int tile)
	{
		tile = neutralizeRoad(tile);
		return (tile >= RAILHPOWERV && tile <= VRAILROAD &&
			tile != RAILHPOWERV &&
			tile != HRAILROAD &&
			tile != HRAIL);
	}

	public static boolean railConnectsWest(int tile)
	{
		tile = neutralizeRoad(tile);
		return (tile >= RAILHPOWERV && tile <= VRAILROAD &&
			tile != RAILVPOWERH &&
			tile != VRAILROAD &&
			tile != VRAIL);
	}

	public static boolean isWireDynamic(int tile)
	{
		int tmp = tile & LOMASK;
		return (tmp >= LHPOWER && tmp <= LVPOWER10);
	}

	public static boolean wireConnectsEast(int tile)
	{
		int ntile = neutralizeRoad(tile);
		return (isConductive(tile) &&
			ntile != HPOWER &&
			ntile != HROADPOWER &&
			ntile != RAILHPOWERV);
	}

	public static boolean wireConnectsNorth(int tile)
	{
		int ntile = neutralizeRoad(tile);
		return (isConductive(tile) &&
			ntile != VPOWER &&
			ntile != VROADPOWER &&
			ntile != RAILVPOWERH);
	}

	public static boolean wireConnectsSouth(int tile)
	{
		int ntile = neutralizeRoad(tile);
		return (isConductive(tile) &&
			ntile != VPOWER &&
			ntile != VROADPOWER &&
			ntile != RAILVPOWERH);
	}

	public static boolean wireConnectsWest(int tile)
	{
		int ntile = neutralizeRoad(tile);
		return (isConductive(tile) &&
			ntile != HPOWER &&
			ntile != HROADPOWER &&
			ntile != RAILHPOWERV);
	}

	public static boolean isCommercialZone(int tile)
	{
		int tmp = tile & LOMASK;
		TileSpec ts = Tiles.get(tmp);
		if (ts != null) {
			if (ts.owner != null) {
				ts = ts.owner;
			}
			return ts.getBooleanAttribute("commercial-zone");
		}
		return false;
	}

	public static boolean isHospitalOrChurch(int tile)
	{
		return (tile & LOMASK) >= HOSPITAL &&
			(tile & LOMASK) < COMBASE;
	}

	/**
	 * Checks whether the tile is defined with the "industrial-zone" attribute.
	 * Note: the old version of this function erroneously included the coal power
	 * plant smoke as an industrial zone.
	 */
	public static boolean isIndustrialZone(int tile)
	{
		int tmp = tile & LOMASK;
		TileSpec ts = Tiles.get(tmp);
		if (ts != null) {
			if (ts.owner != null) {
				ts = ts.owner;
			}
			return ts.getBooleanAttribute("industrial-zone");
		}
		return false;
	}

	public static boolean isResidentialClear(int tile)
	{
		return (tile & LOMASK) >= RESBASE && (tile & LOMASK) <= RESBASE+8;
	}

	/** Note: does not include hospital/church.
	 * @see #isHospitalOrChurch
	 */
	public static boolean isResidentialZone(int tile)
	{
		return (tile & LOMASK) >= RESBASE &&
			(tile & LOMASK) < HOSPITAL;
	}

	// includes hospital/church.
	public static boolean isResidentialZoneAny(int tile)
	{
		int tmp = tile & LOMASK;
		TileSpec ts = Tiles.get(tmp);
		if (ts != null) {
			if (ts.owner != null) {
				ts = ts.owner;
			}
			return ts.getBooleanAttribute("residential-zone");
		}
		return false;
	}

	/** Tile represents a part of any sort of building. */
	public static boolean isZoneAny(int tile)
	{
		return (tile & LOMASK) >= RESBASE;
	}

	public static boolean isZoneCenter(int tile)
	{
		TileSpec spec = Tiles.get(tile & LOMASK);
		return spec != null && spec.zone;
	}

	/**
	 * Converts a road tile value with traffic to the equivalent
	 * road tile without traffic.
	 */
	public static char neutralizeRoad(int tile)
	{
		tile &= LOMASK;
		if (tile >= ROADBASE && tile <= LASTROAD) {
			tile = ((tile - ROADBASE) & 0xf) + ROADBASE;
		}
		return (char)tile;
	}

	/**
	 * Determine the population level of a Residential zone
	 * tile. Note: the input tile MUST be a full-size res zone,
	 * it cannot be an empty zone.
	 * @return int multiple of 8 between 16 and 40.
	 */
	public static int residentialZonePop(int tile)
	{
		TileSpec ts = Tiles.get(tile & LOMASK);
		return ts.getPopulation();
	}

	/**
	 * Determine the population level of a Commercial zone
	 * tile.
	 * The input tile MAY be an empty zone.
	 * @return int between 0 and 5.
	 */
	public static int commercialZonePop(int tile)
	{
		TileSpec ts = Tiles.get(tile & LOMASK);
		return ts.getPopulation() / 8;
	}

	/**
	 * Determine the population level of an Industrial zone tile.
	 * The input tile MAY be an empty zone.
	 * @return int between 0 and 4.
	 */
	public static int industrialZonePop(int tile)
	{
		TileSpec ts = Tiles.get(tile & LOMASK);
		return ts.getPopulation() / 8;
	}
}
