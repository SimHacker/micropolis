// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

/**
 * Lists the various results that may occur when applying a tool.
 */
public enum ToolResult
{
	SUCCESS, // 1
	NONE,    // 0
	UH_OH,   // -1; invalid position
	INSUFFICIENT_FUNDS;   // -2
}
