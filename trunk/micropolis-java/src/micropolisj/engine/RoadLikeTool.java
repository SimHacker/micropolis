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
	protected void applyArea(ToolEffectIfc eff)
	{
		for (;;) {
			if (!applyForward(eff)) {
				break;
			}
			if (!applyBackward(eff)) {
				break;
			}
		}
	}

	boolean applyBackward(ToolEffectIfc eff)
	{
		boolean anyChange = false;

		Rectangle b = getBounds();
		for (int i = b.height - 1; i >= 0; i--) {
			for (int j = b.width - 1; j >= 0; j--) {
				TranslatedToolEffect tte = new TranslatedToolEffect(eff, b.x+j, b.y+i);
				anyChange = anyChange || applySingle(tte);
			}
		}
		return anyChange;
	}

	boolean applyForward(ToolEffectIfc eff)
	{
		boolean anyChange = false;

		Rectangle b = getBounds();
		for (int i = 0; i < b.height; i++) {
			for (int j = 0; j < b.width; j++) {
				TranslatedToolEffect tte = new TranslatedToolEffect(eff, b.x+j, b.y+i);
				anyChange = anyChange || applySingle(tte);
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

	boolean applySingle(ToolEffectIfc eff)
	{
		switch (tool)
		{
		case RAIL:
			return applyRailTool(eff);

		case ROADS:
			return applyRoadTool(eff);

		case WIRE:
			return applyWireTool(eff);

		default:
			throw new Error("Unexpected tool: " + tool);
		}
	}

	boolean applyRailTool(ToolEffectIfc eff)
	{
		if (layRail(eff)) {
			fixZone(eff);
			return true;
		}
		else {
			return false;
		}
	}

	boolean applyRoadTool(ToolEffectIfc eff)
	{
		if (layRoad(eff)) {
			fixZone(eff);
			return true;
		}
		else {
			return false;
		}
	}

	boolean applyWireTool(ToolEffectIfc eff)
	{
		if (layWire(eff)) {
			fixZone(eff);
			return true;
		}
		else {
			return false;
		}
	}

	private boolean layRail(ToolEffectIfc eff)
	{
		final int RAIL_COST = 20;
		final int TUNNEL_COST = 100;

		int cost = RAIL_COST;

		char tile = (char) (eff.getTile(0, 0) & LOMASK);
		tile = neutralizeRoad(tile);

		switch (tile)
		{
		case RIVER:		// rail on water
		case REDGE:
		case CHANNEL:

			cost = TUNNEL_COST;

			// check east
			{
				char eTile = neutralizeRoad(eff.getTile(1, 0));
				if (eTile == RAILHPOWERV ||
					eTile == HRAIL ||
					(eTile >= LHRAIL && eTile <= HRAILROAD))
				{
					eff.setTile(0, 0, (char) (HRAIL | BULLBIT));
					break;
				}
			}

			// check west
			{
				char wTile = neutralizeRoad(eff.getTile(-1, 0));
				if (wTile == RAILHPOWERV ||
					wTile == HRAIL ||
					(wTile > VRAIL && wTile < VRAILROAD))
				{
					eff.setTile(0, 0, (char) (HRAIL | BULLBIT));
					break;
				}
			}

			// check south
			{
				char sTile = neutralizeRoad(eff.getTile(0, 1));
				if (sTile == RAILVPOWERH ||
					sTile == VRAILROAD ||
					(sTile > HRAIL && sTile < HRAILROAD))
				{
					eff.setTile(0, 0, (char) (VRAIL | BULLBIT));
					break;
				}
			}

			// check north
			{
				char nTile = neutralizeRoad(eff.getTile(0, -1));
				if (nTile == RAILVPOWERH ||
					nTile == VRAILROAD ||
					(nTile > HRAIL && nTile < HRAILROAD))
				{
					eff.setTile(0, 0, (char) (VRAIL | BULLBIT));
					break;
				}
			}

			// cannot do road here
			return false;

		case LHPOWER: // rail on power
			eff.setTile(0, 0, (char) (RAILVPOWERH | CONDBIT | BULLBIT));
			break;

		case LVPOWER: // rail on power
			eff.setTile(0, 0, (char) (RAILHPOWERV | CONDBIT | BULLBIT));
			break;

		case TileConstants.ROADS:	// rail on road (case 1)
			eff.setTile(0, 0, (char) (VRAILROAD | BULLBIT));
			break;

		case ROADS2:	// rail on road (case 2)
			eff.setTile(0, 0, (char) (HRAILROAD | BULLBIT));
			break;

		default:
			if (tile != DIRT) {
				if (city.autoBulldoze && canAutoBulldozeRRW(tile)) {
					cost += 1; //autodoze cost
				}
				else {
					// cannot do rail here
					return false;
				}
			}

		  	//rail on dirt
			eff.setTile(0, 0, (char) (LHRAIL | BULLBIT));
			break;
		}

		eff.spend(cost);
		return true;
	}

	private boolean layRoad(ToolEffectIfc eff)
	{
		final int ROAD_COST = 10;
		final int BRIDGE_COST = 50;

		int cost = ROAD_COST;

		char tile = (char) (eff.getTile(0, 0) & LOMASK);
		switch (tile)
		{
		case RIVER:		// road on water
		case REDGE:
		case CHANNEL:	// check how to build bridges, if possible.

			cost = BRIDGE_COST;

			// check east
			{
				char eTile = neutralizeRoad(eff.getTile(1, 0));
				if (eTile == VRAILROAD ||
					eTile == HBRIDGE ||
					(eTile >= TileConstants.ROADS && eTile <= HROADPOWER))
				{
					eff.setTile(0, 0, (char) (HBRIDGE | BULLBIT));
					break;
				}
			}

			// check west
			{
				char wTile = neutralizeRoad(eff.getTile(-1, 0));
				if (wTile == VRAILROAD ||
					wTile == HBRIDGE ||
					(wTile >= TileConstants.ROADS && wTile <= INTERSECTION))
				{
					eff.setTile(0, 0, (char) (HBRIDGE | BULLBIT));
					break;
				}
			}

			// check south
			{
				char sTile = neutralizeRoad(eff.getTile(0, 1));
				if (sTile == HRAILROAD ||
					sTile == VROADPOWER ||
					(sTile >= VBRIDGE && sTile <= INTERSECTION))
				{
					eff.setTile(0, 0, (char) (VBRIDGE | BULLBIT));
					break;
				}
			}

			// check north
			{
				char nTile = neutralizeRoad(eff.getTile(0, -1));
				if (nTile == HRAILROAD ||
					nTile == VROADPOWER ||
					(nTile >= VBRIDGE && nTile <= INTERSECTION))
				{
					eff.setTile(0, 0, (char) (VBRIDGE | BULLBIT));
					break;
				}
			}

			// cannot do road here
			return false;

		case LHPOWER: //road on power
			eff.setTile(0, 0, (char) (VROADPOWER | CONDBIT | BULLBIT));
			break;

		case LVPOWER: //road on power #2
			eff.setTile(0, 0, (char) (HROADPOWER | CONDBIT | BULLBIT));
			break;

		case LHRAIL: //road on rail
			eff.setTile(0, 0, (char) (HRAILROAD | BULLBIT));
			break;

		case LVRAIL: //road on rail #2
			eff.setTile(0, 0, (char) (VRAILROAD | BULLBIT));
			break;

		default:
			if (tile != DIRT) {
				if (city.autoBulldoze && canAutoBulldozeRRW(tile)) {
					cost += 1; //autodoze cost
				}
				else {
					// cannot do road here
					return false;
				}
			}

			// road on dirt;
			// just build a plain road, fixZone will fix it.
			eff.setTile(0, 0, (char) (TileConstants.ROADS | BULLBIT));
			break;
		}
	
		eff.spend(cost);
		return true;
	}

	private boolean layWire(ToolEffectIfc eff)
	{
		final int WIRE_COST = 5;
		final int UNDERWATER_WIRE_COST = 25;

		int cost = WIRE_COST;

		char tile = (char) (eff.getTile(0, 0) & LOMASK);
		tile = neutralizeRoad(tile);

		switch (tile)
		{
		case RIVER:		// wire on water
		case REDGE:
		case CHANNEL:

			cost = UNDERWATER_WIRE_COST;

			// check east
			{
				int tmp = eff.getTile(1, 0);
				char tmpn = neutralizeRoad(tmp);

				if ((tmp & CONDBIT) != 0 &&
					tmpn != HROADPOWER &&
					tmpn != RAILHPOWERV &&
					tmpn != HPOWER)
				{
					eff.setTile(0, 0, (char) (VPOWER | CONDBIT | BULLBIT));
					break;
				}
			}

			// check west
			{
				int tmp = eff.getTile(-1, 0);
				char tmpn = neutralizeRoad(tmp);

				if ((tmp & CONDBIT) != 0 &&
					tmpn != HROADPOWER &&
					tmpn != RAILHPOWERV &&
					tmpn != HPOWER)
				{
					eff.setTile(0, 0, (char) (VPOWER | CONDBIT | BULLBIT));
					break;
				}
			}

			// check south
			{
				int tmp = eff.getTile(0, 1);
				char tmpn = neutralizeRoad(tmp);

				if ((tmp & CONDBIT) != 0 &&
					tmpn != VROADPOWER &&
					tmpn != RAILVPOWERH &&
					tmpn != VPOWER)
				{
					eff.setTile(0, 0, (char) (HPOWER | CONDBIT | BULLBIT));
					break;
				}
			}

			// check north
			{
				int tmp = eff.getTile(0, -1);
				char tmpn = neutralizeRoad(tmp);

				if ((tmp & CONDBIT) != 0 &&
					tmpn != VROADPOWER &&
					tmpn != RAILVPOWERH &&
					tmpn != VPOWER)
				{
					eff.setTile(0, 0, (char) (HPOWER | CONDBIT | BULLBIT));
					break;
				}
			}

			// cannot do wire here
			return false;

		case TileConstants.ROADS: // wire on E/W road
			eff.setTile(0, 0, (char) (HROADPOWER | CONDBIT | BULLBIT));
			break;

		case ROADS2: // wire on N/S road
			eff.setTile(0, 0, (char) (VROADPOWER | CONDBIT | BULLBIT));
			break;

		case LHRAIL:	// wire on E/W railroad tracks
			eff.setTile(0, 0, (char) (RAILHPOWERV | CONDBIT | BULLBIT));
			break;

		case LVRAIL:	// wire on N/S railroad tracks
			eff.setTile(0, 0, (char) (RAILVPOWERH | CONDBIT | BULLBIT));
			break;

		default:
			if (tile != DIRT) {
				if (city.autoBulldoze && canAutoBulldozeRRW(tile)) {
					cost += 1; //autodoze cost
				}
				else {
					//cannot do wire here
					return false;
				}
			}

			//wire on dirt
			eff.setTile(0, 0, (char) (LHPOWER | CONDBIT | BULLBIT));
			break;
		}

		eff.spend(cost);
		return true;
	}
}
