/* connect.cpp
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

/** @file connect.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"
#include "tool.h"
#include "position.h"

////////////////////////////////////////////////////////////////////////


static const unsigned short RoadTable[16] = {
    ROADS, ROADS2, ROADS, ROADS3,
    ROADS2, ROADS2, ROADS4, ROADS8,
    ROADS, ROADS6, ROADS, ROADS7,
    ROADS5, ROADS10, ROADS9, INTERSECTION
};

static const unsigned short RailTable[16] = {
    LHRAIL, LVRAIL, LHRAIL, LVRAIL2,
    LVRAIL, LVRAIL, LVRAIL3, LVRAIL7,
    LHRAIL, LVRAIL5, LHRAIL, LVRAIL6,
    LVRAIL4, LVRAIL9, LVRAIL8, LVRAIL10
};

static const unsigned short WireTable[16] = {
    LHPOWER, LVPOWER, LHPOWER, LVPOWER2,
    LVPOWER, LVPOWER, LVPOWER3, LVPOWER7,
    LHPOWER, LVPOWER5, LHPOWER, LVPOWER6,
    LVPOWER4, LVPOWER9, LVPOWER8, LVPOWER10
};


////////////////////////////////////////////////////////////////////////

/**
 * Remove road from the tile.
 * @param tile Current tile value.
 * @return Equivalent tile without road.
 */
static inline MapTile neutralizeRoad(MapTile tile)
{
    if (tile >= 64 && tile <= 207) {
        tile = (tile & 0x000F) + 64;
    }
    return tile;
};

/**
 * Perform the command, and fix wire/road/rail/zone connections around it.
 * @param x   X world position to perform the command.
 * @param y   Y world position to perform the command.
 * @param cmd Command to perform.
 * @return Tool result.
 *
 * @note Temporary function to prevent breaking a lot of code.
 */
ToolResult Micropolis::connectTile(short x, short y, ConnectTileCommand cmd)
{
    ToolEffects effects(this);

    effects.clear();

    ToolResult result = connectTile(x, y, cmd, &effects);
    if (result == TOOLRESULT_OK) {
        if (effects.modifyIfEnoughFunding()) {
            return TOOLRESULT_OK;
        }

        return TOOLRESULT_NO_MONEY;
    }

    return result;
}

/**
 * Perform the command, and fix wire/road/rail/zone connections around it.
 * Store modification in the \a effects object.
 * @param x      X world position to perform the command.
 * @param y      Y world position to perform the command.
 * @param cmd    Command to perform.
 * @param effects Modification collecting object.
 * @return Tool result.
 */
ToolResult Micropolis::connectTile(short x, short y,
                                   ConnectTileCommand cmd, ToolEffects *effects)
{
    ToolResult result = TOOLRESULT_OK;

    // Make sure the array subscripts are in bounds.
    if (!testBounds(x, y)) {
        return TOOLRESULT_FAILED;
    }

    // Perform auto-doze if appropriate.
    switch (cmd) {

        case CONNECT_TILE_ROAD:
        case CONNECT_TILE_RAILROAD:
        case CONNECT_TILE_WIRE:

            // Silently skip auto-bulldoze if no money.
            if (autoBulldoze) {

                MapValue mapVal = effects->getMapValue(x, y);

                if (mapVal & BULLBIT) {
                    mapVal &= LOMASK;
                    mapVal = neutralizeRoad(mapVal);

                    /* Maybe this should check BULLBIT instead of checking tile values? */
                    if ((mapVal >= TINYEXP && mapVal <= LASTTINYEXP) ||
                            (mapVal < HBRIDGE && mapVal != DIRT)) {

                        effects->addCost(1);

                        effects->setMapValue(x, y, DIRT);

                    }
                }
            }
            break;

        default:
            // Do nothing.
            break;

    }

    // Perform the command.
    switch (cmd) {

    case CONNECT_TILE_FIX: // Fix zone.
        fixZone(x, y, effects);
        break;

    case CONNECT_TILE_BULLDOZE: // Bulldoze zone.
        result = layDoze(x, y, effects);
        fixZone(x, y, effects);
        break;

    case CONNECT_TILE_ROAD: // Lay road.
        result = layRoad(x, y, effects);
        fixZone(x, y, effects);
        break;

    case CONNECT_TILE_RAILROAD: // Lay railroad.
        result = layRail(x, y, effects);
        fixZone(x, y, effects);
        break;

    case CONNECT_TILE_WIRE: // Lay wire.
        result = layWire(x, y, effects);
        fixZone(x, y, effects);
        break;

    default:
        NOT_REACHED();
        break;

    }

    return result;
}


