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
		ToolResult checkResult = check();
		if (checkResult != ToolResult.SUCCESS) {
			return checkResult;
		}

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

	ToolResult checkZoneTool()
	{
		Rectangle b = getBounds();
		int cost = 0;
		int numPlaced = 0;

		for (int i = b.y; i < b.y + b.height; i += tool.getHeight()) {
			for (int j = b.x; j < b.x + b.width; j += tool.getWidth()) {
				int x = j + (tool.getWidth() >= 3 ? 1 : 0);
				int y = i + (tool.getHeight() >= 3 ? 1 : 0);
				int c = costToPlaceZone(x, y);

				if (c != Integer.MAX_VALUE) {
					numPlaced++;
					cost += c;
				}
			}
		}

		return numPlaced == 0 ? ToolResult.NONE :
			cost <= city.budget.totalFunds ? ToolResult.SUCCESS :
			ToolResult.INSUFFICIENT_FUNDS;
	}

	/**
	 * @return SUCCESS if the operation will succeed, and at least
	 * one tile will be changed;
	 * INSUFFICIENT_FUNDS in case there's not enough money,
	 * and NONE is it is a null op.
	 */
	ToolResult check()
	{
		switch (tool)
		{
	//	case RAIL:
	//	case ROADS:
	//	case WIRE:
	//		return checkRailRoadWireTool();
	//	case PARK:
	//		return checkParkTool();
		default:
			return checkZoneTool();
		}
			
	}

	ToolResult apply1(int xpos, int ypos)
	{
		switch (tool)
		{
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

	public Rectangle getBounds()
	{
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

	int costToPlaceZone(int xpos, int ypos)
	{
		final int width = tool.getWidth();
		final int height = tool.getHeight();

		int cost = tool.getToolCost();
		boolean canBuild = true;
		for (int rowNum = 0; rowNum < height; rowNum++) {
			for (int colNum = 0; colNum < width; colNum++) {
				int x = xpos - 1 + colNum;
				int y = ypos - 1 + rowNum;

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

		return canBuild ? cost :
			Integer.MAX_VALUE;
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

	static char neutralizeRoad(char tile)
	{
		tile &= LOMASK;
		if (tile >= 64 && tile <= 207)
			tile = (char)( (tile & 0xf) + 64 );
		return tile;
	}

	protected void fixZone(int xpos, int ypos)
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
