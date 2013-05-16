package micropolisj.engine;

import java.awt.Rectangle;
import static micropolisj.engine.TileConstants.*;

class RoadLikeTool extends ToolStroke
{
	RoadLikeTool(Micropolis city, MicropolisTool tool, int xpos, int ypos)
	{
		super(city, tool, xpos, ypos);
	}

	@Override
	public ToolResult apply()
	{
		ToolResult checkResult = check();
		if (checkResult != ToolResult.SUCCESS) {
			return checkResult;
		}

		for (;;) {
			if (!applyForward()) {
				return checkResult;
			}
			if (!applyBackward()) {
				return checkResult;
			}
		}
	}

	boolean applyBackward()
	{
		boolean anyChange = false;

		Rectangle b = getBounds();
		for (int i = b.height - 1; i >= 0; i--) {
			for (int j = b.width - 1; j >= 0; j--) {
				anyChange = anyChange || applySingle(b.x + j, b.y + i);
			}
		}
		return anyChange;
	}

	boolean applyForward()
	{
		boolean anyChange = false;

		Rectangle b = getBounds();
		for (int i = 0; i < b.height; i++) {
			for (int j = 0; j < b.width; j++) {
				anyChange = anyChange || applySingle(b.x + j, b.y + i);
			}
		}
		return anyChange;
	}

	@Override
	public Rectangle getBounds()
	{
		// constrain bounds to be a rectangle with
		// either width or height equal to one.

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

	boolean applySingle(int xpos, int ypos)
	{
		switch (tool)
		{
		case RAIL:
			return applyRailTool(xpos, ypos);

		case ROADS:
			return applyRoadTool(xpos, ypos);

		case WIRE:
			return applyWireTool(xpos, ypos);

		default:
			throw new Error("Unexpected tool: " + tool);
		}
	}

	boolean applyRailTool(int xpos, int ypos)
	{
		assert city.testBounds(xpos, ypos);

		ToolResult result = layRail(xpos, ypos);
		fixZone(xpos, ypos);
		return result == ToolResult.SUCCESS;
	}

	boolean applyRoadTool(int xpos, int ypos)
	{
		assert city.testBounds(xpos, ypos);

		ToolResult result = layRoad(xpos, ypos);
		fixZone(xpos, ypos);
		return result == ToolResult.SUCCESS;
	}

	boolean applyWireTool(int xpos, int ypos)
	{
		assert city.testBounds(xpos, ypos);

		ToolResult result = layWire(xpos, ypos);
		fixZone(xpos, ypos);
		return result == ToolResult.SUCCESS;
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
}
