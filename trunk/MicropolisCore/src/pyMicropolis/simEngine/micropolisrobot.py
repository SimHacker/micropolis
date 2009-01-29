# micropolisrobot.py
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
# Micropolis Robot
# Don Hopkins


########################################################################
# Import Modules


import random
import math
import micropolisengine
import micropolistool
from pyMicropolis.tileEngine import tiletool


########################################################################


class MicropolisRobot:


    def __init__(
        self,
        engine=None,
        x=0,
        y=0,
        direction=0.0,
        **args):
      
        self.engine = engine
        self.x = x
        self.y = y
        self.direction = 0.0


    def simulate(self):

        engine = self.engine


    def draw(self, ctx):

        ctx.save()
        
        ctx.translate(self.x, self.y)

        ctx.rotate(self.direction)

        self.drawRobot(ctx)

        ctx.restore()


    def drawRobot(self, ctx):

        ctx.rectangle(-7, -7, 14, 14)
        ctx.set_source_rgba(0.5, 0.5, 0.5, 0.5)
        ctx.fill_preserve()
        ctx.set_source_rgba(0, 0, 0, 0.5)
        ctx.set_line_width(1)
        ctx.stroke()
        ctx.move_to(0, 0)
        ctx.line_to(8, 0)
        ctx.set_line_cap(cairo.LINE_JOIN_ROUND)
        ctx.stroke()


########################################################################


