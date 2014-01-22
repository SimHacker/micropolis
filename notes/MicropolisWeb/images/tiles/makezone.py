# makezone.py
#
# Micropolis, Unix Version.  This game was released for the Unix platform
# in or about 1990 and has been modified for inclusion in the One Laptop
# Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
# you need assistance with this program, you may contact:
#   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.  You should have received a
# copy of the GNU General Public License along with this program.  If
# not, see <http://www.gnu.org/licenses/>.
#
#             ADDITIONAL TERMS per GNU GPL Section 7
#
# No trademark or publicity rights are granted.  This license does NOT
# give you any right, title or interest in the trademark SimCity or any
# other Electronic Arts trademark.  You may not distribute any
# modification of this program using the trademark SimCity or claim any
# affliation or association with Electronic Arts Inc. or its employees.
#
# Any propagation or conveyance of this program must include this
# copyright notice and these terms.
#
# If you convey this program (or any modifications of it) and assume
# contractual liability for the program to recipients of it, you agree
# to indemnify Electronic Arts for any liability that those contractual
# assumptions impose on Electronic Arts.
#
# You may not misrepresent the origins of this program; modified
# versions of the program must be marked as such and not identified as
# the original program.
#
# This disclaimer supplements the one included in the General Public
# License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
# PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
# OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
# SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
# DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
# INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
# FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
# RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
# USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
# INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
# MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
# UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
# WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
# CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
# ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
# JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
# WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
# CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
# NOT APPLY TO YOU.


########################################################################
# Make Zone
# Don Hopkins


import os, sys, math
import cairo


########################################################################
# Globals


TileSize = 16
TileCount = 1024
TilesFileName = 'micropolis_tiles.png'
ZoneFileNameTemplate = 'zone_%s.png'
TileFileNameTemplate = 'micropolis_tile_%04d.png'


ZoneTable = {
    'church0': {
        'base': 414,
        'size': 3,
    },
    'church1': {
        'base': 956,
        'size': 3,
    },
    'church2': {
        'base': 965,
        'size': 3,
    },
    'church3': {
        'base': 974,
        'size': 3,
    },
    'church4': {
        'base': 983,
        'size': 3,
    },
    'church5': {
        'base': 992,
        'size': 3,
    },
    'church6': {
        'base': 1001,
        'size': 3,
    },
    'church7': {
        'base': 1010,
        'size': 3,
    },
}


########################################################################
# Utilities


def main():
    if len(sys.argv) not in (2, 3, 4,):
        print "Usage: makezone.py all -- make all zone s"
        print "Usage: makezone.py zoneIDs [zoneImage] [tileDir] -- make zone(s)"
        print "zoneIDs: comma separated list of zone IDs"
        print "zoneImage: Name of PNG file with image of zone. Optional, defaults to zone_<zoneID>.png"
        print "tileDir: Name of output tile directory. Optional, defaults to current directory."
        sys.exit(1)

    args = sys.argv
    argCount = len(args)

    print "ARGCOUNT", argCount, "ARGS[1]", args[1]
    if (argCount == 2) and (args[1] == 'all'):
        zoneIDs = sorted(ZoneTable.keys())
    else:
        zoneIDs = args[1].split(',')

    if argCount > 3:
        tileDir = args[2]
    else:
        tileDir = os.getcwd()

    if not os.path.exists(tileDir):
        print "Tile directory does not exist:", tileDir

    print "ZONEIDS", zoneIDs
    for zoneID in zoneIDs:

        if zoneID not in ZoneTable:
            print "Undefined zoneID", zoneID, "should be one of:", ZoneTable.keys()
            sys.exit(2)

        if argCount > 2:
            zoneImageFileName = args[2]
        else:
            zoneImageFileName = os.path.join(
                tileDir,
                ZoneFileNameTemplate % (zoneID,))

        if not os.path.exists(zoneImageFileName):
            print "Zone image file does not exist:", zoneImageFileName
            exit(3)

        MakeZone(zoneID, zoneImageFileName, tileDir)

    MakeAllTiles(tileDir)


def MakeZone(zoneID, zoneImageFileName, tileDir):
    print "MakeZone", zoneID, zoneImageFileName, tileDir

    zoneInfo = ZoneTable[zoneID]
    base = zoneInfo['base']
    size = zoneInfo['size']

    zoneImage = cairo.ImageSurface.create_from_png(zoneImageFileName)

    tileImage = cairo.ImageSurface(cairo.FORMAT_ARGB32, TileSize, TileSize)
    ctx = cairo.Context(tileImage)

    for row in range(0, size):
        for col in range(0, size):
            offset = (row * size) + col
            tileNumber = base + offset
            tileFileName = os.path.join(
                tileDir,
                TileFileNameTemplate % (tileNumber,))

            #print row, col, tileFileName

            ctx.set_source_surface(
                zoneImage,
                -col * TileSize,
                -row * TileSize)
            ctx.paint()

            tileImage.write_to_png(tileFileName)
            print "Wrote tile", tileFileName


def MakeAllTiles(tileDir):

    print "Make All Tiles"

    cols = 32
    rows = int(math.ceil(TileCount / float(cols)))

    tileImages = []

    allTilesImage = cairo.ImageSurface(cairo.FORMAT_ARGB32, TileSize * cols, TileSize * rows)
    ctx = cairo.Context(allTilesImage)

    for tileNumber in range(0, TileCount):
        col = tileNumber % cols
        row = int(tileNumber / cols)
        tileFileName = os.path.join(
            tileDir,
            TileFileNameTemplate % (tileNumber,))
        tileImage = cairo.ImageSurface.create_from_png(
            tileFileName)
        #print tileNumber, col, row, tileFileName
        ctx.set_source_surface(
            tileImage,
            col * TileSize,
            row * TileSize)
        ctx.paint()


    allTilesImage.write_to_png(TilesFileName)
    print "Wrote tiles", TilesFileName


########################################################################


if __name__ == '__main__':

    main()


########################################################################
