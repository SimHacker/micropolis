package micropolisj.engine;

import java.util.Arrays;
import static micropolisj.engine.TileConstants.*;

public class ToolPreview implements ToolEffectIfc
{
	public int originX;
	public int originY;
	public int offsetX;
	public int offsetY;
	public short [][] tiles;
	public int cost;

	ToolPreview(int x, int y)
	{
		this.originX = x;
		this.originY = y;
		this.tiles = new short[1][1];
		this.tiles[0][0] = CLEAR;
	}

	//implements ToolEffectIfc
	public int getTile(int dx, int dy)
	{
		if (inRange(dx, dy)) {
			return tiles[offsetY+dy][offsetX+dx];
		}
		else {
			return CLEAR;
		}
	}

	boolean inRange(int dx, int dy)
	{
		return offsetY+dy >= 0 &&
			offsetY+dy < tiles.length &&
			offsetX+dx >= 0 &&
			offsetX+dx < tiles[0].length;
	}

	void expandTo(int dx, int dy)
	{
		for (int i = 0; i < tiles.length; i++) {
			short[] A = tiles[i];
			if (offsetX+dx >= A.length) {
				int newLen = offsetX+dx+1;
				short[] AA = new short[newLen];
				System.arraycopy(A, 0, AA, 0, A.length);
				Arrays.fill(AA, A.length, newLen, CLEAR);
				tiles[i] = AA;
			}
			else if (offsetX+dx < 0) {
				int addl = -(offsetX+dx);
				int newLen = A.length + addl;
				short[] AA = new short[newLen];
				System.arraycopy(A, 0, AA, addl, A.length);
				Arrays.fill(AA, 0, addl, CLEAR);
				tiles[i] = AA;
			}
		}

		if (offsetX+dx < 0) {
			int addl = -(offsetX+dx);
			offsetX += addl;
		}

		if (offsetY+dy >= tiles.length) {
			int newLen = offsetY+dy+1;
			short[][] newTiles = new short[newLen][tiles[0].length];
			System.arraycopy(tiles, 0, newTiles, 0, tiles.length);
			for (int i = tiles.length; i < newLen; i++) {
				Arrays.fill(newTiles[i], CLEAR);
			}
			tiles = newTiles;
		}
		else if (offsetY+dy < 0) {
			int addl = -(offsetY+dy);
			int newLen = tiles.length + addl;
			short[][] newTiles = new short[newLen][tiles[0].length];
			System.arraycopy(tiles, addl, newTiles, 0, tiles.length);
			for (int i = 0; i < addl; i++) {
				Arrays.fill(newTiles[i], CLEAR);
			}
			tiles = newTiles;

			offsetY += addl;
		}
	}

	//implements ToolEffectIfc
	public void setTile(int dx, int dy, int tileValue)
	{
		expandTo(dx, dy);
		tiles[offsetY+dy][offsetX+dx] = (short)tileValue;
	}

	//implements ToolEffectIfc
	public void spend(int amount)
	{
		cost += amount;
	}
}
