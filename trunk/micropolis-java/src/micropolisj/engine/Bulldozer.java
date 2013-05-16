package micropolisj.engine;

import java.awt.Rectangle;
import static micropolisj.engine.TileConstants.*;

class Bulldozer extends ToolStroke
{
	Bulldozer(Micropolis city, int xpos, int ypos)
	{
		super(city, MicropolisTool.BULLDOZER, xpos, ypos);
	}

	@Override
	public ToolResult apply()
	{
		ToolResult checkResult = check();
		if (checkResult != ToolResult.SUCCESS) {
			return checkResult;
		}

		Rectangle b = getBounds();
		if (b.width == 1 && b.height == 1 &&
			isZoneCenter(city.getTile(b.x, b.y)))
		{
			return dozeZone(b.x, b.y);
		}

		int countDozed = 0;
		for (int y = b.y; y < b.y+b.height; y++) {
			for (int x = b.x; x < b.x+b.width; x++) {

				char tile = city.getTile(x, y);
				if (isDozeable(tile)) {
					countDozed++;

					dozeField(x, y);
				}

			}
		}
		return ToolResult.SUCCESS;
	}

	@Override
	public ToolResult check()
	{
		Rectangle b = getBounds();
		if (b.width == 1 && b.height == 1 &&
			isZoneCenter(city.getTile(b.x, b.y)))
		{
			int cost = 1;
			return city.budget.totalFunds >= cost ?
				ToolResult.SUCCESS :
				ToolResult.INSUFFICIENT_FUNDS;
		}

		int countDozed = 0;
		for (int y = b.y; y < b.y+b.height; y++) {
			for (int x = b.x; x < b.x+b.width; x++) {

				char tile = city.getTile(x, y);
				if (isDozeable(tile)) {
					countDozed++;
				}

			}
		}

		int cost = 1 * countDozed;
		return city.budget.totalFunds < cost ? ToolResult.INSUFFICIENT_FUNDS :
			countDozed != 0 ? ToolResult.SUCCESS :
			ToolResult.NONE;
	}

	ToolResult dozeZone(int xpos, int ypos)
	{
		assert city.testBounds(xpos, ypos);

		char currTile = city.getTile(xpos, ypos);
		char tmp = (char)(currTile & LOMASK);

		// zone center bit is set
		assert isZoneCenter(currTile);

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

	ToolResult dozeField(int xpos, int ypos)
	{
		char tile = city.getTile(xpos, ypos);

		// check dozeable bit
		assert isDozeable(tile);

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

}