/**
 * Builldoze a tile (make it a #RIVER or #DIRT).
 * @param x      X map coordinate.
 * @param y      Y map coordinate.
 * @param effects Modification collecting object.
 * @return Tool result.
 */
ToolResult Micropolis::layDoze(int x, int y, ToolEffects *effects)
{
    MapValue tile = effects->getMapValue(x, y);

    if (!(tile & BULLBIT)) {
        return TOOLRESULT_FAILED;         /* Check dozeable bit. */
    }

    tile &= LOMASK;
    tile = neutralizeRoad(tile);

    switch (tile) {
    case HBRIDGE:
    case VBRIDGE:
    case BRWV:
    case BRWH:
    case HBRDG0:
    case HBRDG1:
    case HBRDG2:
    case HBRDG3:
    case VBRDG0:
    case VBRDG1:
    case VBRDG2:
    case VBRDG3:
    case HPOWER:
    case VPOWER:
    case HRAIL:
    case VRAIL:           /* Dozing over water, replace with water. */
        effects->setMapValue(x, y, RIVER);
        break;

    default:              /* Dozing on land, replace with land.  Simple, eh? */
        effects->setMapValue(x, y, DIRT);
        break;
    }

    effects->addCost(1);                     /* Costs $1.00.... */

    return TOOLRESULT_OK;
}


/**
 * Lay a road, and update road around it.
 * @param x X map coordinate.
 * @param y Y map coordinate.
 * @param effects Modification collecting object.
 * @return Tool result.
 */
ToolResult Micropolis::layRoad(int x, int y, ToolEffects *effects)
{
    int cost = 10;

    MapTile tile = effects->getMapTile(x, y);

    switch (tile) {

    case DIRT:
        effects->setMapValue(x, y, ROADS | BULLBIT | BURNBIT);
        break;

    case RIVER:                   /* Road on Water */
    case REDGE:
    case CHANNEL:                 /* Check how to build bridges, if possible. */
        cost = 50;

        if (x < WORLD_W - 1) {
            tile = effects->getMapTile(x + 1, y);
            tile = neutralizeRoad(tile);
            if (tile == VRAILROAD || tile == HBRIDGE
                                || (tile >= ROADS && tile <= HROADPOWER)) {
                effects->setMapValue(x, y, HBRIDGE | BULLBIT);
                break;
            }
        }

        if (x > 0) {
            tile = effects->getMapTile(x - 1, y);
            tile = neutralizeRoad(tile);
            if (tile == VRAILROAD || tile == HBRIDGE
                                || (tile >= ROADS && tile <= INTERSECTION)) {
                effects->setMapValue(x, y, HBRIDGE | BULLBIT);
                break;
            }
        }

        if (y < WORLD_H - 1) {
            tile = effects->getMapTile(x, y + 1);
            tile = neutralizeRoad(tile);
            if (tile == HRAILROAD || tile == VROADPOWER
                                || (tile >= VBRIDGE && tile <= INTERSECTION)) {
                effects->setMapValue(x, y, VBRIDGE | BULLBIT);
                break;
            }
        }

        if (y > 0) {
            tile = effects->getMapTile(x, y - 1);
            tile = neutralizeRoad(tile);
            if (tile == HRAILROAD || tile == VROADPOWER
                                || (tile >= VBRIDGE && tile <= INTERSECTION)) {
                effects->setMapValue(x, y, VBRIDGE | BULLBIT);
                break;
            }
        }

        /* Can't do road... */
        return TOOLRESULT_FAILED;

    case LHPOWER:         /* Road on power */
        effects->setMapValue(x, y, VROADPOWER | CONDBIT | BURNBIT | BULLBIT);
        break;

    case LVPOWER:         /* Road on power #2 */
        effects->setMapValue(x, y, HROADPOWER | CONDBIT | BURNBIT | BULLBIT);
        break;

    case LHRAIL:          /* Road on rail */
        effects->setMapValue(x, y, HRAILROAD | BURNBIT | BULLBIT);
        break;

    case LVRAIL:          /* Road on rail #2 */
        effects->setMapValue(x, y, VRAILROAD | BURNBIT | BULLBIT);
        break;

    default:              /* Can't do road */
        return TOOLRESULT_FAILED;

    }

    effects->addCost(cost);
    return TOOLRESULT_OK;
}


