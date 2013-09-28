// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

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
	public void makeSound(int x, int y, Sound sound)
	{
		base.makeSound(x+dx, y+dy, sound);
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

	//implements ToolEffectIfc
	public void toolResult(ToolResult tr)
	{
		base.toolResult(tr);
	}
}
