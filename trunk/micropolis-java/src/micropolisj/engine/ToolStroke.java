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
	boolean inPreview;

	ToolStroke(Micropolis city, MicropolisTool tool, int xpos, int ypos)
	{
		this.city = city;
		this.tool = tool;
		this.xpos = xpos;
		this.ypos = ypos;
		this.xdest = xpos;
		this.ydest = ypos;
	}

	public final ToolPreview getPreview()
	{
		ToolEffect eff = new ToolEffect(city);
		inPreview = true;
		try {
			applyArea(eff);
		}
		finally {
			inPreview = false;
		}
		return eff.preview;
	}

	public final ToolResult apply()
	{
		ToolEffect eff = new ToolEffect(city);
		applyArea(eff);
		return eff.apply();
	}

	protected void applyArea(ToolEffectIfc eff)
	{
		Rectangle r = getBounds();

		for (int i = 0; i < r.height; i += tool.getHeight()) {
			for (int j = 0; j < r.width; j += tool.getWidth()) {
				apply1(new TranslatedToolEffect(eff, r.x+j, r.y+i));
			}
		}
	}

	boolean apply1(ToolEffectIfc eff)
	{
		switch (tool)
		{
		case PARK:
			return applyParkTool(eff);

		case RESIDENTIAL:
			return applyZone(eff, 3, 3, RESBASE);

		case COMMERCIAL:
			return applyZone(eff, 3, 3, COMBASE);

		case INDUSTRIAL:
			return applyZone(eff, 3, 3, INDBASE);

		case FIRE:
			return applyZone(eff, 3, 3, FIRESTBASE);

		case POLICE:
			return applyZone(eff, 3, 3, POLICESTBASE);

		case POWERPLANT:
			return applyZone(eff, 4, 4, COALBASE);

		case STADIUM:
			return applyZone(eff, 4, 4, STADIUMBASE);

		case SEAPORT:
			return applyZone(eff, 4, 4, PORTBASE);

		case NUCLEAR:
			return applyZone(eff, 4, 4, NUCLEARBASE);

		case AIRPORT:
			return applyZone(eff, 6, 6, AIRPORTBASE);

		default:
			// not expected
			throw new Error("unexpected tool: "+tool);
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

	public CityLocation getLocation()
	{
		return new CityLocation(xpos, ypos);
	}

	boolean applyZone(ToolEffectIfc eff, int width, int height, char tileBase)
	{
		int cost = tool.getToolCost();
		boolean canBuild = true;
		for (int rowNum = 0; rowNum < height; rowNum++) {
			for (int columnNum = 0; columnNum < width; columnNum++)
			{
				int tileValue = eff.getTile(columnNum, rowNum);
				tileValue = tileValue & LOMASK;

				if (tileValue != DIRT) {
					if (city.autoBulldoze && canAutoBulldozeZ((char)tileValue)) {
						cost++;
					}
					else {
						canBuild = false;
					}
				}
			}
		}
		if (!canBuild) {
			eff.toolResult(ToolResult.UH_OH);
			return false;
		}

		eff.spend(cost);

		for (int rowNum = 0; rowNum < height; rowNum++)
		{
			for (int columnNum = 0; columnNum < width; columnNum++)
			{
				eff.setTile(columnNum, rowNum, (char) (
					tileBase
					));
				tileBase++;
			}
		}

		fixBorder(eff, width, height);
		return true;
	}

	//compatible function
	void fixBorder(int left, int top, int right, int bottom)
	{
		ToolEffect eff = new ToolEffect(city, left, top);
		fixBorder(eff, right+1-left, bottom+1-top);
		eff.apply();
	}

	void fixBorder(ToolEffectIfc eff, int width, int height)
	{
		for (int x = 0; x < width; x++)
		{
			fixZone(new TranslatedToolEffect(eff, x, 0));
			fixZone(new TranslatedToolEffect(eff, x, height-1));
		}
		for (int y = 1; y < height - 1; y++)
		{
			fixZone(new TranslatedToolEffect(eff, 0, y));
			fixZone(new TranslatedToolEffect(eff, width-1, y));
		}
	}

	boolean applyParkTool(ToolEffectIfc eff)
	{
		int cost = tool.getToolCost();

		if (eff.getTile(0, 0) != DIRT) {
			// some sort of bulldozing is necessary
			if (!city.autoBulldoze) {
				eff.toolResult(ToolResult.UH_OH);
				return false;
			}

			//FIXME- use a canAutoBulldoze-style function here
			if (isRubble(eff.getTile(0, 0))) {
				// this tile can be auto-bulldozed
				cost++;
			}
			else {
				// cannot be auto-bulldozed
				eff.toolResult(ToolResult.UH_OH);
				return false;
			}
		}

		int z = inPreview ? 0 : city.PRNG.nextInt(5);
		int tile;
		if (z < 4) {
			tile = (WOODS2 + z) | BULLBIT;
		} else {
			tile = FOUNTAIN | BULLBIT;
		}

		eff.spend(cost);
		eff.setTile(0, 0, tile);

		return true;
	}

	protected void fixZone(int xpos, int ypos)
	{
		ToolEffect eff = new ToolEffect(city, xpos, ypos);
		fixZone(eff);
		eff.apply();
	}

	protected void fixZone(ToolEffectIfc eff)
	{
		fixSingle(eff);

		// "fix" the cells to the north, west, east, and south
		fixSingle(new TranslatedToolEffect(eff, 0, -1));
		fixSingle(new TranslatedToolEffect(eff, -1, 0));
		fixSingle(new TranslatedToolEffect(eff, 1, 0));
		fixSingle(new TranslatedToolEffect(eff, 0, 1));
	}

	private void fixSingle(ToolEffectIfc eff)
	{
		int tile = eff.getTile(0, 0);

		if (isRoadDynamic(tile))
		{
			// cleanup road
			int adjTile = 0;

			// check road to north
			if (roadConnectsSouth(eff.getTile(0, -1)))
			{
				adjTile |= 1;
			}

			// check road to east
			if (roadConnectsWest(eff.getTile(1, 0)))
			{
				adjTile |= 2;
			}

			// check road to south
			if (roadConnectsNorth(eff.getTile(0, 1)))
			{
				adjTile |= 4;
			}

			// check road to west
			if (roadConnectsEast(eff.getTile(-1, 0)))
			{
				adjTile |= 8;
			}

			eff.setTile(0, 0, (RoadTable[adjTile] | BULLBIT));
		} //endif on a road tile

		else if (isRailDynamic(tile))
		{
			// cleanup Rail
			int adjTile = 0;

			// check rail to north
			if (railConnectsSouth(eff.getTile(0, -1)))
			{
				adjTile |= 1;
			}

			// check rail to east
			if (railConnectsWest(eff.getTile(1, 0)))
			{
				adjTile |= 2;
			}

			// check rail to south
			if (railConnectsNorth(eff.getTile(0, 1)))
			{
				adjTile |= 4;
			}

			// check rail to west
			if (railConnectsEast(eff.getTile(-1, 0)))
			{
				adjTile |= 8;
			}

			eff.setTile(0, 0, (RailTable[adjTile] | BULLBIT));
		} //end if on a rail tile

		else if (isWireDynamic(tile))
		{
			// Cleanup Wire
			int adjTile = 0;

			// check wire to north
			if (wireConnectsSouth(eff.getTile(0, -1)))
			{
				adjTile |= 1;
			}

			// check wire to east
			if (wireConnectsWest(eff.getTile(1, 0)))
			{
				adjTile |= 2;
			}

			// check wire to south
			if (wireConnectsNorth(eff.getTile(0, 1)))
			{
				adjTile |= 4;
			}

			// check wire to west
			if (wireConnectsEast(eff.getTile(-1, 0)))
			{
				adjTile |= 8;
			}

			eff.setTile(0, 0, (WireTable[adjTile] | BULLBIT));
		} //end if on a rail tile

		return;
	}
}
