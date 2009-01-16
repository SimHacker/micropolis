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


////////////////////////////////////////////////////////////////////////


static const short RoadTable[16] = {
    ROADS, ROADS2, ROADS, ROADS3,
    ROADS2, ROADS2, ROADS4, ROADS8,
    ROADS, ROADS6, ROADS, ROADS7,
    ROADS5, ROADS10, ROADS9, INTERSECTION
};

static const short RailTable[16] = {
    LHRAIL, LVRAIL, LHRAIL, LVRAIL2,
    LVRAIL, LVRAIL, LVRAIL3, LVRAIL7,
    LHRAIL, LVRAIL5, LHRAIL, LVRAIL6,
    LVRAIL4, LVRAIL9, LVRAIL8, LVRAIL10
};

static const short WireTable[16] = {
    LHPOWER, LVPOWER, LHPOWER, LVPOWER2,
    LVPOWER, LVPOWER, LVPOWER3, LVPOWER7,
    LHPOWER, LVPOWER5, LHPOWER, LVPOWER6,
    LVPOWER4, LVPOWER9, LVPOWER8, LVPOWER10
};


////////////////////////////////////////////////////////////////////////


/* comefrom: check3Border check4Border check5Border processWand */
int Micropolis::connectTile(short x, short y, short *tileAdrPtr, short Command)
{
    short tile;
    int result = 1;

    /* make sure the array subscripts are in bounds */
    if (!testBounds(x, y)) {
        return (0);
    }

    /* AutoDoze */
    if (Command >= 2 && Command <= 4) {

        if (autoBulldoze && totalFunds > 0) {

            tile = *tileAdrPtr;

            if (tile & BULLBIT) {
	        tile &= LOMASK;
                tile = neutralizeRoad(tile);

                /* Maybe this should check BULLBIT instead of checking tile values? */
                if ((tile >= TINYEXP && tile <= LASTTINYEXP) ||
                        (tile < HBRIDGE && tile != DIRT)) {

                    spend(1);

                    *tileAdrPtr = DIRT;

                }
            }
        }
    }

    switch (Command) {

    case 0:       /* Fix zone */
        fixZone(x, y, tileAdrPtr);
        break;

    case 1:       /* Doze zone */
        result = layDoze(x, y, tileAdrPtr);
        fixZone(x, y, tileAdrPtr);
        break;

    case 2:       /* Lay Road */
        result = layRoad(x, y, tileAdrPtr);
        fixZone(x, y, tileAdrPtr);
        break;

    case 3:       /* Lay Rail */
        result = layRail(x, y, tileAdrPtr);
        fixZone(x, y, tileAdrPtr);
        break;

    case 4:       /* Lay Wire */
        result = layWire(x, y, tileAdrPtr);
        fixZone(x, y, tileAdrPtr);
        break;

    }

    return result;
}


/**
 * Builldoze, and update connections (rail, road, and wire) around it.
 * @param x          X map coordinate.
 * @param y          Y map coordinate.
 * @param tileAdrPtr Pointer to tile (\a x, \a y).
 */
int Micropolis::layDoze(int x, int y, short *tileAdrPtr)
{
    short tile;

    if (totalFunds == 0) {
        return -2;                  /* no mas dinero. */
    }

    tile = *tileAdrPtr;

    if (!(tile & BULLBIT)) {
        return 0;                   /* Check dozeable bit. */
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
        *tileAdrPtr = RIVER;
        break;

    default:              /* Dozing on land, replace with land.  Simple, eh? */
        *tileAdrPtr = DIRT;
        break;
    }

    spend(1);                     /* Costs $1.00.... */

    return 1;
}


/**
 * Lay a road, and update connections (rail, road, and wire) around it.
 * @param x          X map coordinate.
 * @param y          Y map coordinate.
 * @param tileAdrPtr Pointer to tile (\a x, \a y).
 */
