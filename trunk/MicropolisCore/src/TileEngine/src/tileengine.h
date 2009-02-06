/* tileengine.h
 * Tile Engine include file
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */

/** @file tileengine.h
 * Tile Engine include file
 */

////////////////////////////////////////////////////////////////////////
// Includes


#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#ifdef _WIN32

#include <winsock2.h>
#include <sys/stat.h>
#include <time.h>

#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif

#else

#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>

#endif

#include <string>
#include <vector>
#include <map>

#include <cairo.h>
#include <pycairo.h>


////////////////////////////////////////////////////////////////////////
// Constants


#define TILEENGINE_VERSION      "1.0"


////////////////////////////////////////////////////////////////////////
// Macros


// This is missing from pycairo.h
#define PycairoSurface_GET(obj) (((PycairoSurface *)(obj))->surface)


////////////////////////////////////////////////////////////////////////
// Enumerated Types


/** Tile format codes for different tile data types. */
enum TileFormat {
  TILE_FORMAT_BYTE_SIGNED,
  TILE_FORMAT_BYTE_UNSIGNED,
  TILE_FORMAT_SHORT_SIGNED,
  TILE_FORMAT_SHORT_UNSIGNED,
  TILE_FORMAT_LONG_SIGNED,
  TILE_FORMAT_LONG_UNSIGNED,
  TILE_FORMAT_FLOAT,
  TILE_FORMAT_DOUBLE,
  TILE_FORMAT_COUNT,
};

////////////////////////////////////////////////////////////////////////
// Classes


class TileEngine {

public:

    int width;
    int height;
    void *bufData;
    int colBytes;
    int rowBytes;
    char tileFormat;
    float floatOffset;
    float floatScale;
    unsigned int tileShift;
    unsigned int tileMask;


    ////////////////////////////////////////////////////////////////////////


    TileEngine();

    ~TileEngine();


    ////////////////////////////////////////////////////////////////////////


    void setBuffer(
	void *buf);


    unsigned long getValue(
	int col,
	int row,
	PyObject *tileFunction = NULL,
	const int *tileMapData = NULL,
	unsigned int tileMapCount = 0);


    void renderTiles(
	cairo_t *ctx,
	cairo_surface_t *tilesSurf,
	int tilesWidth,
	int tilesHeight,
	PyObject *tileFunction,
	PyObject *tileMap,
	int tileSize,
	int renderCol,
	int renderRow,
	int renderCols,
	int renderRows,
	double alpha);


    void renderTilesLazy(
	cairo_t *ctx,
	PyObject *tileFunction,
	PyObject *tileMap,
	int tileSize,
	int renderCol,
	int renderRow,
	int renderCols,
	int renderRows,
	double alpha,
	PyObject *tileGenerator,
	PyObject *tileCache,
	PyObject *tileCacheSurfaces,
	PyObject *tileState);


    void renderPixels(
	cairo_surface_t *destSurf,
	cairo_surface_t *cmapSurf,
	PyObject *tileFunction,
	PyObject *tileMap,
	int renderCol,
	int renderRow,
	int renderCols,
	int renderRows);


    PyObject *getTileData(
	PyObject *tileFunction,
	PyObject *tileMap,
	int col,
	int row,
	int cols,
	int rows,
	int code,
	PyObject *tileViewCache);


    ////////////////////////////////////////////////////////////////////////


};


////////////////////////////////////////////////////////////////////////
