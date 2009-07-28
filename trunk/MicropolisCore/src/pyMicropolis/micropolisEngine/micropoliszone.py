# micropoliszone.py
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
# Micropolis Zone
# Don Hopkins


########################################################################
# Import Modules


import random
import math
import micropolisengine
import micropolistool
from pyMicropolis.tileEngine import tiletool


########################################################################
# Globals


# Array of (base tile number, center tile number) of churches.
ChurchTiles = (
    (micropolisengine.CHURCH0BASE, micropolisengine.CHURCH0,),
    (micropolisengine.CHURCH1BASE, micropolisengine.CHURCH1,),
    (micropolisengine.CHURCH2BASE, micropolisengine.CHURCH2,),
    (micropolisengine.CHURCH3BASE, micropolisengine.CHURCH3,),
    (micropolisengine.CHURCH4BASE, micropolisengine.CHURCH4,),
    (micropolisengine.CHURCH5BASE, micropolisengine.CHURCH5,),
    (micropolisengine.CHURCH6BASE, micropolisengine.CHURCH6,),
    (micropolisengine.CHURCH7BASE, micropolisengine.CHURCH7,),
)


########################################################################


# TODO: Centralize this in a utility module.

UniqueNumberNext = 0

def UniqueNumber():
    global UniqueNumberNext
    UniqueNumberNext += 1
    return UniqueNumberNext


########################################################################


class MicropolisZone:


    width = 3
    height = 3
    hotX = 1
    hotY = 1


    def __init__(
        self,
        engine=None,
        x=0,
        y=0,
        churchNumber=0,
        **args):

        self.engine = engine
        self.x = x
        self.y = y
        self.churchNumber = churchNumber
        self.baseTile, self.hotTile = ChurchTiles[churchNumber]

        self.zoneID = UniqueNumber()


    # High frequency tick.
    def tick(self):

        # If our center tile is no longer hotTile, the zone was
        # destroyed somehow (bulldozer, fire, etc), so we destroy the
        # Python zone object.
        #print "ZONE TICK", self, self.x, self.y
        engine = self.engine
        tile = engine.getTile(self.x, self.y) & micropolisengine.LOMASK
        if tile != self.hotTile:
            #print "ZONE TICK EXPECTED TILE", self.hotTile, "GOT TILE", tile, "GOODBYE!"
            self.destroy()

        # TODO: Destroy all tiles when reseting city.


    # Occasional simulate.
    def simulate(self):

        # print "ZONE SIMULATE", self, self.x, self.y
        pass

    def destroy(self):
        #print "ZONE DESTROY", self, self.x, self.y
        # TODO: destroy the zone.
        self.engine.removeZone(self)


########################################################################


class MicropolisZone_Church0(MicropolisZone):

    pass


########################################################################


class MicropolisZone_Church1(MicropolisZone):

    def simulate(self):

        engine = self.engine
        x = self.x
        y = self.y

        # The Pacmania church generates lots of traffic, in order
        # to appease the PacBots, which who are attracted to the
        # traffic and eat it. 
        engine.makeTraffic(x, y, micropolisengine.ZT_RESIDENTIAL)
        engine.makeTraffic(x, y, micropolisengine.ZT_COMMERCIAL)
        engine.makeTraffic(x, y, micropolisengine.ZT_INDUSTRIAL)


########################################################################


class MicropolisZone_Church2(MicropolisZone):

    pass


########################################################################


class MicropolisZone_Church3(MicropolisZone):

    pass


########################################################################


class MicropolisZone_Church4(MicropolisZone):

    pass


########################################################################


class MicropolisZone_Church5(MicropolisZone):

    pass


########################################################################


class MicropolisZone_Church6(MicropolisZone):

    pass


########################################################################


class MicropolisZone_Church7(MicropolisZone):

    pass


########################################################################
