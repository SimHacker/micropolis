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
int Micropolis::ConnecTile(short x, short y, short *TileAdrPtr, short Command)
{
    short Tile;
    int result = 1;

    /* make sure the array subscripts are in bounds */
    if (!TestBounds(x, y)) {
        return (0);
    }

    /* AutoDoze */
    if (Command >= 2 && Command <= 4) {

        if (autoBulldoze && TotalFunds > 0) {

            Tile = *TileAdrPtr;

            if (Tile & BULLBIT) {
                NeutralizeRoad(Tile);

                /* Maybe this should check BULLBIT instead of checking tile values? */
                if ((Tile >= TINYEXP && Tile <= LASTTINYEXP) ||
                        (Tile < HBRIDGE && Tile != DIRT)) {

                    Spend(1);

                    *TileAdrPtr = DIRT;

                }
            }
        }
    }

    switch (Command) {

    case 0:       /* Fix zone */
        FixZone(x, y, TileAdrPtr);
        break;

    case 1:       /* Doze zone */
        result = LayDoze(x, y, TileAdrPtr);
        FixZone(x, y, TileAdrPtr);
        break;

    case 2:       /* Lay Road */
        result = LayRoad(x, y, TileAdrPtr);
        FixZone(x, y, TileAdrPtr);
        break;

    case 3:       /* Lay Rail */
        result = LayRail(x, y, TileAdrPtr);
        FixZone(x, y, TileAdrPtr);
        break;

    case 4:       /* Lay Wire */
        result = LayWire(x, y, TileAdrPtr);
        FixZone(x, y, TileAdrPtr);
        break;

    }

    return result;
}


/**
 * Builldoze, and update connections (rail, road, and wire) around it.
 * @param x          X map coordinate.
 * @param y          Y map coordinate.
 * @param TileAdrPtr Pointer to tile (\a x, \a y).
 */
int Micropolis::LayDoze(int x, int y, short *TileAdrPtr)
{
    short Tile;

    if (TotalFunds == 0) {
        return -2;                  /* no mas dinero. */
    }

    Tile = *TileAdrPtr;

    if (!(Tile & BULLBIT)) {
        return 0;                   /* Check dozeable bit. */
    }

    NeutralizeRoad(Tile);

    switch (Tile) {
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
        *TileAdrPtr = RIVER;
        break;

    default:              /* Dozing on land, replace with land.  Simple, eh? */
        *TileAdrPtr = DIRT;
        break;
    }

    Spend(1);                     /* Costs $1.00.... */

    return 1;
}


/**
 * Lay a road, and update connections (rail, road, and wire) around it.
 * @param x          X map coordinate.
 * @param y          Y map coordinate.
 * @param TileAdrPtr Pointer to tile (\a x, \a y).
 */
int Micropolis::LayRoad(int x, int y, short *TileAdrPtr)
{
    int cost = 10;

    if (TotalFunds < cost) {
        return -2;
    }

    short Tile = *TileAdrPtr & LOMASK;

    switch (Tile) {

    case DIRT:
        *TileAdrPtr = ROADS | BULLBIT | BURNBIT;
        break;

    case RIVER:                   /* Road on Water */
    case REDGE:
    case CHANNEL:                 /* Check how to build bridges, if possible. */
        cost = 50;

        if (TotalFunds < cost) {
            return -2;
        }

        if (x < WORLD_X - 1) {
            Tile = TileAdrPtr[WORLD_Y];
            NeutralizeRoad(Tile);
            if (Tile == VRAILROAD || Tile == HBRIDGE
                                || (Tile >= ROADS && Tile <= HROADPOWER)) {
                *TileAdrPtr = HBRIDGE | BULLBIT;
                break;
            }
        }

        if (x > 0) {
            Tile = TileAdrPtr[-WORLD_Y];
            NeutralizeRoad(Tile);
            if (Tile == VRAILROAD || Tile == HBRIDGE
                                || (Tile >= ROADS && Tile <= INTERSECTION)) {
                *TileAdrPtr = HBRIDGE | BULLBIT;
                break;
            }
        }

        if (y < WORLD_Y - 1) {
            Tile = TileAdrPtr[1];
            NeutralizeRoad(Tile);
            if (Tile == HRAILROAD || Tile == VROADPOWER
                                || (Tile >= VBRIDGE && Tile <= INTERSECTION)) {
                *TileAdrPtr = VBRIDGE | BULLBIT;
                break;
            }
        }

        if (y > 0) {
            Tile = TileAdrPtr[-1];
            NeutralizeRoad(Tile);
            if (Tile == HRAILROAD || Tile == VROADPOWER
                                || (Tile >= VBRIDGE && Tile <= INTERSECTION)) {
                *TileAdrPtr = VBRIDGE | BULLBIT;
                break;
            }
        }

        /* Can't do road... */
        return 0;

    case LHPOWER:         /* Road on power */
        *TileAdrPtr = VROADPOWER | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LVPOWER:         /* Road on power #2 */
        *TileAdrPtr = HROADPOWER | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LHRAIL:          /* Road on rail */
        *TileAdrPtr = HRAILROAD | BURNBIT | BULLBIT;
        break;

    case LVRAIL:          /* Road on rail #2 */
        *TileAdrPtr = VRAILROAD | BURNBIT | BULLBIT;
        break;

    default:              /* Can't do road */
        return 0;

    }

    Spend(cost);
    return 1;
}