int Micropolis::layRoad(int x, int y, short *tileAdrPtr)
{
    int cost = 10;

    if (totalFunds < cost) {
        return -2;
    }

    short tile = *tileAdrPtr & LOMASK;

    switch (tile) {

    case DIRT:
        *tileAdrPtr = ROADS | BULLBIT | BURNBIT;
        break;

    case RIVER:                   /* Road on Water */
    case REDGE:
    case CHANNEL:                 /* Check how to build bridges, if possible. */
        cost = 50;

        if (totalFunds < cost) {
            return -2;
        }

        if (x < WORLD_W - 1) {
            tile = tileAdrPtr[WORLD_H] & LOMASK;
            tile = neutralizeRoad(tile);
            if (tile == VRAILROAD || tile == HBRIDGE
                                || (tile >= ROADS && tile <= HROADPOWER)) {
                *tileAdrPtr = HBRIDGE | BULLBIT;
                break;
            }
        }

        if (x > 0) {
            tile = tileAdrPtr[-WORLD_H] & LOMASK;
            tile = neutralizeRoad(tile);
            if (tile == VRAILROAD || tile == HBRIDGE
                                || (tile >= ROADS && tile <= INTERSECTION)) {
                *tileAdrPtr = HBRIDGE | BULLBIT;
                break;
            }
        }

        if (y < WORLD_H - 1) {
            tile = tileAdrPtr[1] & LOMASK;
            tile = neutralizeRoad(tile);
            if (tile == HRAILROAD || tile == VROADPOWER
                                || (tile >= VBRIDGE && tile <= INTERSECTION)) {
                *tileAdrPtr = VBRIDGE | BULLBIT;
                break;
            }
        }

        if (y > 0) {
            tile = tileAdrPtr[-1] & LOMASK;
            tile = neutralizeRoad(tile);
            if (tile == HRAILROAD || tile == VROADPOWER
                                || (tile >= VBRIDGE && tile <= INTERSECTION)) {
                *tileAdrPtr = VBRIDGE | BULLBIT;
                break;
            }
        }

        /* Can't do road... */
        return 0;

    case LHPOWER:         /* Road on power */
        *tileAdrPtr = VROADPOWER | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LVPOWER:         /* Road on power #2 */
        *tileAdrPtr = HROADPOWER | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LHRAIL:          /* Road on rail */
        *tileAdrPtr = HRAILROAD | BURNBIT | BULLBIT;
        break;

    case LVRAIL:          /* Road on rail #2 */
        *tileAdrPtr = VRAILROAD | BURNBIT | BULLBIT;
        break;

    default:              /* Can't do road */
        return 0;

    }

    spend(cost);
    return 1;
}


/**
 * Lay a rail, and update connections (rail, road, and wire) around it.
 * @param x          X map coordinate.
 * @param y          Y map coordinate.
 * @param tileAdrPtr Pointer to tile (\a x, \a y).
 */
int Micropolis::layRail(int x, int y, short *tileAdrPtr)
{
    int cost = 20;

    if (totalFunds < cost) {
        return -2;
    }

    short tile = *tileAdrPtr & LOMASK;

    tile = neutralizeRoad(tile);

    switch (tile) {
    case 0:                       /* Rail on Dirt */

        *tileAdrPtr = LHRAIL | BULLBIT | BURNBIT;

        break;

    case 2:          /* Rail on Water */
    case 3:
    case 4:          /* Check how to build underwater tunnel, if possible. */

        cost = 100;

        if (totalFunds < cost) {
            return -2;
        }

        if (x < WORLD_W - 1) {
            tile = tileAdrPtr[WORLD_H] & LOMASK;
            tile = neutralizeRoad(tile);
            if (tile == RAILHPOWERV || tile == HRAIL
                                    || (tile >= LHRAIL && tile <= HRAILROAD)) {
                *tileAdrPtr = HRAIL | BULLBIT;
                break;
            }
        }

        if (x > 0) {
            tile = tileAdrPtr[-WORLD_H] & LOMASK;
            tile = neutralizeRoad(tile);
            if (tile == RAILHPOWERV || tile == HRAIL
                                    || (tile > VRAIL && tile < VRAILROAD)) {
                *tileAdrPtr = HRAIL | BULLBIT;
                break;
            }
        }

        if (y < WORLD_H - 1) {
            tile = tileAdrPtr[1] & LOMASK;
            tile = neutralizeRoad(tile);
            if (tile == RAILVPOWERH || tile == VRAILROAD
                                    || (tile > HRAIL && tile < HRAILROAD)) {
                *tileAdrPtr = VRAIL | BULLBIT;
                break;
            }
        }

        if (y > 0) {
            tile = tileAdrPtr[-1] & LOMASK;
            tile = neutralizeRoad(tile);
            if (tile == RAILVPOWERH || tile == VRAILROAD
                                    || (tile > HRAIL && tile < HRAILROAD)) {
                *tileAdrPtr = VRAIL | BULLBIT;
                break;
            }
        }

        /* Can't do rail... */
        return 0;

    case LHPOWER:             /* Rail on power */
        *tileAdrPtr = RAILVPOWERH | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LVPOWER:             /* Rail on power #2 */
        *tileAdrPtr = RAILHPOWERV | CONDBIT | BURNBIT | BULLBIT;
        break;

    case ROADS:              /* Rail on road */
        *tileAdrPtr = VRAILROAD | BURNBIT | BULLBIT;
        break;

    case ROADS2:              /* Rail on road #2 */
        *tileAdrPtr = HRAILROAD | BURNBIT | BULLBIT;
        break;

    default:              /* Can't do rail */
        return 0;
    }

    spend(cost);
    return 1;
}


