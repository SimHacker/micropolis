package micropolisj.engine;

public interface ToolEffectIfc
{
	/**
	 * Gets the tile at a relative location.
	 */
	int getTile(int dx, int dy);

	void makeSound(int dx, int dy, Sound sound);

	/**
	 * Sets the tile value at a relative location.
	 */
	void setTile(int dx, int dy, int tileValue);

	/**
	 * Deduct an amount from the controller's cash funds.
	 */
	void spend(int amount);

	void toolResult(ToolResult tr);
}