/**
 * Lay a rail, and update connections (rail, road, and wire) around it.
 * @param x          X map coordinate.
 * @param y          Y map coordinate.
 * @param TileAdrPtr Pointer to tile (\a x, \a y).
 */
int Micropolis::LayRail(int x, int y, short *TileAdrPtr)
{
    int cost = 20;

    if (TotalFunds < cost) {
        return -2;
    }

    short Tile = *TileAdrPtr & LOMASK;

    NeutralizeRoad(Tile);

    switch (Tile) {
    case 0:                       /* Rail on Dirt */

        *TileAdrPtr = LHRAIL | BULLBIT | BURNBIT;

        break;

    case 2:          /* Rail on Water */
    case 3:
    case 4:          /* Check how to build underwater tunnel, if possible. */

        cost = 100;

        if (TotalFunds < cost) {
            return -2;
        }

        if (x < WORLD_X - 1) {
            Tile = TileAdrPtr[WORLD_Y];
            NeutralizeRoad(Tile);
            if (Tile == RAILHPOWERV || Tile == HRAIL
                                    || (Tile >= LHRAIL && Tile <= HRAILROAD)) {
                *TileAdrPtr = HRAIL | BULLBIT;
                break;
            }
        }

        if (x > 0) {
            Tile = TileAdrPtr[-WORLD_Y];
            NeutralizeRoad(Tile);
            if (Tile == RAILHPOWERV || Tile == HRAIL
                                    || (Tile > VRAIL && Tile < VRAILROAD)) {
                *TileAdrPtr = HRAIL | BULLBIT;
                break;
            }
        }

        if (y < WORLD_Y - 1) {
            Tile = TileAdrPtr[1];
            NeutralizeRoad(Tile);
            if (Tile == RAILVPOWERH || Tile == VRAILROAD
                                    || (Tile > HRAIL && Tile < HRAILROAD)) {
                *TileAdrPtr = VRAIL | BULLBIT;
                break;
            }
        }

        if (y > 0) {
            Tile = TileAdrPtr[-1];
            NeutralizeRoad(Tile);
            if (Tile == RAILVPOWERH || Tile == VRAILROAD
                                    || (Tile > HRAIL && Tile < HRAILROAD)) {
                *TileAdrPtr = VRAIL | BULLBIT;
                break;
            }
        }

        /* Can't do rail... */
        return 0;

    case LHPOWER:             /* Rail on power */
        *TileAdrPtr = RAILVPOWERH | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LVPOWER:             /* Rail on power #2 */
        *TileAdrPtr = RAILHPOWERV | CONDBIT | BURNBIT | BULLBIT;
        break;

    case ROADS:              /* Rail on road */
        *TileAdrPtr = VRAILROAD | BURNBIT | BULLBIT;
        break;

    case ROADS2:              /* Rail on road #2 */
        *TileAdrPtr = HRAILROAD | BURNBIT | BULLBIT;
        break;

    default:              /* Can't do rail */
        return 0;
    }

    Spend(cost);
    return 1;
}


/**
 * Lay a wire, and update connections (rail, road, and wire) around it.
 * @param x          X map coordinate.
 * @param y          Y map coordinate.
 * @param TileAdrPtr Pointer to tile (\a x, \a y).
 */
