// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

import static micropolisj.engine.TileConstants.*;

/**
 * Enumerates the various tools that can be applied to the map by the user.
 * Call the tool's apply() method to actually use the tool on the map.
 */
public enum MicropolisTool
{
	BULLDOZER,
	WIRE,
	ROADS,
	RAIL,
	RESIDENTIAL,
	COMMERCIAL,
	INDUSTRIAL,
	FIRE,
	POLICE,
	STADIUM,
	PARK,
	SEAPORT,
	POWERPLANT,
	NUCLEAR,
	AIRPORT,
	QUERY;

	public int getWidth()
	{
		switch(this)
		{
		case RESIDENTIAL:
		case COMMERCIAL:
		case INDUSTRIAL:
		case FIRE:
		case POLICE:
			return 3;

		case STADIUM:
		case SEAPORT:
		case POWERPLANT:
		case NUCLEAR:
			return 4;

		case AIRPORT:
			return 6;

		default:
			return 1;
		}
	}

	public int getHeight()
	{
		return getWidth();
	}

	public ToolResult apply(Micropolis engine, int xpos, int ypos)
	{
		switch (this)
		{
		case BULLDOZER:
			return applyBulldozer(engine, xpos, ypos);

		case RAIL:
			return applyRailTool(engine, xpos, ypos);

		case ROADS:
			return applyRoadTool(engine, xpos, ypos);

		case WIRE:
			return applyWireTool(engine, xpos, ypos);

		case PARK:
			return applyParkTool(engine, xpos, ypos);

		case RESIDENTIAL:
			return apply3x3buildingTool(engine, xpos, ypos, RESBASE);

		case COMMERCIAL:
			return apply3x3buildingTool(engine, xpos, ypos, COMBASE);

		case INDUSTRIAL:
			return apply3x3buildingTool(engine, xpos, ypos, INDBASE);

		case FIRE:
			return apply3x3buildingTool(engine, xpos, ypos, FIRESTBASE);

		case POLICE:
			return apply3x3buildingTool(engine, xpos, ypos, POLICESTBASE);

		case POWERPLANT:
			return apply4x4buildingTool(engine, xpos, ypos, COALBASE);

		case STADIUM:
			return apply4x4buildingTool(engine, xpos, ypos, STADIUMBASE);

		case SEAPORT:
			return apply4x4buildingTool(engine, xpos, ypos, PORTBASE);

		case NUCLEAR:
			return apply4x4buildingTool(engine, xpos, ypos, NUCLEARBASE);

		case AIRPORT:
			return apply6x6buildingTool(engine, xpos, ypos, AIRPORTBASE);

		default:
			// not expected
			return ToolResult.UH_OH;
		}
	}

	ToolResult apply3x3buildingTool(Micropolis engine, int xpos, int ypos, char tileBase)
	{
		int mapH = xpos - 1;
		int mapV = ypos - 1;

		if (!(mapH >= 0 && mapH + 2 < engine.getWidth()))
			return ToolResult.UH_OH;
		if (!(mapV >= 0 && mapV + 2 < engine.getHeight()))
			return ToolResult.UH_OH;

		int cost = 0;
		boolean canBuild = true;
		for (int rowNum = 0; rowNum <= 2; rowNum++)
		{
			for (int columnNum = 0; columnNum <= 2; columnNum++)
			{
				int x = mapH + columnNum;
				int y = mapV + rowNum;
				char tileValue = (char) (engine.getTile(x,y) & LOMASK);

				if (tileValue != DIRT)
				{
					if (engine.autoBulldoze)
					{
						if (canAutoBulldoze(tileValue))
							cost++;
						else
							canBuild = false;
					}
					else
						canBuild = false;
				}
			}
		}

		if (!canBuild)
			return ToolResult.UH_OH;

		cost += getToolCost();

		if (engine.totalFunds < cost)
			return ToolResult.INSUFFICIENT_FUNDS;

		// take care of the money situation here
		engine.spend(cost);

		for (int rowNum = 0; rowNum <= 2; rowNum++)
		{
			for (int columnNum = 0; columnNum <= 2; columnNum++)
			{
				engine.setTile(mapH + columnNum, mapV + rowNum, (char) (
					tileBase + BNCNBIT +
					(columnNum == 1 && rowNum == 1 ? ZONEBIT : 0)
					));
				tileBase++;
			}
		}

		fixBorder(engine, mapH, mapV, mapH + 2, mapV + 2);
		return ToolResult.SUCCESS;
	}