/**
 * Lay a rail, and update connections (rail, road, and wire) around it.
 * @param x      X map coordinate.
 * @param y      Y map coordinate.
 * @param effects Modification collecting object.
 * @return Tool result.
 */
ToolResult Micropolis::layRail(int x, int y, ToolEffects *effects)
{
    int cost = 20;

    MapTile tile = effects->getMapTile(x, y);

    tile = neutralizeRoad(tile);

    switch (tile) {
    case DIRT:                   /* Rail on Dirt */

        effects->setMapValue(x, y, LHRAIL | BULLBIT | BURNBIT);

        break;

    case RIVER:      /* Rail on Water */
    case REDGE:
    case CHANNEL:    /* Check how to build underwater tunnel, if possible. */

        cost = 100;

        if (x < WORLD_W - 1) {
            tile = effects->getMapTile(x + 1, y);
            tile = neutralizeRoad(tile);
            if (tile == RAILHPOWERV || tile == HRAIL
                                    || (tile >= LHRAIL && tile <= HRAILROAD)) {
                effects->setMapValue(x, y, HRAIL | BULLBIT);
                break;
            }
        }

        if (x > 0) {
            tile = effects->getMapTile(x - 1, y);
            tile = neutralizeRoad(tile);
            if (tile == RAILHPOWERV || tile == HRAIL
                                    || (tile > VRAIL && tile < VRAILROAD)) {
                effects->setMapValue(x, y, HRAIL | BULLBIT);
                break;
            }
        }

        if (y < WORLD_H - 1) {
            tile = effects->getMapTile(x, y + 1);
            tile = neutralizeRoad(tile);
            if (tile == RAILVPOWERH || tile == VRAILROAD
                                    || (tile > HRAIL && tile < HRAILROAD)) {
                effects->setMapValue(x, y, VRAIL | BULLBIT);
                break;
            }
        }

        if (y > 0) {
            tile = effects->getMapTile(x, y - 1);
            tile = neutralizeRoad(tile);
            if (tile == RAILVPOWERH || tile == VRAILROAD
                                    || (tile > HRAIL && tile < HRAILROAD)) {
                effects->setMapValue(x, y, VRAIL | BULLBIT);
                break;
            }
        }

        /* Can't do rail... */
        return TOOLRESULT_FAILED;

    case LHPOWER:             /* Rail on power */
        effects->setMapValue(x, y, RAILVPOWERH | CONDBIT | BURNBIT | BULLBIT);
        break;

    case LVPOWER:             /* Rail on power #2 */
        effects->setMapValue(x, y, RAILHPOWERV | CONDBIT | BURNBIT | BULLBIT);
        break;

    case ROADS:              /* Rail on road */
        effects->setMapValue(x, y, VRAILROAD | BURNBIT | BULLBIT);
        break;

    case ROADS2:              /* Rail on road #2 */
        effects->setMapValue(x, y, HRAILROAD | BURNBIT | BULLBIT);
        break;

    default:              /* Can't do rail */
        return TOOLRESULT_FAILED;
    }

    effects->addCost(cost);
    return TOOLRESULT_OK;
}


/**
 * Lay a wire, and update connections (rail, road, and wire) around it.
 * @param x X map coordinate.
 * @param y Y map coordinate.
 * @param effects Modification collecting object.
 * @return Tool result.
 */