int Micropolis::LayWire(int x, int y, short *TileAdrPtr)
{
    int cost = 5;

    if (TotalFunds < cost) {
        return -2;
    }

    short Tile = *TileAdrPtr & LOMASK;

    NeutralizeRoad(Tile);

    switch (Tile) {

    case 0:                       /* Wire on Dirt */

        *TileAdrPtr = LHPOWER | CONDBIT | BURNBIT | BULLBIT;

        break;

    case 2:               /* Wire on Water */
    case 3:
    case 4:               /* Check how to lay underwater wire, if possible. */

        cost = 25;

        if (TotalFunds < cost) {
            return -2;
        }

        if (x < WORLD_X - 1) {
            Tile = TileAdrPtr[WORLD_Y];
            if (Tile & CONDBIT) {
                NeutralizeRoad(Tile);
                if (Tile != HROADPOWER && Tile != RAILHPOWERV && Tile != HPOWER) {
                    *TileAdrPtr = VPOWER | CONDBIT | BULLBIT;
                    break;
                }
            }
        }

        if (x > 0) {
            Tile = TileAdrPtr[-WORLD_Y];
            if (Tile & CONDBIT) {
                NeutralizeRoad(Tile);
                if (Tile != HROADPOWER && Tile != RAILHPOWERV && Tile != HPOWER) {
                    *TileAdrPtr = VPOWER | CONDBIT | BULLBIT;
                    break;
                }
            }
        }

        if (y < WORLD_Y - 1) {
            Tile = TileAdrPtr[1];
            if (Tile & CONDBIT) {
                NeutralizeRoad(Tile);
                if (Tile != VROADPOWER && Tile != RAILVPOWERH && Tile != VPOWER) {
                    *TileAdrPtr = HPOWER | CONDBIT | BULLBIT;
                    break;
                }
            }
        }

        if (y > 0) {
            Tile = TileAdrPtr[-1];
            if (Tile & CONDBIT) {
                NeutralizeRoad(Tile);
                if (Tile != VROADPOWER && Tile != RAILVPOWERH && Tile != VPOWER) {
                    *TileAdrPtr = HPOWER | CONDBIT | BULLBIT;
                    break;
                }
            }
        }

        /* Can't do wire... */
        return 0;

    case ROADS:              /* Wire on Road */
        *TileAdrPtr = HROADPOWER | CONDBIT | BURNBIT | BULLBIT;
        break;

    case ROADS2:              /* Wire on Road #2 */
        *TileAdrPtr = VROADPOWER | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LHRAIL:             /* Wire on rail */
        *TileAdrPtr = RAILHPOWERV | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LVRAIL:             /* Wire on rail #2 */
        *TileAdrPtr = RAILVPOWERH | CONDBIT | BURNBIT | BULLBIT;
        break;

    default:              /* Can't do wire */
        return 0;

    }

    Spend(cost);
    return 1;
}


/**
 * Fix a zone, and update connections (rails, and wire connections) around it.
 * @param x          X map coordinate
 * @param y          Y map coordinate.
 * @param TileAdrPtr Pointer to tile (\a x, \a y).
 */
void Micropolis::FixZone(int x, int y, short *TileAdrPtr)
{
    FixSingle(x, y, &TileAdrPtr[0]);

    if (y > 0) {
        FixSingle(x, y - 1, &TileAdrPtr[-1]);
    }

    if (x < WORLD_X - 1) {
        FixSingle(x + 1, y, &TileAdrPtr[WORLD_Y]);
    }

    if (y < WORLD_Y - 1) {
        FixSingle(x, y + 1, &TileAdrPtr[1]);
    }

    if (x > 0) {
        FixSingle(x - 1, y, &TileAdrPtr[-WORLD_Y]);
    }
}


/**
 * Modify road, rails, and wire connections at a given tile.
 * @param x          X map coordinate
 * @param y          Y map coordinate.
 * @param TileAdrPtr Pointer to tile (\a x, \a y).
 */
