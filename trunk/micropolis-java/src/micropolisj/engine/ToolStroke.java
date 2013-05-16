// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

import java.awt.Rectangle;
import static micropolisj.engine.TileConstants.*;

public class ToolStroke
{
	final Micropolis city;
	final MicropolisTool tool;
	int xpos;
	int ypos;
	int xdest;
	int ydest;

	ToolStroke(Micropolis city, MicropolisTool tool, int xpos, int ypos)
	{
		this.city = city;
		this.tool = tool;
		this.xpos = xpos;
		this.ypos = ypos;
		this.xdest = xpos;
		this.ydest = ypos;
	}

	public ToolResult apply()
	{
		Rectangle r = getBounds();
		for (int i = r.y; i < r.y + r.height; i += tool.getHeight()) {
			for (int j = r.x; j < r.x + r.width;
					j += tool.getWidth()) {
				apply1(j + (tool.getWidth() >= 3 ? 1 : 0),
					i + (tool.getHeight() >= 3 ? 1 : 0)
				);
			}
		}
		// TODO- actually check the result of application
		return ToolResult.SUCCESS;
	}

	public ToolResult apply1(int xpos, int ypos)
	{
		switch (tool)
		{
		case BULLDOZER:
			return applyBulldozer(xpos, ypos);

		case RAIL:
			return applyRailTool(xpos, ypos);

		case ROADS:
			return applyRoadTool(xpos, ypos);

		case WIRE:
			return applyWireTool(xpos, ypos);

		case PARK:
			return applyParkTool(xpos, ypos);

		case RESIDENTIAL:
			return apply3x3buildingTool(xpos, ypos, RESBASE);

		case COMMERCIAL:
			return apply3x3buildingTool(xpos, ypos, COMBASE);

		case INDUSTRIAL:
			return apply3x3buildingTool(xpos, ypos, INDBASE);

		case FIRE:
			return apply3x3buildingTool(xpos, ypos, FIRESTBASE);

		case POLICE:
			return apply3x3buildingTool(xpos, ypos, POLICESTBASE);

		case POWERPLANT:
			return apply4x4buildingTool(xpos, ypos, COALBASE);

		case STADIUM:
			return apply4x4buildingTool(xpos, ypos, STADIUMBASE);

		case SEAPORT:
			return apply4x4buildingTool(xpos, ypos, PORTBASE);

		case NUCLEAR:
			return apply4x4buildingTool(xpos, ypos, NUCLEARBASE);

		case AIRPORT:
			return apply6x6buildingTool(xpos, ypos, AIRPORTBASE);

		default:
			// not expected
			return ToolResult.UH_OH;
		}
	}

	public void dragTo(int xdest, int ydest)
	{
		this.xdest = xdest;
		this.ydest = ydest;
	}

	public Rectangle getBounds_line()
	{
		assert tool.getWidth() == 1;
		assert tool.getHeight() == 1;

		if (Math.abs(xdest-xpos) >= Math.abs(ydest-ypos)) {
			// horizontal line
			Rectangle r = new Rectangle();
			r.x = Math.min(xpos, xdest);
			r.width = Math.abs(xdest-xpos) + 1;
			r.y = ypos;
			r.height = 1;
			return r;
		}
		else {
			// vertical line
			Rectangle r = new Rectangle();
			r.x = xpos;
			r.width = 1;
			r.y = Math.min(ypos, ydest);
			r.height = Math.abs(ydest-ypos) + 1;
			return r;
		}
	}

	public Rectangle getBounds()
	{
		if (tool == MicropolisTool.RAIL ||
			tool == MicropolisTool.ROADS ||
			tool == MicropolisTool.WIRE)
		{
			return getBounds_line();
		}

		Rectangle r = new Rectangle();

		r.x = xpos;
		if (tool.getWidth() >= 3) {
			r.x--;
		}
		if (xdest >= xpos) {
			r.width = ((xdest-xpos) / tool.getWidth() + 1) * tool.getWidth();
		}
		else {
			r.width = ((xpos-xdest) / tool.getWidth() + 1) * tool.getHeight();
			r.x += tool.getWidth() - r.width;
		}

		r.y = ypos;
		if (tool.getHeight() >= 3) {
			r.y--;
		}
		if (ydest >= ypos) {
			r.height = ((ydest-ypos) / tool.getHeight() + 1) * tool.getHeight();
		}
		else {
			r.height = ((ypos-ydest) / tool.getHeight() + 1) * tool.getHeight();
			r.y += tool.getHeight() - r.height;
		}

		return r;
	}

