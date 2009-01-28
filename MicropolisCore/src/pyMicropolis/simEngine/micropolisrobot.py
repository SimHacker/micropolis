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
        speed=0.0,
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
        self.mouthSize = mouthSize
        self.radius = radius
        self.hilite = False
        self.score = 0


    def simulate(self):

        engine = self.engine

        tickCount = engine.tickCount()
        self.mouthOpen = \
            (tickCount % self.mouthOpenCycle) < self.mouthOpenDuration

        x = self.x
        y = self.y
        speed = self.speed
        direction = self.direction
        maxX = micropolisengine.WORLD_W * 16
        maxY = micropolisengine.WORLD_H * 16

        dx = math.cos(direction) * speed
        dy = -math.sin(direction) * speed
        x += dx
        y += dy

        turned = False

        if x < 0:
            x = 0
            dx = -dx
            turned = True
        elif x > maxX:
            x = maxX
            dx = -dx
            turned = True

        if y < 0:
            y = 0
            dy = -dy
            turned = True
        elif y > maxY:
            y = maxY
            dy = -dy
            turned = True

        distance = math.sqrt((dx * dx) + (dy * dy))

        dir = -1 # none
        if (dx == 0) and (dy == 0):
            pass
        elif abs(dx) > abs(dy):
            x = int(x)
            y = (int(y / 16) * 16) + 8
            dy = 0
            if dx < 0:
                dx = -1
                dir = 2 # west
            else:
                dx = 1
                dir = 0 # east
        else:
            x = (int(x / 16) * 16) + 8
            y = int(y)
            dx = 0
            if dy < 0:
                dy = -1
                dir = 1 # north
            else:
                dy = 1
                dir = 3 # south

        if (dx != 0) and (dy != 0):
            direction = math.atan2(dy, dx)

        onTile = ((x % 16) == 8) and ((y % 16) == 8)
        #self.hilite = onTile

        tileX = int(x / 16)
        tileY = int(y / 16)

        def isRoad(tile):
            return ((tile >= micropolisengine.ROADBASE) and
                    (tile < micropolisengine.POWERBASE))

        def getTile(dx, dy):
            return engine.getTile(tileX + dx, tileY + dy) & micropolisengine.LOMASK

        def getAny(a):
            return a[random.randint(0, len(a) - 1)]

        trafficX = int(tileX / 2)
        trafficY = int(tileY / 2)
        trafficDensity = engine.getTrafficDensity(trafficX, trafficY)
        engine.setTrafficDensity(trafficX, trafficY, 0)
        self.score += trafficDensity
        #print trafficDensity, self.score

        if onTile:
            tile = getTile(0, 0)
            if isRoad(tile):

                roadNorth = isRoad(getTile(0, -1))
                roadSouth = isRoad(getTile(0, 1))
                roadEast = isRoad(getTile(1, 0))
                roadWest = isRoad(getTile(-1, 0))
                #print "e", roadEast, "n", roadNorth, "w", roadWest, "s", roadSouth

                if ((not roadNorth) and
                    (not roadSouth) and
                    (not roadEast) and
                    (not roadWest)):
                    direction = math.randint(0, 3) * (math.pi / 2.0)
                    #print "oof!", direction
                    speed = 0
                else:
                    dirs = []
                    if roadEast:
                        dirs.append(0 * math.pi / 2.0)
                    if roadNorth:
                        dirs.append(1 * math.pi / 2.0)
                    if roadWest:
                        dirs.append(2 * math.pi / 2.0)
                    if roadSouth:
                        dirs.append(3 * math.pi / 2.0)

                    mightKeepGoingProb = 0.9
                    canKeepGoing = (
                        ((dir == 0) and roadEast) or
                        ((dir == 1) and roadNorth) or
                        ((dir == 2) and roadWest) or
                        ((dir == 3) and roadSouth))
                    if (canKeepGoing and
                        (len(dirs) < 3) and
                        (random.random() < mightKeepGoingProb)):
                        pass
                    else:
                        direction = getAny(dirs)
                        speed = 1

        self.x = x
        self.y = y
        self.direction = direction
        self.speed = speed


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

        if hilite:
            ctx.set_source_rgba(1, 0, 0, 1)
        else:
            ctx.set_source_rgba(1, 1, 0, 1)
        ctx.fill_preserve()
        ctx.set_source_rgba(0, 0, 0, 1)
        ctx.set_line_width(1)
        ctx.stroke()


########################################################################