class MicropolisRobot_PacMan(MicropolisRobot):

    
    def __init__(
        self,
        speed=4,
        defaultSpeed=4,
        direction=0,
        mouthOpen=True,
        mouthOpenCycle=30,
        mouthOpenDuration=15,
        mouthSize=math.pi / 2.0,
        radius=10,
        **args):

        MicropolisRobot.__init__(self, **args)

        self.speed = speed
        self.direction = direction
        self.mouthOpen = mouthOpen
        self.mouthOpenCycle = mouthOpenCycle
        self.mouthOpenDuration = mouthOpenDuration
        self.mouthPhase = random.randint(0, mouthOpenCycle - 1)
        self.mouthSize = mouthSize
        self.radius = radius
        self.defaultSpeed = defaultSpeed
        self.hilite = 0
        self.score = 0


    def simulate(self):

        engine = self.engine
        getTile = engine.getTile

        def getAny(a):
            return a[random.randint(0, len(a) - 1)]

        def isRoad(tile):
            tile = tile & micropolisengine.LOMASK
            return ((tile >= micropolisengine.ROADBASE) and
                    (tile < micropolisengine.POWERBASE))

        self.mouthOpen = (
            ((engine.tickCount() + self.mouthPhase) % self.mouthOpenCycle) <
            self.mouthOpenDuration)

        x = self.x
        y = self.y
        speed = self.speed
        direction = self.direction
        defaultSpeed = self.defaultSpeed

        maxX = int((micropolisengine.WORLD_W * 16) / defaultSpeed) * defaultSpeed
        maxY = int((micropolisengine.WORLD_H * 16) / defaultSpeed) * defaultSpeed

        eastDir = 0 * math.pi / 2.0
        northDir = 1 * math.pi / 2.0
        westDir = 2 * math.pi / 2.0
        southDir = 3 * math.pi / 2.0
        allDirs = [eastDir, northDir, westDir, southDir]

        frustrated = False

        # Snap into grid lanes.

        x = int(x / defaultSpeed) * defaultSpeed
        y = int(y / defaultSpeed) * defaultSpeed

        inLaneVertical = (x % 16) == 8
        inLaneHorizontal = (y % 16) == 8
        inIntersection = inLaneVertical and inLaneHorizontal

        if (not inLaneVertical) and (not inLaneHorizontal):
            x = (int(x / 16) * 16) + 8
            y = (int(y / 16) * 16) + 8
            inLaneVertical = True
            inLaneHorizontal = True
            inIntersection = True

        if inIntersection:
            self.hilite = 1
            #print "inIntersection"
        elif inLaneVertical:
            self.hilite = 2
            #print "inLaneVertical"
        elif inLaneHorizontal:
            self.hilite = 3
            #print "inLaneHorizontal"
        else:
            #print "BAD PACMAN: not in lane or intersection!!!"
            pass

        dx = math.cos(direction) * speed
        dy = -math.sin(direction) * speed

        #print "direction", direction, "dx", dx, "dy", dy
        if (dx == 0) and (dy == 0):
            # Sitting still.
            pass
        if abs(dx) > abs(dy):
            # Horizontal
            dy = 0
            if not inLaneHorizontal:
                dx = 0
                #print "frustrated horizontal"
                frustrated = True
                self.hilite = 5
            else:
                if dx < 0:
                    dx = -defaultSpeed
                    #print "force west"
                else:
                    dx = defaultSpeed
                    #print "force east"
        else:
            # Vertical
            dx = 0
            if not inLaneVertical:
                dy = 0
                #print "frustrated vertical"
                frustrated = True
                self.hilite = 5
            else:
                if dy < 0:
                    dy = -defaultSpeed
                    #print "force north"
                else:
                    dy = defaultSpeed
                    #print "force south"

        if (dx == 0) and (dy == 0):
            #print "STOPPED direction", direction
            pass
        else:
            direction = math.atan2(-dy, dx)
            #print "direction", direction

        # Calculate tile coordinates.

        tileX = int(x / 16)
        tileY = int(y / 16)

        tile = getTile(tileX, tileY)
        onRoad = isRoad(tile)
        #print "onRoad", onRoad, "x", tileX, "y", tileY, "tile", tile

        if not onRoad:

            dx = 0
            dy = 0

        else:

            # Eat traffic.

            trafficX = int(tileX / 2)
            trafficY = int(tileY / 2)
            trafficDensity = engine.getTrafficDensity(trafficX, trafficY)
            engine.setTrafficDensity(trafficX, trafficY, 0)
            if trafficDensity:
                self.score += trafficDensity
                #print "ATE TRAFFIC", trafficDensity, "SCORE", self.score

            tileBits = tile & ~micropolisengine.LOMASK
            tileNumber = tile & micropolisengine.LOMASK
            if (tileNumber >= 64) and (tileNumber <= 207):
                #print "neutralizing tile", tile
                tileNumber = (tileNumber & 0x000F) + 64
                #print "Setting tile", tileX, tileY, tileNumber
                tile = tileNumber | tileBits
                engine.setTile(tileX, tileY, tile)

            # Look for adjacent roads.

            roadNorth = isRoad(getTile(tileX, tileY - 1))
            roadSouth = isRoad(getTile(tileX, tileY + 1))
            roadEast = isRoad(getTile(tileX + 1, tileY))
            roadWest = isRoad(getTile(tileX - 1, tileY))
            #print "e", roadEast, "n", roadNorth, "w", roadWest, "s", roadSouth

            if inIntersection:

                #print "INTERSECTION"

                if inLaneHorizontal:
                    if dx < 0:
                        if not roadWest:
                            #print "blocked to west so no dx", dx
                            dx = 0
                    elif dx > 0:
                        if not roadEast:
                            #print "blocked to east so no dx", dx
                            dx = 0
                else:
                    #print "not in horizontal lane, so no dx", dx
                    dx = 0

                if inLaneVertical:
                    if dy < 0:
                        if not roadNorth:
                            #print "blocked to north so no dy", dy
                            dy = 0
                    elif dy > 0:
                        if not roadSouth:
                            #print "blocked to south so no dy", dy
                            dy = 0
                else:
                    #print "not in vertical lane, so no dy", dy
                    dy = 0

                dirs = []
                if roadEast:
                    dirs.append((defaultSpeed, 0))
                if roadWest:
                    dirs.append((-defaultSpeed, 0))
                if roadSouth:
                    dirs.append((0, defaultSpeed))
                if roadNorth:
                    dirs.append((0, -defaultSpeed))

                #print "DIRS", len(dirs), dirs

                if len(dirs) == 0:

                    # On a road island.
                    dx = 0
                    dy = 0
                    #print "frustrated island"
                    frustrated = True
                    self.hilite = 5

                else:

                    keepGoingProb = 0.95

                    if ((len(dirs) == 1) or # Dead end? 
                        (len(dirs) > 2) or # intersection?
                        ((dx > 0) and (not roadEast)) or # End of east road?
                        ((dx < 0) and (not roadWest)) or # End if west road?
                        ((dy > 0) and (not roadSouth)) or # End of south road?
                        ((dy < 0) and (not roadNorth)) or # End of north road?
                        (random.random() > keepGoingProb)): # Why the fuck not?
                        # Choose a random direction to go.
                        dx, dy = getAny(dirs)
                        #print "RANDOM DIRECTION", dx, dy

        #print "DX", dx, "DY", dy

        if (dx == 0) and (dy == 0):
            #print "stopped"
            speed = defaultSpeed
        else:
            direction = math.atan2(-dy, dx)
            speed = defaultSpeed
            #print "direction", direction

        # Calculate velocity and move position.

        x += dx
        y += dy

        self.x = x
        self.y = y
        self.direction = direction
        self.speed = speed
        self.hilite = 0 # disable hilite

        #print "NOW", "x", x, "y", y, "direction", direction, "speed", speed


    def drawRobot(self, ctx):

        direction = self.direction
        mouthOpen = self.mouthOpen
        mouthSize = self.mouthSize
        radius = self.radius
        hilite = self.hilite

        ctx.rotate(direction)

        if mouthOpen:
            ctx.move_to(0, 0)
            ctx.arc(0, 0, radius, direction + (mouthSize / 2), direction + (2 * math.pi) - (mouthSize / 2))
            ctx.line_to(0, 0)
            ctx.close_path()
        else:
            ctx.arc(0, 0, radius, 0, 2* math.pi)
            ctx.close_path()

        if hilite == 0:
            ctx.set_source_rgba(1, 1, 0, 1)
        elif hilite == 1:
            ctx.set_source_rgba(1, 0, 0, 1)
        elif hilite == 2:
            ctx.set_source_rgba(0, 1, 0, 1)
        elif hilite == 3:
            ctx.set_source_rgba(0, 0, 1, 1)
        elif hilite == 4:
            ctx.set_source_rgba(0, 1, 1, 1)
        elif hilite == 5:
            ctx.set_source_rgba(1, 0, 1, 1)
        elif hilite == 6:
            ctx.set_source_rgba(0, 0, 0, 1)
        elif hilite == 7:
            ctx.set_source_rgba(1, 1, 1, 1)
        else:
            ctx.set_source_rgba(0.5, 0.5, 0.5, 1)
        ctx.fill_preserve()
        ctx.set_source_rgba(0, 0, 0, 1)
        ctx.set_line_width(1)
        ctx.stroke()


########################################################################