ToolResult Micropolis::layWire(int x, int y, ToolEffects *effects)
{
    int cost = 5;

    MapTile tile = effects->getMapTile(x, y);

    tile = neutralizeRoad(tile);

    switch (tile) {

    case DIRT:            /* Wire on Dirt */

        effects->setMapValue(x, y, LHPOWER | CONDBIT | BURNBIT | BULLBIT);

        break;

    case RIVER:           /* Wire on Water */
    case REDGE:
    case CHANNEL:         /* Check how to lay underwater wire, if possible. */

        cost = 25;

        if (x < WORLD_W - 1) {
            tile = effects->getMapValue(x + 1, y);
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != HROADPOWER && tile != RAILHPOWERV && tile != HPOWER) {
                    effects->setMapValue(x, y, VPOWER | CONDBIT | BULLBIT);
                    break;
                }
            }
        }

        if (x > 0) {
            tile = effects->getMapValue(x - 1, y);
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != HROADPOWER && tile != RAILHPOWERV && tile != HPOWER) {
                    effects->setMapValue(x, y, VPOWER | CONDBIT | BULLBIT);
                    break;
                }
            }
        }

        if (y < WORLD_H - 1) {
            tile = effects->getMapValue(x, y + 1);
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != VROADPOWER && tile != RAILVPOWERH && tile != VPOWER) {
                    effects->setMapValue(x, y, HPOWER | CONDBIT | BULLBIT);
                    break;
                }
            }
        }

        if (y > 0) {
            tile = effects->getMapValue(x, y - 1);
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != VROADPOWER && tile != RAILVPOWERH && tile != VPOWER) {
                    effects->setMapValue(x, y, HPOWER | CONDBIT | BULLBIT);
                    break;
                }
            }
        }

        /* Can't do wire... */
        return TOOLRESULT_FAILED;

    case ROADS:              /* Wire on Road */
        effects->setMapValue(x, y, HROADPOWER | CONDBIT | BURNBIT | BULLBIT);
        break;

    case ROADS2:              /* Wire on Road #2 */
        effects->setMapValue(x, y, VROADPOWER | CONDBIT | BURNBIT | BULLBIT);
        break;

    case LHRAIL:             /* Wire on rail */
        effects->setMapValue(x, y, RAILHPOWERV | CONDBIT | BURNBIT | BULLBIT);
        break;

    case LVRAIL:             /* Wire on rail #2 */
        effects->setMapValue(x, y, RAILVPOWERH | CONDBIT | BURNBIT | BULLBIT);
        break;

    default:              /* Can't do wire */
        return TOOLRESULT_FAILED;

    }

    effects->addCost(cost);
    return TOOLRESULT_OK;
}


/**
 * Update connections (rails, and wire connections) to a zone.
 * @param x X map coordinate
 * @param y Y map coordinate.
 * @param effects Modification collecting object.
 */
void Micropolis::fixZone(int x, int y, ToolEffects *effects)
{
    fixSingle(x, y, effects);

    if (y > 0) {
        fixSingle(x, y - 1, effects);
    }

    if (x < WORLD_W - 1) {
        fixSingle(x + 1, y, effects);
    }

    if (y < WORLD_H - 1) {
        fixSingle(x, y + 1, effects);
    }

    if (x > 0) {
        fixSingle(x - 1, y, effects);
    }
}


/**
 * Modify road, rails, and wire connections at a given tile.
 * @param x X map coordinate.
 * @param y Y map coordinate.
 * @param effects Modification collecting object.
 */
