package micropolisj.engine;

class TranslatedToolEffect implements ToolEffectIfc
{
	final ToolEffectIfc base;
	final int dx;
	final int dy;

	TranslatedToolEffect(ToolEffectIfc base, int dx, int dy)
	{
		this.base = base;
		this.dx = dx;
		this.dy = dy;
	}

	//implements ToolEffectIfc
	public int getTile(int x, int y)
	{
		return base.getTile(x+dx, y+dy);
	}

	//implements ToolEffectIfc
	public void setTile(int x, int y, int tileValue)
	{
		base.setTile(x+dx, y+dy, tileValue);
	}

	//implements ToolEffectIfc
	public void spend(int amount)
	{
		base.spend(amount);
	}
}