	ToolResult apply4x4buildingTool(Micropolis engine, int xpos, int ypos, char tileBase)
	{
		int mapH = xpos - 1;
		int mapV = ypos - 1;

		if (!(mapH >= 0 && mapH + 3 < engine.getWidth()))
			return ToolResult.UH_OH;
		if (!(mapV >= 0 && mapV + 3 < engine.getHeight()))
			return ToolResult.UH_OH;

		int cost = 0;
		boolean canBuild = true;
		for (int rowNum = 0; rowNum <= 3; rowNum++)
		{
			for (int columnNum = 0; columnNum <= 3; columnNum++)
			{
				int x = mapH + columnNum;
				int y = mapV + rowNum;
				char tileValue = (char) (engine.getTile(x,y) & LOMASK);

				if (tileValue != DIRT)
				{
					if (engine.autoBulldoze)
					{
						if (canAutoBulldoze(tileValue))
							cost++;
						else
							canBuild = false;
					}
					else
						canBuild = false;
				}
			}
		}

		if (!canBuild)
			return ToolResult.UH_OH;

		cost += getToolCost();

		if (engine.totalFunds < cost)
			return ToolResult.INSUFFICIENT_FUNDS;

		// take care of the money situation here
		engine.spend(cost);

		for (int rowNum = 0; rowNum <= 3; rowNum++)
		{
			for (int columnNum = 0; columnNum <= 3; columnNum++)
			{
				engine.setTile(mapH + columnNum, mapV + rowNum, (char) (
					tileBase + BNCNBIT +
					(columnNum == 1 && rowNum == 1 ? ZONEBIT : 0) +
					(columnNum == 1 && rowNum == 2 ? ANIMBIT : 0)
					));
				tileBase++;
			}
		}

		fixBorder(engine, mapH, mapV, mapH + 3, mapV + 3);
		return ToolResult.SUCCESS;
	}

	ToolResult apply6x6buildingTool(Micropolis engine, int xpos, int ypos, char tileBase)
	{
		int mapH = xpos - 1;
		int mapV = ypos - 1;

		if (!(mapH >= 0 && mapH + 5 < engine.getWidth()))
			return ToolResult.UH_OH;
		if (!(mapV >= 0 && mapV + 5 < engine.getHeight()))
			return ToolResult.UH_OH;

		int cost = 0;
		boolean canBuild = true;
		for (int rowNum = 0; rowNum <= 5; rowNum++)
		{
			for (int columnNum = 0; columnNum <= 5; columnNum++)
			{
				int x = mapH + columnNum;
				int y = mapV + rowNum;
				char tileValue = (char) (engine.getTile(x,y) & LOMASK);

				if (tileValue != DIRT)
				{
					if (engine.autoBulldoze)
					{
						if (canAutoBulldoze(tileValue))
							cost++;
						else
							canBuild = false;
					}
					else
						canBuild = false;
				}
			}
		}

		if (!canBuild)
			return ToolResult.UH_OH;

		cost += getToolCost();

		if (engine.totalFunds < cost)
			return ToolResult.INSUFFICIENT_FUNDS;

		// take care of the money situation here
		engine.spend(cost);

		for (int rowNum = 0; rowNum <= 5; rowNum++)
		{
			for (int columnNum = 0; columnNum <= 5; columnNum++)
			{
				engine.setTile(mapH + columnNum, mapV + rowNum, (char) (
					tileBase + BNCNBIT +
					(columnNum == 1 && rowNum == 1 ? ZONEBIT : 0)
					));
				tileBase++;
			}
		}

		fixBorder(engine, mapH, mapV, mapH + 5, mapV + 5);
		return ToolResult.SUCCESS;
	}