void Micropolis::fixSingle(int x, int y, ToolEffects *effects)
{
    unsigned short adjTile = 0;

    MapTile tile = effects->getMapTile(x, y);

    tile = neutralizeRoad(tile);

    if (tile >= ROADS && tile <= INTERSECTION) {           /* Cleanup Road */

        if (y > 0) {
            tile = effects->getMapTile(x, y - 1);
            tile = neutralizeRoad(tile);
            if ((tile == HRAILROAD || (tile >= ROADBASE && tile <= VROADPOWER))
                                && tile != HROADPOWER && tile != VRAILROAD
                                && tile != ROADBASE) {
                adjTile |= 0x0001;
            }
        }

        if (x < WORLD_W - 1) {
            tile = effects->getMapTile(x + 1, y);
            tile = neutralizeRoad(tile);
            if ((tile == VRAILROAD || (tile >= ROADBASE && tile <= VROADPOWER))
                                && tile != VROADPOWER && tile != HRAILROAD
                                && tile != VBRIDGE) {
                adjTile |= 0x0002;
            }
        }

        if (y < WORLD_H - 1) {
            tile = effects->getMapTile(x, y + 1);
            tile = neutralizeRoad(tile);
            if ((tile == HRAILROAD || (tile >= ROADBASE && tile <= VROADPOWER))
                                && tile != HROADPOWER && tile != VRAILROAD
                                && tile != ROADBASE) {
                adjTile |= 0x0004;
            }
        }

        if (x > 0) {
            tile = effects->getMapTile(x - 1, y);
            tile = neutralizeRoad(tile);
            if ((tile == VRAILROAD || (tile >= ROADBASE && tile <= VROADPOWER))
                                && tile != VROADPOWER && tile != HRAILROAD
                                && tile != VBRIDGE) {
                adjTile |= 0x0008;
            }
        }

        effects->setMapValue(x, y, RoadTable[adjTile] | BULLBIT | BURNBIT);
        return;
    }

    if (tile >= LHRAIL && tile <= LVRAIL10) {         /* Cleanup Rail */

        if (y > 0) {
            tile = effects->getMapTile(x, y - 1);
            tile = neutralizeRoad(tile);
            if (tile >= RAILHPOWERV && tile <= VRAILROAD
                                && tile != RAILHPOWERV && tile != HRAILROAD
                                && tile != HRAIL) {
                adjTile |= 0x0001;
            }
        }

        if (x < WORLD_W - 1) {
            tile = effects->getMapTile(x + 1, y);
            tile = neutralizeRoad(tile);
            if (tile >= RAILHPOWERV && tile <= VRAILROAD
                                && tile != RAILVPOWERH && tile != VRAILROAD
                                && tile != VRAIL) {
                adjTile |= 0x0002;
            }
        }

        if (y < WORLD_H - 1) {
            tile = effects->getMapTile(x, y + 1);
            tile = neutralizeRoad(tile);
            if (tile >= RAILHPOWERV && tile <= VRAILROAD
                                && tile != RAILHPOWERV && tile != HRAILROAD
                                && tile != HRAIL) {
                adjTile |= 0x0004;
            }
        }

        if (x > 0) {
            tile = effects->getMapTile(x - 1, y);
            tile = neutralizeRoad(tile);
            if (tile >= RAILHPOWERV && tile <= VRAILROAD
                                && tile != RAILVPOWERH && tile != VRAILROAD
                                && tile != VRAIL) {
                adjTile |= 0x0008;
            }
        }

        effects->setMapValue(x, y, RailTable[adjTile] | BULLBIT | BURNBIT);
        return;
    }

    if (tile >= LHPOWER && tile <= LVPOWER10) {         /* Cleanup Wire */

        if (y > 0) {
            tile = effects->getMapValue(x, y - 1);
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != VPOWER && tile != VROADPOWER && tile != RAILVPOWERH) {
                    adjTile |= 0x0001;
                }
            }
        }

        if (x < WORLD_W - 1) {
            tile = effects->getMapValue(x + 1, y);
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != HPOWER && tile != HROADPOWER && tile != RAILHPOWERV) {
                    adjTile |= 0x0002;
                }
            }
        }

        if (y < WORLD_H - 1) {
            tile = effects->getMapValue(x, y + 1);
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != VPOWER && tile != VROADPOWER && tile != RAILVPOWERH) {
                    adjTile |= 0x0004;
                }
            }
        }

        if (x > 0) {
            tile = effects->getMapValue(x - 1, y);
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != HPOWER && tile != HROADPOWER && tile != RAILHPOWERV) {
                    adjTile |= 0x0008;
                }
            }
        }

        effects->setMapValue(x, y, WireTable[adjTile] | BLBNCNBIT);
        return;
    }
}


////////////////////////////////////////////////////////////////////////