void Micropolis::FixSingle(int x, int y, short *TileAdrPtr)
{
    short adjTile = 0;

    short Tile = *TileAdrPtr & LOMASK;

    NeutralizeRoad(Tile);

    if (Tile >= ROADS && Tile <= INTERSECTION) {           /* Cleanup Road */

        if (y > 0) {
            Tile = TileAdrPtr[-1];
            NeutralizeRoad(Tile);
            if ((Tile == HRAILROAD || (Tile >= ROADBASE && Tile <= VROADPOWER))
                                && Tile != HROADPOWER && Tile != VRAILROAD
                                && Tile != ROADBASE) {
                adjTile |= 0x0001;
            }
        }

        if (x < WORLD_X - 1) {
            Tile = TileAdrPtr[WORLD_Y];
            NeutralizeRoad(Tile);
            if ((Tile == VRAILROAD || (Tile >= ROADBASE && Tile <= VROADPOWER))
                                && Tile != VROADPOWER && Tile != HRAILROAD
                                && Tile != VBRIDGE) {
                adjTile |= 0x0002;
            }
        }

        if (y < WORLD_Y - 1) {
            Tile = TileAdrPtr[1];
            NeutralizeRoad(Tile);
            if ((Tile == HRAILROAD || (Tile >= ROADBASE && Tile <= VROADPOWER))
                                && Tile != HROADPOWER && Tile != VRAILROAD
                                && Tile != ROADBASE) {
                adjTile |= 0x0004;
            }
        }

        if (x > 0) {
            Tile = TileAdrPtr[-WORLD_Y];
            NeutralizeRoad(Tile);
            if ((Tile == VRAILROAD || (Tile >= ROADBASE && Tile <= VROADPOWER))
                                && Tile != VROADPOWER && Tile != HRAILROAD
                                && Tile != VBRIDGE) {
                adjTile |= 0x0008;
            }
        }

        *TileAdrPtr = RoadTable[adjTile] | BULLBIT | BURNBIT;
        return;
    }

    if (Tile >= LHRAIL && Tile <= LVRAIL10) {         /* Cleanup Rail */

        if (y > 0) {
            Tile = TileAdrPtr[-1];
            NeutralizeRoad(Tile);
            if (Tile >= RAILHPOWERV && Tile <= VRAILROAD
                                && Tile != RAILHPOWERV && Tile != HRAILROAD
                                && Tile != HRAIL) {
                adjTile |= 0x0001;
            }
        }

        if (x < WORLD_X - 1) {
            Tile = TileAdrPtr[WORLD_Y];
            NeutralizeRoad(Tile);
            if (Tile >= RAILHPOWERV && Tile <= VRAILROAD
                                && Tile != RAILVPOWERH && Tile != VRAILROAD
                                && Tile != VRAIL) {
                adjTile |= 0x0002;
            }
        }

        if (y < WORLD_Y - 1) {
            Tile = TileAdrPtr[1];
            NeutralizeRoad(Tile);
            if (Tile >= RAILHPOWERV && Tile <= VRAILROAD
                                && Tile != RAILHPOWERV && Tile != HRAILROAD
                                && Tile != HRAIL) {
                adjTile |= 0x0004;
            }
        }

        if (x > 0) {
            Tile = TileAdrPtr[-WORLD_Y];
            NeutralizeRoad(Tile);
            if (Tile >= RAILHPOWERV && Tile <= VRAILROAD
                                && Tile != RAILVPOWERH && Tile != VRAILROAD
                                && Tile != VRAIL) {
                adjTile |= 0x0008;
            }
        }

        *TileAdrPtr = RailTable[adjTile] | BULLBIT | BURNBIT;
        return;
    }

    if (Tile >= LHPOWER && Tile <= LVPOWER10) {         /* Cleanup Wire */

        if (y > 0) {
            Tile = TileAdrPtr[-1];
            if (Tile & CONDBIT) {
                NeutralizeRoad(Tile);
                if (Tile != VPOWER && Tile != VROADPOWER && Tile != RAILVPOWERH) {
                    adjTile |= 0x0001;
                }
            }
        }

        if (x < WORLD_X - 1) {
            Tile = TileAdrPtr[WORLD_Y];
            if (Tile & CONDBIT) {
                NeutralizeRoad(Tile);
                if (Tile != HPOWER && Tile != HROADPOWER && Tile != RAILHPOWERV) {
                    adjTile |= 0x0002;
                }
            }
        }

        if (y < WORLD_Y - 1) {
            Tile = TileAdrPtr[1];
            if (Tile & CONDBIT) {
                NeutralizeRoad(Tile);
                if (Tile != VPOWER && Tile != VROADPOWER && Tile != RAILVPOWERH) {
                    adjTile |= 0x0004;
                }
            }
        }

        if (x > 0) {
            Tile = TileAdrPtr[-WORLD_Y];
            if (Tile & CONDBIT) {
                NeutralizeRoad(Tile);
                if (Tile != HPOWER && Tile != HROADPOWER && Tile != RAILHPOWERV) {
                    adjTile |= 0x0008;
                }
            }
        }

        *TileAdrPtr = WireTable[adjTile] | BULLBIT | BURNBIT | CONDBIT;
        return;
    }
}


////////////////////////////////////////////////////////////////////////