	ToolResult apply3x3buildingTool(int xpos, int ypos, char tileBase)
	{
		int mapH = xpos - 1;
		int mapV = ypos - 1;

		if (!(mapH >= 0 && mapH + 2 < city.getWidth()))
			return ToolResult.UH_OH;
		if (!(mapV >= 0 && mapV + 2 < city.getHeight()))
			return ToolResult.UH_OH;

		int cost = 0;
		boolean canBuild = true;
		for (int rowNum = 0; rowNum <= 2; rowNum++)
		{
			for (int columnNum = 0; columnNum <= 2; columnNum++)
			{
				int x = mapH + columnNum;
				int y = mapV + rowNum;
				char tileValue = (char) (city.getTile(x,y) & LOMASK);

				if (tileValue != DIRT)
				{
					if (city.autoBulldoze)
					{
						if (canAutoBulldozeZ(tileValue))
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

		cost += tool.getToolCost();

		if (city.budget.totalFunds < cost)
			return ToolResult.INSUFFICIENT_FUNDS;

		// take care of the money situation here
		city.spend(cost);

		for (int rowNum = 0; rowNum <= 2; rowNum++)
		{
			for (int columnNum = 0; columnNum <= 2; columnNum++)
			{
				city.setTile(mapH + columnNum, mapV + rowNum, (char) (
					tileBase + BNCNBIT +
					(columnNum == 1 && rowNum == 1 ? ZONEBIT : 0)
					));
				tileBase++;
			}
		}

		fixBorder(mapH, mapV, mapH + 2, mapV + 2);
		return ToolResult.SUCCESS;
	}

	ToolResult apply4x4buildingTool(int xpos, int ypos, char tileBase)
	{
		int mapH = xpos - 1;
		int mapV = ypos - 1;

		if (!(mapH >= 0 && mapH + 3 < city.getWidth()))
			return ToolResult.UH_OH;
		if (!(mapV >= 0 && mapV + 3 < city.getHeight()))
			return ToolResult.UH_OH;

		int cost = 0;
		boolean canBuild = true;
		for (int rowNum = 0; rowNum <= 3; rowNum++)
		{
			for (int columnNum = 0; columnNum <= 3; columnNum++)
			{
				int x = mapH + columnNum;
				int y = mapV + rowNum;
				char tileValue = (char) (city.getTile(x,y) & LOMASK);

				if (tileValue != DIRT)
				{
					if (city.autoBulldoze)
					{
						if (canAutoBulldozeZ(tileValue))
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

		cost += tool.getToolCost();

		if (city.budget.totalFunds < cost)
			return ToolResult.INSUFFICIENT_FUNDS;

		// take care of the money situation here
		city.spend(cost);

		for (int rowNum = 0; rowNum <= 3; rowNum++)
		{
			for (int columnNum = 0; columnNum <= 3; columnNum++)
			{
				city.setTile(mapH + columnNum, mapV + rowNum, (char) (
					tileBase + BNCNBIT +
					(columnNum == 1 && rowNum == 1 ? ZONEBIT : 0) +
					(columnNum == 1 && rowNum == 2 ? ANIMBIT : 0)
					));
				tileBase++;
			}
		}

		fixBorder(mapH, mapV, mapH + 3, mapV + 3);
		return ToolResult.SUCCESS;
	}

	ToolResult apply6x6buildingTool(int xpos, int ypos, char tileBase)
	{
		int mapH = xpos - 1;
		int mapV = ypos - 1;

		if (!(mapH >= 0 && mapH + 5 < city.getWidth()))
			return ToolResult.UH_OH;
		if (!(mapV >= 0 && mapV + 5 < city.getHeight()))
			return ToolResult.UH_OH;

		int cost = 0;
		boolean canBuild = true;
		for (int rowNum = 0; rowNum <= 5; rowNum++)
		{
			for (int columnNum = 0; columnNum <= 5; columnNum++)
			{
				int x = mapH + columnNum;
				int y = mapV + rowNum;
				char tileValue = (char) (city.getTile(x,y) & LOMASK);

				if (tileValue != DIRT)
				{
					if (city.autoBulldoze)
					{
						if (canAutoBulldozeZ(tileValue))
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

		cost += tool.getToolCost();

		if (city.budget.totalFunds < cost)
			return ToolResult.INSUFFICIENT_FUNDS;

		// take care of the money situation here
		city.spend(cost);

		for (int rowNum = 0; rowNum <= 5; rowNum++)
		{
			for (int columnNum = 0; columnNum <= 5; columnNum++)
			{
				city.setTile(mapH + columnNum, mapV + rowNum, (char) (
					tileBase + BNCNBIT +
					(columnNum == 1 && rowNum == 1 ? ZONEBIT : 0)
					));
				tileBase++;
			}
		}

		fixBorder(mapH, mapV, mapH + 5, mapV + 5);
		return ToolResult.SUCCESS;
	}

	void fixBorder(int left, int top, int right, int bottom)
	{
		for (int x = left; x <= right; x++)
		{
			fixZone(x, top);
			fixZone(x, bottom);
		}
		for (int y = top + 1; y <= bottom - 1; y++)
		{
			fixZone(left, y);
			fixZone(right, y);
		}
	}

	ToolResult applyBulldozer(int xpos, int ypos)
	{
		if (!city.testBounds(xpos, ypos))
			return ToolResult.UH_OH;

		char currTile = city.getTile(xpos, ypos);
		char tmp = (char)(currTile & LOMASK);

		if ((currTile & ZONEBIT) != 0)
		{
			// zone center bit is set
			if (city.budget.totalFunds >= 1)
			{
				city.spend(1);
				switch (checkSize(tmp))
				{
				case 3:
					city.makeSound(xpos, ypos, Sound.EXPLOSION_HIGH);
					putRubble(xpos, ypos, 3, 3);
					break;
				case 4:
					city.makeSound(xpos, ypos, Sound.EXPLOSION_LOW);
					putRubble(xpos, ypos, 4, 4);
					break;
				case 6:
					city.makeSound(xpos, ypos, Sound.EXPLOSION_BOTH);
					putRubble(xpos, ypos, 6, 6);
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
			if (city.budget.totalFunds >= 6)
			{
				ToolResult result = layDoze(xpos, ypos);
				if (tmp != (city.getTile(xpos, ypos) & LOMASK))
				{
					// tile changed
					city.spend(5);
					fixZone(xpos, ypos);
				}
				return result;
			}
			else {
				return ToolResult.INSUFFICIENT_FUNDS;
			}
		}
		else
		{
			ToolResult result = layDoze(xpos, ypos);
			fixZone(xpos, ypos);
			return result;
		}
	}

	ToolResult applyRailTool(int xpos, int ypos)
	{
		if (!city.testBounds(xpos, ypos))
			return ToolResult.UH_OH;

		ToolResult result = layRail(xpos, ypos);
		fixZone(xpos, ypos);
		return result;
	}

	ToolResult applyRoadTool(int xpos, int ypos)
	{
		if (!city.testBounds(xpos, ypos))
			return ToolResult.UH_OH;

		ToolResult result = layRoad(xpos, ypos);
		fixZone(xpos, ypos);
		return result;
	}

	ToolResult applyParkTool(int xpos, int ypos)
	{
		if (!city.testBounds(xpos, ypos))
			return ToolResult.UH_OH;

		int cost = tool.getToolCost();

		if (city.getTile(xpos, ypos) != DIRT) {
			// some sort of bulldozing is necessary
			if (!city.autoBulldoze) {
				return ToolResult.UH_OH;
			}

			if (isRubble(city.getTile(xpos, ypos))) {
				// this tile can be auto-bulldozed
				cost++;
			}
			else {
				// cannot be auto-bulldozed
				return ToolResult.UH_OH;
			}
		}

		if (city.budget.totalFunds < cost) {
			return ToolResult.INSUFFICIENT_FUNDS;
		}

		int z = city.PRNG.nextInt(5);
		int tile;
		if (z < 4) {
			tile = (WOODS2 + z) | BURNBIT | BULLBIT;
		} else {
			tile = FOUNTAIN | BURNBIT | BULLBIT | ANIMBIT;
		}

		city.spend(cost);
		city.setTile(xpos, ypos, (char) tile);
		return ToolResult.SUCCESS;
	}

	ToolResult applyWireTool(int xpos, int ypos)
	{
		if (!city.testBounds(xpos, ypos))
			return ToolResult.UH_OH;

		ToolResult result = layWire(xpos, ypos);
		fixZone(xpos, ypos);
		return result;
	}

	static char neutralizeRoad(char tile)
	{
		tile &= LOMASK;
		if (tile >= 64 && tile <= 207)
			tile = (char)( (tile & 0xf) + 64 );
		return tile;
	}

	private ToolResult layDoze(int xpos, int ypos)
	{
		if (city.budget.totalFunds <= 0)
			return ToolResult.INSUFFICIENT_FUNDS;

		char tile = city.getTile(xpos, ypos);

		// check dozeable bit
		if ((tile & BULLBIT) == 0)
			return ToolResult.NONE;

		tile = neutralizeRoad(tile);
		if (isOverWater(tile))
		{
			// dozing over water, replace with water.
			city.setTile(xpos, ypos, RIVER);
		}
		else
		{
			// dozing on land, replace with land. Simple, eh?
			city.setTile(xpos, ypos, DIRT);
		}

		city.spend(1);
		return ToolResult.SUCCESS;
	}

	private ToolResult layRail(int xpos, int ypos)
	{
		final int RAIL_COST = 20;
		final int TUNNEL_COST = 100;

		int cost = RAIL_COST;
		if (city.budget.totalFunds < cost)
			return ToolResult.INSUFFICIENT_FUNDS;

		char tile = (char) (city.getTile(xpos, ypos) & LOMASK);
		switch (tile)
		{
		case RIVER:		// rail on water
		case REDGE:
		case CHANNEL:

			cost = TUNNEL_COST;
			if (city.budget.totalFunds < cost)
				return ToolResult.INSUFFICIENT_FUNDS;

			if (xpos + 1 < city.getWidth())
			{
				char eTile = neutralizeRoad(city.getTile(xpos + 1, ypos));
				if (eTile == RAILHPOWERV ||
					eTile == HRAIL ||
					(eTile >= LHRAIL && eTile <= HRAILROAD))
				{
					city.setTile(xpos, ypos, (char) (HRAIL | BULLBIT));
					break;
				}
			}

			if (xpos > 0)
			{
				char wTile = neutralizeRoad(city.getTile(xpos - 1, ypos));
				if (wTile == RAILHPOWERV ||
					wTile == HRAIL ||
					(wTile > VRAIL && wTile < VRAILROAD))
				{
					city.setTile(xpos, ypos, (char) (HRAIL | BULLBIT));
					break;
				}
			}

			if (ypos + 1 < city.getHeight())
			{
				char sTile = neutralizeRoad(city.getTile(xpos, ypos + 1));
				if (sTile == RAILVPOWERH ||
					sTile == VRAILROAD ||
					(sTile > HRAIL && sTile < HRAILROAD))
				{
					city.setTile(xpos, ypos, (char) (VRAIL | BULLBIT));
					break;
				}
			}

			if (ypos > 0)
			{
				char nTile = neutralizeRoad(city.getTile(xpos, ypos - 1));
				if (nTile == RAILVPOWERH ||
					nTile == VRAILROAD ||
					(nTile > HRAIL && nTile < HRAILROAD))
				{
					city.setTile(xpos, ypos, (char) (VRAIL | BULLBIT));
					break;
				}
			}

			// cannot do road here
			return ToolResult.NONE;

		case LHPOWER: // rail on power
			city.setTile(xpos, ypos, (char) (RAILVPOWERH | CONDBIT | BURNBIT | BULLBIT));
			break;

		case LVPOWER: // rail on power
			city.setTile(xpos, ypos, (char) (RAILHPOWERV | CONDBIT | BURNBIT | BULLBIT));
			break;

		case TileConstants.ROADS:	// rail on road (case 1)
			city.setTile(xpos, ypos, (char) (VRAILROAD | BURNBIT | BULLBIT));
			break;

		case ROADS2:	// rail on road (case 2)
			city.setTile(xpos, ypos, (char) (HRAILROAD | BURNBIT | BULLBIT));
			break;

		default:
			if (tile != DIRT) {
				if (city.autoBulldoze && canAutoBulldozeRRW(tile)) {
					cost += 1; //autodoze cost
				}
				else {
					// cannot do rail here
					return ToolResult.NONE;
				}
			}

		  	//rail on dirt
			city.setTile(xpos, ypos, (char) (LHRAIL | BULLBIT | BURNBIT));
			break;
		}

		city.spend(cost);
		return ToolResult.SUCCESS;
	}

	private ToolResult layRoad(int xpos, int ypos)
	{
		final int ROAD_COST = 10;
		final int BRIDGE_COST = 50;

		int cost = ROAD_COST;
		if (city.budget.totalFunds < cost)
			return ToolResult.INSUFFICIENT_FUNDS;

		char tile = (char) (city.getTile(xpos, ypos) & LOMASK);
		switch (tile)
		{
		case RIVER:		// road on water
		case REDGE:
		case CHANNEL:	// check how to build bridges, if possible.

			cost = BRIDGE_COST;
			if (city.budget.totalFunds < cost)
				return ToolResult.INSUFFICIENT_FUNDS;

			if (xpos + 1 < city.getWidth())
			{
				char eTile = neutralizeRoad(city.getTile(xpos + 1, ypos));
				if (eTile == VRAILROAD ||
					eTile == HBRIDGE ||
					(eTile >= TileConstants.ROADS && eTile <= HROADPOWER))
				{
					city.setTile(xpos, ypos, (char) (HBRIDGE | BULLBIT));
					break;
				}
			}

			if (xpos > 0)
			{
				char wTile = neutralizeRoad(city.getTile(xpos - 1, ypos));
				if (wTile == VRAILROAD ||
					wTile == HBRIDGE ||
					(wTile >= TileConstants.ROADS && wTile <= INTERSECTION))
				{
					city.setTile(xpos, ypos, (char) (HBRIDGE | BULLBIT));
					break;
				}
			}

			if (ypos + 1 < city.getHeight())
			{
				char sTile = neutralizeRoad(city.getTile(xpos, ypos + 1));
				if (sTile == HRAILROAD ||
					sTile == VROADPOWER ||
					(sTile >= VBRIDGE && sTile <= INTERSECTION))
				{
					city.setTile(xpos, ypos, (char) (VBRIDGE | BULLBIT));
					break;
				}
			}

			if (ypos > 0)
			{
				char nTile = neutralizeRoad(city.getTile(xpos, ypos - 1));
				if (nTile == HRAILROAD ||
					nTile == VROADPOWER ||
					(nTile >= VBRIDGE && nTile <= INTERSECTION))
				{
					city.setTile(xpos, ypos, (char) (VBRIDGE | BULLBIT));
					break;
				}
			}

			// cannot do road here
			return ToolResult.NONE;

		case LHPOWER: //road on power
			city.setTile(xpos, ypos, (char) (VROADPOWER | CONDBIT | BURNBIT | BULLBIT));
			break;

		case LVPOWER: //road on power #2
			city.setTile(xpos, ypos, (char) (HROADPOWER | CONDBIT | BURNBIT | BULLBIT));
			break;

		case LHRAIL: //road on rail
			city.setTile(xpos, ypos, (char) (HRAILROAD | BURNBIT | BULLBIT));
			break;

		case LVRAIL: //road on rail #2
			city.setTile(xpos, ypos, (char) (VRAILROAD | BURNBIT | BULLBIT));
			break;

		default:
			if (tile != DIRT) {
				if (city.autoBulldoze && canAutoBulldozeRRW(tile)) {
					cost += 1; //autodoze cost
				}
				else {
					// cannot do road here
					return ToolResult.NONE;
				}
			}

			// road on dirt
			city.setTile(xpos, ypos, (char) (TileConstants.ROADS | BULLBIT | BURNBIT));
			break;
		}
	
		city.spend(cost);
		return ToolResult.SUCCESS;
	}

	private ToolResult layWire(int xpos, int ypos)
	{
		final int WIRE_COST = 5;
		final int UNDERWATER_WIRE_COST = 25;

		int cost = WIRE_COST;
		if (city.budget.totalFunds < cost)
			return ToolResult.INSUFFICIENT_FUNDS;

		char tile = (char) (city.getTile(xpos, ypos) & LOMASK);
		tile = neutralizeRoad(tile);

		switch (tile)
		{
		case RIVER:		// wire on water
		case REDGE:
		case CHANNEL:

			cost = UNDERWATER_WIRE_COST;
			if (city.budget.totalFunds < cost)
				return ToolResult.INSUFFICIENT_FUNDS;

			if (xpos + 1 < city.getWidth())
			{
				char tmp = city.getTile(xpos + 1, ypos);
				char tmpn = neutralizeRoad(tmp);

				if ((tmp & CONDBIT) != 0 &&
					tmpn != HROADPOWER &&
					tmpn != RAILHPOWERV &&
					tmpn != HPOWER)
				{
					city.setTile(xpos, ypos, (char) (VPOWER | CONDBIT | BULLBIT));
					break;
				}
			}

			if (xpos > 0)
			{
				char tmp = city.getTile(xpos - 1, ypos);
				char tmpn = neutralizeRoad(tmp);

				if ((tmp & CONDBIT) != 0 &&
					tmpn != HROADPOWER &&
					tmpn != RAILHPOWERV &&
					tmpn != HPOWER)
				{
					city.setTile(xpos, ypos, (char) (VPOWER | CONDBIT | BULLBIT));
					break;
				}
			}

			if (ypos + 1 < city.getHeight())
			{
				char tmp = city.getTile(xpos, ypos + 1);
				char tmpn = neutralizeRoad(tmp);

				if ((tmp & CONDBIT) != 0 &&
					tmpn != VROADPOWER &&
					tmpn != RAILVPOWERH &&
					tmpn != VPOWER)
				{
					city.setTile(xpos, ypos, (char) (HPOWER | CONDBIT | BULLBIT));
					break;
				}
			}

			if (ypos > 0)
			{
				char tmp = city.getTile(xpos, ypos - 1);
				char tmpn = neutralizeRoad(tmp);

				if ((tmp & CONDBIT) != 0 &&
					tmpn != VROADPOWER &&
					tmpn != RAILVPOWERH &&
					tmpn != VPOWER)
				{
					city.setTile(xpos, ypos, (char) (HPOWER | CONDBIT | BULLBIT));
					break;
				}
			}

			// cannot do wire here
			return ToolResult.NONE;

		case TileConstants.ROADS: // wire on E/W road
			city.setTile(xpos, ypos, (char) (HROADPOWER | CONDBIT | BURNBIT | BULLBIT));
			break;

		case ROADS2: // wire on N/S road
			city.setTile(xpos, ypos, (char) (VROADPOWER | CONDBIT | BURNBIT | BULLBIT));
			break;

		case LHRAIL:	// wire on E/W railroad tracks
			city.setTile(xpos, ypos, (char) (RAILHPOWERV | CONDBIT | BURNBIT | BULLBIT));
			break;

		case LVRAIL:	// wire on N/S railroad tracks
			city.setTile(xpos, ypos, (char) (RAILVPOWERH | CONDBIT | BURNBIT | BULLBIT));
			break;

		default:
			if (tile != DIRT) {
				if (city.autoBulldoze && canAutoBulldozeRRW(tile)) {
					cost += 1; //autodoze cost
				}
				else {
					//cannot do wire here
					return ToolResult.NONE;
				}
			}

			//wire on dirt
			city.setTile(xpos, ypos, (char) (LHPOWER | CONDBIT | BULLBIT | BURNBIT));
			break;
		}

		city.spend(cost);
		return ToolResult.SUCCESS;
	}

	private void fixZone(int xpos, int ypos)
	{
		fixSingle(xpos, ypos);
		if (ypos > 0)
			fixSingle(xpos, ypos - 1);
		if (xpos > 0)
			fixSingle(xpos - 1, ypos);
		if (xpos + 1 < city.getWidth())
			fixSingle(xpos + 1, ypos);
		if (ypos + 1 < city.getHeight())
			fixSingle(xpos, ypos + 1);
	}

	private void fixSingle(int xpos, int ypos)
	{
		char tile = (char) (city.getTile(xpos, ypos) & LOMASK);
		tile = neutralizeRoad(tile);

		if (tile >= TileConstants.ROADS && tile <= INTERSECTION)
		{
			// cleanup road
			int adjTile = 0;

			if (ypos > 0)
			{
				tile = city.getTile(xpos, ypos - 1);
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

			if (xpos + 1 < city.getWidth())
			{
				tile = city.getTile(xpos + 1, ypos);
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

			if (ypos + 1 < city.getHeight())
			{
				tile = city.getTile(xpos, ypos + 1);
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
				tile = city.getTile(xpos - 1, ypos);
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

			city.setTile(xpos, ypos, (char)(RoadTable[adjTile] | BULLBIT | BURNBIT));
			return;
		} //endif on a road tile

		if (tile >= LHRAIL && tile <= LVRAIL10)
		{
			// cleanup Rail
			int adjTile = 0;

			if (ypos > 0)
			{
				tile = city.getTile(xpos, ypos - 1);
				tile = neutralizeRoad(tile);
				if (tile >= RAILHPOWERV && tile <= VRAILROAD &&
					tile != RAILHPOWERV &&
					tile != HRAILROAD &&
					tile != HRAIL)
				{
					adjTile |= 1;
				}
			}

			if (xpos + 1 < city.getWidth())
			{
				tile = city.getTile(xpos + 1, ypos);
				tile = neutralizeRoad(tile);
				if (tile >= RAILHPOWERV && tile <= VRAILROAD &&
					tile != RAILVPOWERH &&
					tile != VRAILROAD &&
					tile != VRAIL)
				{
					adjTile |= 2;
				}
			}

			if (ypos + 1 < city.getHeight())
			{
				tile = city.getTile(xpos, ypos + 1);
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
				tile = city.getTile(xpos - 1, ypos);
				tile = neutralizeRoad(tile);
				if (tile >= RAILHPOWERV && tile <= VRAILROAD &&
					tile != RAILVPOWERH &&
					tile != VRAILROAD &&
					tile != VRAIL)
				{
					adjTile |= 8;
				}
			}

			city.setTile(xpos, ypos, (char)(RailTable[adjTile] | BULLBIT | BURNBIT));
			return;
		} //end if on a rail tile

		if (tile >= LHPOWER && tile <= LVPOWER10)
		{
			// Cleanup Wire
			int adjTile = 0;

			if (ypos > 0)
			{
				tile = city.getTile(xpos, ypos - 1);
				char ntile = neutralizeRoad(tile);
				if ((tile & CONDBIT) != 0 &&
					ntile != VPOWER &&
					ntile != VROADPOWER &&
					ntile != RAILVPOWERH)
				{
					adjTile |= 1;
				}
			}

			if (xpos + 1 < city.getWidth())
			{
				tile = city.getTile(xpos + 1, ypos);
				char ntile = neutralizeRoad(tile);
				if ((tile & CONDBIT) != 0 &&
					ntile != HPOWER &&
					ntile != HROADPOWER &&
					ntile != RAILHPOWERV)
				{
					adjTile |= 2;
				}
			}

			if (ypos + 1 < city.getHeight())
			{
				tile = city.getTile(xpos, ypos + 1);
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
				tile = city.getTile(xpos - 1, ypos);
				char ntile = neutralizeRoad(tile);
				if ((tile & CONDBIT) != 0 &&
					ntile != HPOWER &&
					ntile != HROADPOWER &&
					ntile != RAILHPOWERV)
				{
					adjTile |= 8;
				}
			}

			city.setTile(xpos, ypos, (char)(WireTable[adjTile] | BULLBIT | BURNBIT | CONDBIT));
			return;
		} //end if on a rail tile
	}

	void putRubble(int xpos, int ypos, int w, int h)
	{
		for (int xx = xpos - 1; xx <= xpos + w-2; xx++) {
			for (int yy = ypos - 1; yy <= ypos + h-2; yy++) {
				if (city.testBounds(xx, yy)) {
					int tile = city.getTile(xx,yy) & LOMASK;
					if (tile != RADTILE && tile != DIRT) {
						int nTile = (TINYEXP + city.PRNG.nextInt(3))
							| ANIMBIT | BULLBIT;
						city.setTile(xx, yy, (char)nTile);
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
