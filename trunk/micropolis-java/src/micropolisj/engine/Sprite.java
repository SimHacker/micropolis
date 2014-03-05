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
 * Represents a mobile entity on the city map, such as a tornado
 * or a train. There can be any number present in a city, and each one
 * gets a chance to act on every tick of the simulation.
 *
 * @see Micropolis#moveObjects
 */
public abstract class Sprite
{
	Micropolis city;

	//TODO- enforce read-only nature of the following properties
	// (i.e. do not let them be modified directly by other classes)

	public SpriteKind kind;

	public int offx;
	public int offy;
	public int width = 32;
	public int height = 32;

	public int frame;
	public int x;
	public int y;

	public int lastX;
	public int lastY;

	int dir;

	protected Sprite(Micropolis engine, SpriteKind kind)
	{
		this.city = engine;
		this.kind = kind;
	}

	protected final int getChar(int x, int y)
	{
		int xpos = x / 16;
		int ypos = y / 16;
		if (city.testBounds(xpos, ypos)) {
			return city.getTile(xpos, ypos);
		} else {
			return -1;
		}
	}

	/**
	 * For subclasses to override. Actually does the movement and animation
	 * of this particular sprite. Setting this.frame to zero will cause the
	 * sprite to be unallocated.
	 */
	protected abstract void moveImpl();

	/**
	 * Perform this agent's movement and animation.
	 */
	public final void move()
	{
		lastX = x;
		lastY = y;
		moveImpl();
		city.fireSpriteMoved(this);
	}

	/**
	 * Tells whether this sprite is visible.
	 */
	public final boolean isVisible()
	{
		return this.frame != 0;
	}

	/**
	 * Computes direction from one point to another.
	 * @return integer between 1 and 8, with
	 *          1 == north,
	 *          3 == east,
	 *          5 == south,
	 *          7 == west.
	 */
	static final int getDir(int orgX, int orgY, int desX, int desY)
	{
		final int Gdtab [] = { 0, 3, 2, 1, 3, 4, 5, 7, 6, 5, 7, 8, 1 };
		int dispX = desX - orgX;
		int dispY = desY - orgY;

		int z = dispX < 0 ? (dispY < 0 ? 11 : 8) : (dispY < 0 ? 2 : 5);

		dispX = Math.abs(dispX);
		dispY = Math.abs(dispY);
		int absDist = dispX + dispY;

		if (dispX * 2 < dispY)      z++;
		else if (dispY * 2 < dispX) z--;

		if (z >= 1 && z <= 12) {
			return Gdtab[z];
		}
		else {
			assert false;
			return 0;
		}
	}

	/**
	 * Computes manhatten distance between two points.
	 */
	static final int getDis(int x0, int y0, int x1, int y1)
	{
		return Math.abs(x0-x1) + Math.abs(y0-y1);
	}

	/**
	 * Replaces this sprite with an exploding sprite.
	 */
	final void explodeSprite()
	{
		this.frame = 0;

		city.makeExplosionAt(x, y);
		int xpos = x/16;
		int ypos = y/16;

		switch (kind) {
		case AIR:
			city.crashLocation = new CityLocation(xpos, ypos);
			city.sendMessageAt(MicropolisMessage.PLANECRASH_REPORT, xpos, ypos);
			break;
		case SHI:
			city.crashLocation = new CityLocation(xpos, ypos);
			city.sendMessageAt(MicropolisMessage.SHIPWRECK_REPORT, xpos, ypos);
			break;
		case TRA:
		case BUS:
			city.crashLocation = new CityLocation(xpos, ypos);
			city.sendMessageAt(MicropolisMessage.TRAIN_CRASH_REPORT, xpos, ypos);
			break;
		case COP:
			city.crashLocation = new CityLocation(xpos, ypos);
			city.sendMessageAt(MicropolisMessage.COPTER_CRASH_REPORT, xpos, ypos);
			break;
		}

		city.makeSound(xpos, ypos, Sound.EXPLOSION_HIGH);
	}

	/**
	 * Checks whether another sprite is in collision ranges.
	 * @return true iff the sprite is in collision range
	 */
	final boolean checkSpriteCollision(Sprite otherSprite)
	{
		if (!isVisible()) return false;
		if (!otherSprite.isVisible()) return false;

		return (getDis(this.x, this.y, otherSprite.x, otherSprite.y) < 30);
	}

	/**
	 * Destroys whatever is at the specified location,
	 * replacing it with fire, rubble, or water as appropriate.
	 */
	final void destroyTile(int xpos, int ypos)
	{
		if (!city.testBounds(xpos, ypos))
			return;

		int t = city.getTile(xpos, ypos);

		if (t >= TREEBASE) {
			if (isBridge(t)) {
				city.setTile(xpos, ypos, RIVER);
				return;
			}
			if (!isCombustible(t)) {
				return; //cannot destroy it
			}
			if (isZoneCenter(t)) {
				city.killZone(xpos, ypos, t);
				if (t > RZB) {
					city.makeExplosion(xpos, ypos);
				}
			}
			if (checkWet(t)) {
				city.setTile(xpos, ypos, RIVER);
			}
			else {
				city.setTile(xpos, ypos, TINYEXP);
			}
		}
	}

	/**
	 * Helper function for rotating a sprite.
	 * @param p the sprite's current attitude (1-8)
	 * @param d the desired attitude (1-8)
	 * @return the new attitude
	 */
	static final int turnTo(int p, int d)
	{
		if (p == d)
			return p;
		if (p < d) {
			if (d - p < 4) p++;
			else           p--;
		}
		else {
			if (p - d < 4) p--;
			else           p++;
		}
		if (p > 8) return 1;
		if (p < 1) return 8;
		return p;
	}

}
