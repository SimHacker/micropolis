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

	public ToolStroke beginStroke(Micropolis engine, int xpos, int ypos)
	{
		return new ToolStroke(engine, this, xpos, ypos);
	}

	public ToolResult apply(Micropolis engine, int xpos, int ypos)
	{
		return beginStroke(engine, xpos, ypos).apply();
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
}
