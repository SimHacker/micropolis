package micropolisj.engine;

import static micropolisj.engine.TileConstants.CLEAR;

class ToolEffect implements ToolEffectIfc
{
	final Micropolis city;
	final ToolPreview preview;
	final int originX;
	final int originY;

	ToolEffect(Micropolis city, int xpos, int ypos)
	{
		this.city = city;
		this.preview = new ToolPreview();
		this.originX = xpos;
		this.originY = ypos;
	}

	//implements ToolEffectIfc
	public int getTile(int dx, int dy)
	{
		int c = preview.getTile(dx, dy);
		if (c != CLEAR) {
			return c;
		}

		if (city.testBounds(originX + dx, originY + dy)) {
			return city.getTile(originX + dx, originY + dy);
		}
		else {
			return CLEAR;
		}
	}

	//implements ToolEffectIfc
	public void setTile(int dx, int dy, int tileValue)
	{
		preview.setTile(dx, dy, tileValue);
	}

	//implements ToolEffectIfc
	public void spend(int amount)
	{
		preview.spend(amount);
	}

	ToolResult apply()
	{
		if (originX - preview.offsetX < 0 ||
			originX - preview.offsetX >= city.getWidth() ||
			originY - preview.offsetY < 0 ||
			originY - preview.offsetY >= city.getHeight())
		{
			return ToolResult.UH_OH;
		}

		if (city.budget.totalFunds < preview.cost) {
			return ToolResult.INSUFFICIENT_FUNDS;
		}

		boolean anyFound = false;
		for (int y = 0; y < preview.tiles.length; y++) {
			for (int x = 0; x < preview.tiles[y].length; x++) {
				int c = preview.tiles[y][x];
				if (c != CLEAR) {
					city.setTile(originX + x - preview.offsetX, originY + y - preview.offsetY, (char) c);
					anyFound = true;
				}
			}
		}

		if (anyFound && preview.cost != 0) {
			city.spend(preview.cost);
			return ToolResult.SUCCESS;
		}
		else {
			return ToolResult.NONE;
		}
	}
}
