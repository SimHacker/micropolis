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
import micropolisrobot
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


    zoneType = 'root'
    width = 3
    height = 3
    hotX = 1
    hotY = 1
    messageCount = 0
    messageNumber = 0
    messageProbability = 0
    trafficProbabilityResidential = 0
    trafficProbabilityCommercial = 0
    trafficProbabilityIndustrial = 0
    robotProbability = 0
    maxRobots = 0
    robotClass = None


    def __init__(
        self,
        engine=None,
        x=0,
        y=0,
        churchNumber=0,
        **args):

        #print "CREATE ZONE", x, y, churchNumber

        self.engine = engine
        self.x = x
        self.y = y
        self.churchNumber = churchNumber
        self.baseTile, self.hotTile = ChurchTiles[churchNumber]
        #print "CHURCH NUMBER", churchNumber, "BASE", self.baseTile, "HOT", self.hotTile
        self.robots = []
        self.zoneID = UniqueNumber()
        self.score = 0


    def addScore(self, delta):
        self.score += delta


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


    def addRobot(self, robot):
        #print "ZONE ADD ROBOT", robot, "ZONE", self
        robots = self.robots
        if robot not in robots:
            robots.append(robot)


    def removeRobot(self, robot):
        #print "ZONE REMOVE ROBOT", robot, "ZONE", self
        robots = self.robots
        if robot in robots:
            robots.remove(robot)


    def destroy(self):
        #print "DESTROY ZONE", self, self.x, self.y, self.robots
        # TODO: destroy the zone.

        for robot in list(self.robots):
            robot.destroy()

        self.engine.removeZone(self)


    def findRoads(self):

        engine = self.engine
        roads = []

        x0 = self.x - self.hotX - 1
        y0 = self.y - self.hotY - 1
        x1 = x0 + self.width + 1
        y1 = y0 + self.height + 1
        for x in range(x0, x1 + 1):
            if engine.isRoad(x, y0):
                roads.append((x, y0))
            if engine.isRoad(x, y1):
                roads.append((x, y1))
        for y in range(y0 + 1, y1):
            if engine.isRoad(x0, y):
                roads.append((x0, y))
            if engine.isRoad(x1, y):
                roads.append((x1, y))

        return roads


    def findRandomRoad(self):
        roads = self.findRoads()
        if not roads:
            return None
        return self.getAny(roads)


    def generateTraffic(self):
        engine = self.engine
        x = self.x
        y = self.y
        if random.random() < self.trafficProbabilityResidential:
            engine.makeTraffic(x, y, micropolisengine.ZT_RESIDENTIAL)
        if random.random() < self.trafficProbabilityCommercial:
            engine.makeTraffic(x, y, micropolisengine.ZT_COMMERCIAL)
        if random.random() < self.trafficProbabilityIndustrial:
            engine.makeTraffic(x, y, micropolisengine.ZT_INDUSTRIAL)


    def generateRobots(self):
        robots = self.robots
        if ((len(robots) < self.maxRobots) and
            (random.random() < self.robotProbability)):
            pos = self.findRandomRoad()
            if pos:
                self.createRobot(pos[0], pos[1])


    def createRobot(self, x, y):
        print "CREATE ROBOT", x, y, self, self.robotClass
        robot = self.robotClass(
            x=(x * 16) + 8,
            y=(y * 16) + 8,
            direction = random.randint(0, 3) * math.pi / 2,
            zone=self)
        self.addRobot(robot)
        self.engine.addRobot(robot)


    def generateMessages(self):
        if random.random() < self.messageProbability:
            self.sendNextMessage()


    def sendNextMessage(self):
        messageNumber = self.messageNumber
        self.messageNumber = (self.messageNumber + 1) % self.messageCount
        messageNameBase = 'zone-' + self.zoneType + '-message-' + str(messageNumber)
        #print messageNameBase
        self.engine.sendSessions({
            'message': 'update',
            'variable': 'notice',
            'notice': {
                'title': messageNameBase + '-title',
                'description': messageNameBase + '-text',
                'url': messageNameBase + '-url',
                'showPicture': False,
                'picture': None,
                'showMap': True,
                'x': self.x,
                'y': self.y,
            },
        })


    # TODO: make this a utility.
    def getAny(self, a):
        return a[random.randint(0, len(a) - 1)]


    def simulate(self):

        self.generateTraffic()
        self.generateRobots()
        self.generateMessages()


########################################################################


class MicropolisZone_Church0(MicropolisZone):


    zoneType = 'christian'


########################################################################
# Church of PacMania


class MicropolisZone_ChurchOfPacMania(MicropolisZone):


    zoneType = 'pacmania'
    messageProbability = 0.1
    messageCount = 5
    trafficProbabilityResidential = 1.0
    trafficProbabilityCommercial = 1.0
    trafficProbabilityIndustrial = 1.0
    robotProbability = 0.1
    maxRobots = 4
    robotClass = micropolisrobot.MicropolisRobot_PacBot


########################################################################


class MicropolisZone_ChurchOfScientology(MicropolisZone):


    zoneType = 'scientology'
    messageProbability = 0.1
    messageCount = 30
    trafficProbabilityResidential = 1.0
    trafficProbabilityCommercial = 1.0
    trafficProbabilityIndustrial = 1.0
    robotProbability = 0.1
    maxRobots = 1
    robotClass = micropolisrobot.MicropolisRobot_Xenu


########################################################################


class MicropolisZone_Church3(MicropolisZone):


    zoneType = 'church3'


########################################################################


class MicropolisZone_Church4(MicropolisZone):


    zoneType = 'church4'


########################################################################


class MicropolisZone_Church5(MicropolisZone):


    zoneType = 'church5'


########################################################################


class MicropolisZone_Church6(MicropolisZone):


    zoneType = 'church6'



########################################################################


class MicropolisZone_Church7(MicropolisZone):


    zoneType = 'church7'


########################################################################


ZoneClasses = [
    MicropolisZone_Church0,
    MicropolisZone_ChurchOfPacMania,
    MicropolisZone_ChurchOfScientology,
    MicropolisZone_Church3,
    MicropolisZone_Church4,
    MicropolisZone_Church5,
    MicropolisZone_Church6,
    MicropolisZone_Church7,
]


########################################################################
