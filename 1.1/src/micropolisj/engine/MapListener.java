// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

/**
 * The listener interface for receiving notifications whenever a tile on
 * the city map changes, or when a sprite moves or changes.
 */
public interface MapListener
{
	/** Called whenever data for a specific overlay has changed. */
	void mapOverlayDataChanged(MapState overlayDataType);

	/** Called when a sprite moves. */
	void spriteMoved(Sprite sprite);

	/** Called when a map tile changes, including for animations. */
	void tileChanged(int xpos, int ypos);

	/** Called when the entire map should be reread and rendered. */
	void wholeMapChanged();
}