/**
 * Lay a wire, and update connections (rail, road, and wire) around it.
 * @param x          X map coordinate.
 * @param y          Y map coordinate.
 * @param tileAdrPtr Pointer to tile (\a x, \a y).
 */
int Micropolis::layWire(int x, int y, short *tileAdrPtr)
{
    int cost = 5;

    if (totalFunds < cost) {
        return -2;
    }

    short tile = *tileAdrPtr & LOMASK;

    tile = neutralizeRoad(tile);

    switch (tile) {

    case 0:                       /* Wire on Dirt */

        *tileAdrPtr = LHPOWER | CONDBIT | BURNBIT | BULLBIT;

        break;

    case 2:               /* Wire on Water */
    case 3:
    case 4:               /* Check how to lay underwater wire, if possible. */

        cost = 25;

        if (totalFunds < cost) {
            return -2;
        }

        if (x < WORLD_W - 1) {
            tile = tileAdrPtr[WORLD_H];
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != HROADPOWER && tile != RAILHPOWERV && tile != HPOWER) {
                    *tileAdrPtr = VPOWER | CONDBIT | BULLBIT;
                    break;
                }
            }
        }

        if (x > 0) {
            tile = tileAdrPtr[-WORLD_H];
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != HROADPOWER && tile != RAILHPOWERV && tile != HPOWER) {
                    *tileAdrPtr = VPOWER | CONDBIT | BULLBIT;
                    break;
                }
            }
        }

        if (y < WORLD_H - 1) {
            tile = tileAdrPtr[1];
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != VROADPOWER && tile != RAILVPOWERH && tile != VPOWER) {
                    *tileAdrPtr = HPOWER | CONDBIT | BULLBIT;
                    break;
                }
            }
        }

        if (y > 0) {
            tile = tileAdrPtr[-1];
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != VROADPOWER && tile != RAILVPOWERH && tile != VPOWER) {
                    *tileAdrPtr = HPOWER | CONDBIT | BULLBIT;
                    break;
                }
            }
        }

        /* Can't do wire... */
        return 0;

    case ROADS:              /* Wire on Road */
        *tileAdrPtr = HROADPOWER | CONDBIT | BURNBIT | BULLBIT;
        break;

    case ROADS2:              /* Wire on Road #2 */
        *tileAdrPtr = VROADPOWER | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LHRAIL:             /* Wire on rail */
        *tileAdrPtr = RAILHPOWERV | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LVRAIL:             /* Wire on rail #2 */
        *tileAdrPtr = RAILVPOWERH | CONDBIT | BURNBIT | BULLBIT;
        break;

    default:              /* Can't do wire */
        return 0;

    }

    spend(cost);
    return 1;
}


/**
 * Fix a zone, and update connections (rails, and wire connections) around it.
 * @param x          X map coordinate
 * @param y          Y map coordinate.
 * @param tileAdrPtr Pointer to tile (\a x, \a y).
 */
void Micropolis::fixZone(int x, int y, short *tileAdrPtr)
{
    fixSingle(x, y, &tileAdrPtr[0]);

    if (y > 0) {
        fixSingle(x, y - 1, &tileAdrPtr[-1]);
    }

    if (x < WORLD_W - 1) {
        fixSingle(x + 1, y, &tileAdrPtr[WORLD_H]);
    }

    if (y < WORLD_H - 1) {
        fixSingle(x, y + 1, &tileAdrPtr[1]);
    }

    if (x > 0) {
        fixSingle(x - 1, y, &tileAdrPtr[-WORLD_H]);
    }
}


/**
 * Modify road, rails, and wire connections at a given tile.
 * @param x          X map coordinate
 * @param y          Y map coordinate.
 * @param tileAdrPtr Pointer to tile (\a x, \a y).
 */