	public int getToolCost()
	{
		switch (this)
		{
		case BULLDOZER: return 1;
		case WIRE: return 5; //25 for underwater
		case ROADS: return 10;  //50 for over water
		case RAIL: return 20;   //100 for underwater
		case RESIDENTIAL: return 100;
		case COMMERCIAL: return 100;
		case INDUSTRIAL: return 100;
		case FIRE: return 500;
		case POLICE: return 500;
		case STADIUM: return 5000;
		case PARK: return 10;
		case SEAPORT: return 3000;
		case POWERPLANT: return 3000;
		case NUCLEAR: return 5000;
		case AIRPORT: return 10000;
		case QUERY: return 0;
		default:
			assert false;
			return 1;
		}
	}

	static boolean canAutoBulldoze(char tileValue)
	{
		// can we autobulldoze this tile?
		if ((tileValue >= FIRSTRIVEDGE && tileValue <= LASTRUBBLE) ||
			(tileValue >= POWERBASE + 2 && tileValue <= POWERBASE + 12) ||
			(tileValue >= TINYEXP && tileValue <= LASTTINYEXP + 2))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void fixBorder(Micropolis engine, int left, int top, int right, int bottom)
	{
		for (int x = left; x <= right; x++)
		{
			fixZone(engine, x, top);
			fixZone(engine, x, bottom);
		}
		for (int y = top + 1; y <= bottom - 1; y++)
		{
			fixZone(engine, left, y);
			fixZone(engine, right, y);
		}
	}

	ToolResult applyBulldozer(Micropolis engine, int xpos, int ypos)
	{
		if (!engine.testBounds(xpos, ypos))
			return ToolResult.UH_OH;

		char currTile = engine.getTile(xpos, ypos);
		char tmp = (char)(currTile & LOMASK);

		if ((currTile & ZONEBIT) != 0)
		{
			// zone center bit is set
			if (engine.totalFunds >= 1)
			{
				engine.spend(1);
				switch (checkSize(tmp))
				{
				case 3:
					engine.makeSound(xpos, ypos, Sound.EXPLOSION_HIGH);
					putRubble(engine, xpos, ypos, 3, 3);
					break;
				case 4:
					engine.makeSound(xpos, ypos, Sound.EXPLOSION_LOW);
					putRubble(engine, xpos, ypos, 4, 4);
					break;
				case 6:
					engine.makeSound(xpos, ypos, Sound.EXPLOSION_BOTH);
					putRubble(engine, xpos, ypos, 6, 6);
					break;
				default:
					assert false;
					break;
				}
				return ToolResult.SUCCESS;
			}
			else
			{
				return ToolResult.INSUFFICIENT_FUNDS;
			}
		}
		else if (false && isBigZone(tmp))
		{
			// The GPL Micropolis will uses a bunch of code to find
			// the center of this zone, and then converts it to rubble
			// the same as clicking the center of the zone.
			// I prefer to make the user click the critical spot of
			// the zone to destroy it.
			return ToolResult.UH_OH;
		}
		else if (tmp == RIVER ||
			tmp == REDGE ||
			tmp == CHANNEL)
		{
			if (engine.totalFunds >= 6)
			{
				ToolResult result = layDoze(engine, xpos, ypos);
				if (tmp != (engine.getTile(xpos, ypos) & LOMASK))
				{
					// tile changed
					engine.spend(5);
					fixZone(engine, xpos, ypos);
				}
				return result;
			}
			else {
				return ToolResult.INSUFFICIENT_FUNDS;
			}
		}
		else
		{
			ToolResult result = layDoze(engine, xpos, ypos);
			fixZone(engine, xpos, ypos);
			return result;
		}
	}

	void autoDoze(Micropolis engine, int xpos, int ypos)
	{
		if (engine.autoBulldoze && engine.totalFunds > 0)
		{
			char tile = engine.getTile(xpos, ypos);
			char ntile = neutralizeRoad(tile);

			if ((tile & BULLBIT) != 0 &&
				((ntile >= TINYEXP &&
				ntile <= LASTTINYEXP) ||
				(ntile < HBRIDGE && ntile != DIRT)
				)
				)
			{
				engine.spend(1);
				engine.setTile(xpos, ypos, DIRT);
			}
		}
	}

	ToolResult applyRailTool(Micropolis engine, int xpos, int ypos)
	{
		if (!engine.testBounds(xpos, ypos))
			return ToolResult.UH_OH;

		autoDoze(engine, xpos, ypos);
		ToolResult result = layRail(engine, xpos, ypos);
		fixZone(engine, xpos, ypos);
		return result;
	}

	ToolResult applyRoadTool(Micropolis engine, int xpos, int ypos)
	{
		if (!engine.testBounds(xpos, ypos))
			return ToolResult.UH_OH;

		autoDoze(engine, xpos, ypos);
		ToolResult result = layRoad(engine, xpos, ypos);
		fixZone(engine, xpos, ypos);
		return result;
	}

	ToolResult applyParkTool(Micropolis engine, int xpos, int ypos)
	{
		if (!engine.testBounds(xpos, ypos))
			return ToolResult.UH_OH;

		int cost = getToolCost();

		if (engine.getTile(xpos, ypos) != DIRT) {
			// some sort of bulldozing is necessary
			if (!engine.autoBulldoze) {
				return ToolResult.UH_OH;
			}

			if (isRubble(engine.getTile(xpos, ypos))) {
				// this tile can be auto-bulldozed
				cost++;
			}
			else {
				// cannot be auto-bulldozed
				return ToolResult.UH_OH;
			}
		}

		if (engine.totalFunds < cost) {
			return ToolResult.INSUFFICIENT_FUNDS;
		}

		int z = engine.PRNG.nextInt(5);
		int tile;
		if (z < 4) {
			tile = (WOODS2 + z) | BURNBIT | BULLBIT;
		} else {
			tile = FOUNTAIN | BURNBIT | BULLBIT | ANIMBIT;
		}

		engine.spend(cost);
		engine.setTile(xpos, ypos, (char) tile);
		return ToolResult.SUCCESS;
	}

	ToolResult applyWireTool(Micropolis engine, int xpos, int ypos)
	{
		if (!engine.testBounds(xpos, ypos))
			return ToolResult.UH_OH;

		autoDoze(engine, xpos, ypos);
		ToolResult result = layWire(engine, xpos, ypos);
		fixZone(engine, xpos, ypos);
		return result;
	}

	static char neutralizeRoad(char tile)
	{
		tile &= LOMASK;
		if (tile >= 64 && tile <= 207)
			tile = (char)( (tile & 0xf) + 64 );
		return tile;
	}

	private ToolResult layDoze(Micropolis engine, int xpos, int ypos)
	{
		if (engine.totalFunds <= 0)
			return ToolResult.INSUFFICIENT_FUNDS;

		char tile = engine.getTile(xpos, ypos);

		// check dozeable bit
		if ((tile & BULLBIT) == 0)
			return ToolResult.NONE;

		tile = neutralizeRoad(tile);
		if (isOverWater(tile))
		{
			// dozing over water, replace with water.
			engine.setTile(xpos, ypos, RIVER);
		}
		else
		{
			// dozing on land, replace with land. Simple, eh?
			engine.setTile(xpos, ypos, DIRT);
		}

		engine.spend(1);
		return ToolResult.SUCCESS;
	}

	private ToolResult layRail(Micropolis engine, int xpos, int ypos)
	{
		final int RAIL_COST = 20;
		final int TUNNEL_COST = 100;

		int cost = RAIL_COST;
		if (engine.totalFunds < cost)
			return ToolResult.INSUFFICIENT_FUNDS;

		char tile = (char) (engine.getTile(xpos, ypos) & LOMASK);
		switch (tile)
		{
		case RIVER:		// rail on water
		case REDGE:
		case CHANNEL:

			cost = TUNNEL_COST;
			if (engine.totalFunds < cost)
				return ToolResult.INSUFFICIENT_FUNDS;

			if (xpos + 1 < engine.getWidth())
			{
				char eTile = neutralizeRoad(engine.getTile(xpos + 1, ypos));
				if (eTile == RAILHPOWERV ||
					eTile == HRAIL ||
					(eTile >= LHRAIL && eTile <= HRAILROAD))
				{
					engine.setTile(xpos, ypos, (char) (HRAIL | BULLBIT));
					break;
				}
			}

			if (xpos > 0)
			{
				char wTile = neutralizeRoad(engine.getTile(xpos - 1, ypos));
				if (wTile == RAILHPOWERV ||
					wTile == HRAIL ||
					(wTile > VRAIL && wTile < VRAILROAD))
				{
					engine.setTile(xpos, ypos, (char) (HRAIL | BULLBIT));
					break;
				}
			}

			if (ypos + 1 < engine.getHeight())
			{
				char sTile = neutralizeRoad(engine.getTile(xpos, ypos + 1));
				if (sTile == RAILVPOWERH ||
					sTile == VRAILROAD ||
					(sTile > HRAIL && sTile < HRAILROAD))
				{
					engine.setTile(xpos, ypos, (char) (VRAIL | BULLBIT));
					break;
				}
			}

			if (ypos > 0)
			{
				char nTile = neutralizeRoad(engine.getTile(xpos, ypos - 1));
				if (nTile == RAILVPOWERH ||
					nTile == VRAILROAD ||
					(nTile > HRAIL && nTile < HRAILROAD))
				{
					engine.setTile(xpos, ypos, (char) (VRAIL | BULLBIT));
					break;
				}
			}

			// cannot do road here
			return ToolResult.NONE;

		case LHPOWER: // rail on power
			engine.setTile(xpos, ypos, (char) (RAILVPOWERH | CONDBIT | BURNBIT | BULLBIT));
			break;

		case LVPOWER: // rail on power
			engine.setTile(xpos, ypos, (char) (RAILHPOWERV | CONDBIT | BURNBIT | BULLBIT));
			break;

		case TileConstants.ROADS:	// rail on road (case 1)
			engine.setTile(xpos, ypos, (char) (VRAILROAD | BURNBIT | BULLBIT));
			break;

		case ROADS2:	// rail on road (case 2)
			engine.setTile(xpos, ypos, (char) (HRAILROAD | BURNBIT | BULLBIT));
			break;

		default:
			//TODO- check if auto-bulldoze is enabled
			if (tile != DIRT) {
				return ToolResult.NONE;
			}

		  	//rail on dirt
			engine.setTile(xpos, ypos, (char) (LHRAIL | BULLBIT | BURNBIT));
			break;
		}

		engine.spend(cost);
		return ToolResult.SUCCESS;
	}

	private ToolResult layRoad(Micropolis engine, int xpos, int ypos)
	{
		final int ROAD_COST = 10;
		final int BRIDGE_COST = 50;

		int cost = ROAD_COST;
		if (engine.totalFunds < cost)
			return ToolResult.INSUFFICIENT_FUNDS;

		char tile = (char) (engine.getTile(xpos, ypos) & LOMASK);
		switch (tile)
		{
		case RIVER:		// road on water
		case REDGE:
		case CHANNEL:	// check how to build bridges, if possible.

			cost = BRIDGE_COST;
			if (engine.totalFunds < cost)
				return ToolResult.INSUFFICIENT_FUNDS;

			if (xpos + 1 < engine.getWidth())
			{
				char eTile = neutralizeRoad(engine.getTile(xpos + 1, ypos));
				if (eTile == VRAILROAD ||
					eTile == HBRIDGE ||
					(eTile >= TileConstants.ROADS && eTile <= HROADPOWER))
				{
					engine.setTile(xpos, ypos, (char) (HBRIDGE | BULLBIT));
					break;
				}
			}

			if (xpos > 0)
			{
				char wTile = neutralizeRoad(engine.getTile(xpos - 1, ypos));
				if (wTile == VRAILROAD ||
					wTile == HBRIDGE ||
					(wTile >= TileConstants.ROADS && wTile <= INTERSECTION))
				{
					engine.setTile(xpos, ypos, (char) (HBRIDGE | BULLBIT));
					break;
				}
			}

			if (ypos + 1 < engine.getHeight())
			{
				char sTile = neutralizeRoad(engine.getTile(xpos, ypos + 1));
				if (sTile == HRAILROAD ||
					sTile == VROADPOWER ||
					(sTile >= VBRIDGE && sTile <= INTERSECTION))
				{
					engine.setTile(xpos, ypos, (char) (VBRIDGE | BULLBIT));
					break;
				}
			}

			if (ypos > 0)
			{
				char nTile = neutralizeRoad(engine.getTile(xpos, ypos - 1));
				if (nTile == HRAILROAD ||
					nTile == VROADPOWER ||
					(nTile >= VBRIDGE && nTile <= INTERSECTION))
				{
					engine.setTile(xpos, ypos, (char) (VBRIDGE | BULLBIT));
					break;
				}
			}

			// cannot do road here
			return ToolResult.NONE;

		case LHPOWER: //road on power
			engine.setTile(xpos, ypos, (char) (VROADPOWER | CONDBIT | BURNBIT | BULLBIT));
			break;

		case LVPOWER: //road on power #2
			engine.setTile(xpos, ypos, (char) (HROADPOWER | CONDBIT | BURNBIT | BULLBIT));
			break;

		case LHRAIL: //road on rail
			engine.setTile(xpos, ypos, (char) (HRAILROAD | BURNBIT | BULLBIT));
			break;

		case LVRAIL: //road on rail #2
			engine.setTile(xpos, ypos, (char) (VRAILROAD | BURNBIT | BULLBIT));
			break;

		default:
			// TODO- auto-bulldoze here
			if (tile != DIRT)
				return ToolResult.NONE;

			// road on dirt
			engine.setTile(xpos, ypos, (char) (TileConstants.ROADS | BULLBIT | BURNBIT));
			break;
		}
	
		engine.spend(cost);
		return ToolResult.SUCCESS;
	}

	private ToolResult layWire(Micropolis engine, int xpos, int ypos)
	{
		final int WIRE_COST = 5;
		final int UNDERWATER_WIRE_COST = 25;

		int cost = WIRE_COST;
		if (engine.totalFunds < cost)
			return ToolResult.INSUFFICIENT_FUNDS;

		char tile = (char) (engine.getTile(xpos, ypos) & LOMASK);
		tile = neutralizeRoad(tile);

		switch (tile)
		{
		case RIVER:		// wire on water
		case REDGE:
		case CHANNEL:

			cost = UNDERWATER_WIRE_COST;
			if (engine.totalFunds < cost)
				return ToolResult.INSUFFICIENT_FUNDS;

			if (xpos + 1 < engine.getWidth())
			{
				char tmp = engine.getTile(xpos + 1, ypos);
				char tmpn = neutralizeRoad(tmp);

				if ((tmp & CONDBIT) != 0 &&
					tmpn != HROADPOWER &&
					tmpn != RAILHPOWERV &&
					tmpn != HPOWER)
				{
					engine.setTile(xpos, ypos, (char) (VPOWER | CONDBIT | BULLBIT));
					break;
				}
			}

			if (xpos > 0)
			{
				char tmp = engine.getTile(xpos - 1, ypos);
				char tmpn = neutralizeRoad(tmp);

				if ((tmp & CONDBIT) != 0 &&
					tmpn != HROADPOWER &&
					tmpn != RAILHPOWERV &&
					tmpn != HPOWER)
				{
					engine.setTile(xpos, ypos, (char) (VPOWER | CONDBIT | BULLBIT));
					break;
				}
			}

			if (ypos + 1 < engine.getHeight())
			{
				char tmp = engine.getTile(xpos, ypos + 1);
				char tmpn = neutralizeRoad(tmp);

				if ((tmp & CONDBIT) != 0 &&
					tmpn != VROADPOWER &&
					tmpn != RAILVPOWERH &&
					tmpn != VPOWER)
				{
					engine.setTile(xpos, ypos, (char) (HPOWER | CONDBIT | BULLBIT));
					break;
				}
			}

			if (ypos > 0)
			{
				char tmp = engine.getTile(xpos, ypos - 1);
				char tmpn = neutralizeRoad(tmp);

				if ((tmp & CONDBIT) != 0 &&
					tmpn != VROADPOWER &&
					tmpn != RAILVPOWERH &&
					tmpn != VPOWER)
				{
					engine.setTile(xpos, ypos, (char) (HPOWER | CONDBIT | BULLBIT));
					break;
				}
			}

			// cannot do wire here
			return ToolResult.NONE;

		case TileConstants.ROADS: // wire on E/W road
			engine.setTile(xpos, ypos, (char) (HROADPOWER | CONDBIT | BURNBIT | BULLBIT));
			break;

		case ROADS2: // wire on N/S road
			engine.setTile(xpos, ypos, (char) (VROADPOWER | CONDBIT | BURNBIT | BULLBIT));
			break;

		case LHRAIL:	// wire on E/W railroad tracks
			engine.setTile(xpos, ypos, (char) (RAILHPOWERV | CONDBIT | BURNBIT | BULLBIT));
			break;

		case LVRAIL:	// wire on N/S railroad tracks
			engine.setTile(xpos, ypos, (char) (RAILVPOWERH | CONDBIT | BURNBIT | BULLBIT));
			break;

		default:
			if (tile != DIRT) {
				//cannot do wire here
				return ToolResult.NONE;
			}

			//wire on dirt
			engine.setTile(xpos, ypos, (char) (LHPOWER | CONDBIT | BULLBIT | BURNBIT));
			break;
		}

		engine.spend(cost);
		return ToolResult.SUCCESS;
	}

	private void fixZone(Micropolis engine, int xpos, int ypos)
	{
		fixSingle(engine, xpos, ypos);
		if (ypos > 0)
			fixSingle(engine, xpos, ypos - 1);
		if (xpos > 0)
			fixSingle(engine, xpos - 1, ypos);
		if (xpos + 1 < engine.getWidth())
			fixSingle(engine, xpos + 1, ypos);
		if (ypos + 1 < engine.getHeight())
			fixSingle(engine, xpos, ypos + 1);
	}

	private void fixSingle(Micropolis engine, int xpos, int ypos)
	{
		char tile = (char) (engine.getTile(xpos, ypos) & LOMASK);
		tile = neutralizeRoad(tile);

		if (tile >= TileConstants.ROADS && tile <= INTERSECTION)
		{
			// cleanup road
			int adjTile = 0;

			if (ypos > 0)
			{
				tile = engine.getTile(xpos, ypos - 1);
				tile = neutralizeRoad(tile);
				if (((tile == HRAILROAD) ||
					(tile >= ROADBASE && tile <= VROADPOWER)
					) &&
					(tile != HROADPOWER) &&
					(tile != VRAILROAD) &&
					(tile != ROADBASE))
				{
					adjTile |= 1;
				}
			}

			if (xpos + 1 < engine.getWidth())
			{
				tile = engine.getTile(xpos + 1, ypos);
				tile = neutralizeRoad(tile);
				if (((tile == VRAILROAD) ||
					(tile >= ROADBASE && tile <= VROADPOWER)	
					) &&
					(tile != VROADPOWER) &&
					(tile != HRAILROAD) &&
					(tile != VBRIDGE))
				{
					adjTile |= 2;
				}
			}

			if (ypos + 1 < engine.getHeight())
			{
				tile = engine.getTile(xpos, ypos + 1);
				tile = neutralizeRoad(tile);
				if (((tile == HRAILROAD) ||
					(tile >= ROADBASE && tile <= VROADPOWER)
					) &&
					(tile != HROADPOWER) &&
					(tile != VRAILROAD) &&
					(tile != ROADBASE))
				{
					adjTile |= 4;
				}
				
			}

			if (xpos > 0)
			{
				tile = engine.getTile(xpos - 1, ypos);
				tile = neutralizeRoad(tile);
				if (((tile == VRAILROAD) ||
					(tile >= ROADBASE && tile <= VROADPOWER)
					) &&
					(tile != VROADPOWER) &&
					(tile != HRAILROAD) &&
					(tile != VBRIDGE))
				{
					adjTile |= 8;
				}
			}

			engine.setTile(xpos, ypos, (char)(RoadTable[adjTile] | BULLBIT | BURNBIT));
			return;
		} //endif on a road tile

		if (tile >= LHRAIL && tile <= LVRAIL10)
		{
			// cleanup Rail
			int adjTile = 0;

			if (ypos > 0)
			{
				tile = engine.getTile(xpos, ypos - 1);
				tile = neutralizeRoad(tile);
				if (tile >= RAILHPOWERV && tile <= VRAILROAD &&
					tile != RAILHPOWERV &&
					tile != HRAILROAD &&
					tile != HRAIL)
				{
					adjTile |= 1;
				}
			}

			if (xpos + 1 < engine.getWidth())
			{
				tile = engine.getTile(xpos + 1, ypos);
				tile = neutralizeRoad(tile);
				if (tile >= RAILHPOWERV && tile <= VRAILROAD &&
					tile != RAILVPOWERH &&
					tile != VRAILROAD &&
					tile != VRAIL)
				{
					adjTile |= 2;
				}
			}

			if (ypos + 1 < engine.getHeight())
			{
				tile = engine.getTile(xpos, ypos + 1);
				tile = neutralizeRoad(tile);
				if (tile >= RAILHPOWERV && tile <= VRAILROAD &&
					tile != RAILHPOWERV &&
					tile != HRAILROAD &&
					tile != HRAIL)
				{
					adjTile |= 4;
				}
			}

			if (xpos > 0)
			{
				tile = engine.getTile(xpos - 1, ypos);
				tile = neutralizeRoad(tile);
				if (tile >= RAILHPOWERV && tile <= VRAILROAD &&
					tile != RAILVPOWERH &&
					tile != VRAILROAD &&
					tile != VRAIL)
				{
					adjTile |= 8;
				}
			}

			engine.setTile(xpos, ypos, (char)(RailTable[adjTile] | BULLBIT | BURNBIT));
			return;
		} //end if on a rail tile

		if (tile >= LHPOWER && tile <= LVPOWER10)
		{
			// Cleanup Wire
			int adjTile = 0;

			if (ypos > 0)
			{
				tile = engine.getTile(xpos, ypos - 1);
				char ntile = neutralizeRoad(tile);
				if ((tile & CONDBIT) != 0 &&
					ntile != VPOWER &&
					ntile != VROADPOWER &&
					ntile != RAILVPOWERH)
				{
					adjTile |= 1;
				}
			}

			if (xpos + 1 < engine.getWidth())
			{
				tile = engine.getTile(xpos + 1, ypos);
				char ntile = neutralizeRoad(tile);
				if ((tile & CONDBIT) != 0 &&
					ntile != HPOWER &&
					ntile != HROADPOWER &&
					ntile != RAILHPOWERV)
				{
					adjTile |= 2;
				}
			}

			if (ypos + 1 < engine.getHeight())
			{
				tile = engine.getTile(xpos, ypos + 1);
				char ntile = neutralizeRoad(tile);
				if ((tile & CONDBIT) != 0 &&
					ntile != VPOWER &&
					ntile != VROADPOWER &&
					ntile != RAILVPOWERH)
				{
					adjTile |= 4;
				}
			}

			if (xpos > 0)
			{
				tile = engine.getTile(xpos - 1, ypos);
				char ntile = neutralizeRoad(tile);
				if ((tile & CONDBIT) != 0 &&
					ntile != HPOWER &&
					ntile != HROADPOWER &&
					ntile != RAILHPOWERV)
				{
					adjTile |= 8;
				}
			}

			engine.setTile(xpos, ypos, (char)(WireTable[adjTile] | BULLBIT | BURNBIT | CONDBIT));
			return;
		} //end if on a rail tile
	}

	void putRubble(Micropolis engine, int xpos, int ypos, int w, int h)
	{
		for (int xx = xpos - 1; xx <= xpos + w-2; xx++) {
			for (int yy = ypos - 1; yy <= ypos + h-2; yy++) {
				if (engine.testBounds(xx, yy)) {
					int tile = engine.getTile(xx,yy) & LOMASK;
					if (tile != RADTILE && tile != DIRT) {
						int nTile = (TINYEXP + engine.PRNG.nextInt(3))
							| ANIMBIT | BULLBIT;
						engine.setTile(xx, yy, (char)nTile);
					}
				}
			}
		}
	}

	boolean isBigZone(int tile)
	{
		if (tile >= RESBASE && tile <= LASTZONE)
			return true;
		else if (tile >= SMOKEBASE && tile < TINYEXP)
			return true;
		else if (tile >= COALSMOKE1)
			return true;
		else
			return false;
	}

	int checkSize(int tile)
	{
		if ((tile >= (RESBASE-1) && tile <= (PORTBASE-1)) ||
			(tile >= (LASTPOWERPLANT+1) && tile <= (POLICESTATION+4)))
		{
			return 3;
		}
		else if ((tile >= PORTBASE && tile <= LASTPORT) ||
			(tile >= COALBASE && tile <= LASTPOWERPLANT) ||
			(tile >= STADIUMBASE && tile <= LASTZONE))
		{
			return 4;
		}
		else if (tile == TileConstants.AIRPORT)
		{
			return 6;
		}
		else
		{
			return 0;
		}
	}
}