void Micropolis::fixSingle(int x, int y, short *tileAdrPtr)
{
    short adjTile = 0;

    short tile = *tileAdrPtr & LOMASK;

    tile = neutralizeRoad(tile);

    if (tile >= ROADS && tile <= INTERSECTION) {           /* Cleanup Road */

        if (y > 0) {
            tile = tileAdrPtr[-1] & LOMASK;
            tile = neutralizeRoad(tile);
            if ((tile == HRAILROAD || (tile >= ROADBASE && tile <= VROADPOWER))
                                && tile != HROADPOWER && tile != VRAILROAD
                                && tile != ROADBASE) {
                adjTile |= 0x0001;
            }
        }

        if (x < WORLD_W - 1) {
            tile = tileAdrPtr[WORLD_H] & LOMASK;
            tile = neutralizeRoad(tile);
            if ((tile == VRAILROAD || (tile >= ROADBASE && tile <= VROADPOWER))
                                && tile != VROADPOWER && tile != HRAILROAD
                                && tile != VBRIDGE) {
                adjTile |= 0x0002;
            }
        }

        if (y < WORLD_H - 1) {
            tile = tileAdrPtr[1] & LOMASK;
            tile = neutralizeRoad(tile);
            if ((tile == HRAILROAD || (tile >= ROADBASE && tile <= VROADPOWER))
                                && tile != HROADPOWER && tile != VRAILROAD
                                && tile != ROADBASE) {
                adjTile |= 0x0004;
            }
        }

        if (x > 0) {
            tile = tileAdrPtr[-WORLD_H] & LOMASK;
            tile = neutralizeRoad(tile);
            if ((tile == VRAILROAD || (tile >= ROADBASE && tile <= VROADPOWER))
                                && tile != VROADPOWER && tile != HRAILROAD
                                && tile != VBRIDGE) {
                adjTile |= 0x0008;
            }
        }

        *tileAdrPtr = RoadTable[adjTile] | BULLBIT | BURNBIT;
        return;
    }

    if (tile >= LHRAIL && tile <= LVRAIL10) {         /* Cleanup Rail */

        if (y > 0) {
            tile = tileAdrPtr[-1] & LOMASK;
            tile = neutralizeRoad(tile);
            if (tile >= RAILHPOWERV && tile <= VRAILROAD
                                && tile != RAILHPOWERV && tile != HRAILROAD
                                && tile != HRAIL) {
                adjTile |= 0x0001;
            }
        }

        if (x < WORLD_W - 1) {
            tile = tileAdrPtr[WORLD_H] & LOMASK;
            tile = neutralizeRoad(tile);
            if (tile >= RAILHPOWERV && tile <= VRAILROAD
                                && tile != RAILVPOWERH && tile != VRAILROAD
                                && tile != VRAIL) {
                adjTile |= 0x0002;
            }
        }

        if (y < WORLD_H - 1) {
            tile = tileAdrPtr[1] & LOMASK;
            tile = neutralizeRoad(tile);
            if (tile >= RAILHPOWERV && tile <= VRAILROAD
                                && tile != RAILHPOWERV && tile != HRAILROAD
                                && tile != HRAIL) {
                adjTile |= 0x0004;
            }
        }

        if (x > 0) {
            tile = tileAdrPtr[-WORLD_H] & LOMASK;
            tile = neutralizeRoad(tile);
            if (tile >= RAILHPOWERV && tile <= VRAILROAD
                                && tile != RAILVPOWERH && tile != VRAILROAD
                                && tile != VRAIL) {
                adjTile |= 0x0008;
            }
        }

        *tileAdrPtr = RailTable[adjTile] | BULLBIT | BURNBIT;
        return;
    }

    if (tile >= LHPOWER && tile <= LVPOWER10) {         /* Cleanup Wire */

        if (y > 0) {
            tile = tileAdrPtr[-1];
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != VPOWER && tile != VROADPOWER && tile != RAILVPOWERH) {
                    adjTile |= 0x0001;
                }
            }
        }

        if (x < WORLD_W - 1) {
            tile = tileAdrPtr[WORLD_H];
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != HPOWER && tile != HROADPOWER && tile != RAILHPOWERV) {
                    adjTile |= 0x0002;
                }
            }
        }

        if (y < WORLD_H - 1) {
            tile = tileAdrPtr[1];
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != VPOWER && tile != VROADPOWER && tile != RAILVPOWERH) {
                    adjTile |= 0x0004;
                }
            }
        }

        if (x > 0) {
            tile = tileAdrPtr[-WORLD_H];
            if (tile & CONDBIT) {
                tile &= LOMASK;
                tile = neutralizeRoad(tile);
                if (tile != HPOWER && tile != HROADPOWER && tile != RAILHPOWERV) {
                    adjTile |= 0x0008;
                }
            }
        }

        *tileAdrPtr = WireTable[adjTile] | BULLBIT | BURNBIT | CONDBIT;
        return;
    }
}


////////////////////////////////////////////////////////////////////////
