# micropolismodel.py
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
# Micropolis Model
# Don Hopkins


########################################################################
# Import stuff


import sys
import os
import time
import random
import micropolisengine
import gobject
import cairo
from pyMicropolis.tileEngine import tileengine


########################################################################
# Globals


__version__ = "0.9"


########################################################################
# MicropolisModel Class


class MicropolisModel(micropolisengine.Micropolis):


    messages = [
        '',
        'More residential zones needed.', # 1
        'More commercial zones needed.',
        'More industrial zones needed.',
        'More roads required.',
        'Inadequate rail system.', # 5
        'Build a power plant.',
        'Residents demand a stadium.',
        'Industry requires a sea port.',
        'Commerce requires an airport.',
        'Pollution very high.', # 10
        'Crime very high.',
        'Frequent traffic jams reported.',
        'Citizens demand a fire department.',
        'Citizens demand a police department.',
        'Blackouts reported. Check power map.', # 15
        'Citizens upset. The tax rate is too high.',
        'Roads deteriorating, due to lack of funds.',
        'Fire departments need funding.',
        'Police departments need funding.',
        'Fire reported !', # 20
        'A monster has been sighted !!',
        'Tornado reported !!',
        'Major earthquake reported !!!',
        'A plane has crashed !',
        'Shipwreck reported !', # 25
        'A train crashed !',
        'A helicopter crashed !',
        'Unemployment rate is high.',
        'YOUR CITY HAS GONE BROKE!',
        'Firebombing reported !', # 30
        'Need more parks.',
        'Explosion detected !',
        'Insufficient funds to build that.',
        'Area must be bulldozed first.',
        'Population has reached 2,000.', # 35
        'Population has reached 10,000.',
        'Population has reached 50,000.',
        'Population has reached 100,000.',
        'Population has reached 500,000.',
        'Brownouts, build another power plant.', # 40
        'Heavy traffic reported.',
        'Flooding reported !!',
        'A nuclear meltdown has occurred !!!',
        'They\'re rioting in the streets !!',
        'Started a new city.', # 45
        'Restored a saved city.',
        'You won the scenario!',
        'You lost the scenario.',
        'About Micropolis.',
        'Scenario: DULLSVILLE, USA  1900.', # 50
        'Scenario: SAN FRANCISCO, CA.  1906.',
        'Scenario: HAMBURG, GERMANY  1944.',
        'Scenario: BERN, SWITZERLAND  1965.',
        'Scenario: TOKYO, JAPAN  1957.',
        'Scenario: DETROIT, MI.  1972.', # 55
        'Scenario: BOSTON, MA.  2010.',
        'Scenario: RIO DE JANEIRO, BRAZIL  2047.', # 57
    ]


    notices = {

        10: {
            'id': 10,
            'color': 0xff4f4f,
            'title': 'POLLUTION ALERT!',
            'description': """Pollution in your city has exceeded the maximum allowable amounts established by the Micropolis Pollution Agency. You are running the risk of grave ecological consequences.

Either clean up your act or open a gas mask concession at city hall.""",
        },

        11: {
            'id': 11,
            'color': 0xff4f4f,
            'title': 'CRIME ALERT!',
            'description': """Crime in your city is out of hand. Angry mobs are looting and vandalizing the central city. The president will send in the national guard soon if you cannot control the problem.""",
        },

        12: {
            'id': 12,
            'color': 0xff4f4f,
            'title': 'TRAFFIC WARNING!',
            'description': """Traffic in this city is horrible. The city gridlock is expanding. The commuters are getting militant.

Either build more roads and rails or get a bulletproof limo.""",
        },

        20: {
            'id': 20,
            'color': 0xff4f4f,
            'title': 'FIRE REPORTED!',
            'description': """A fire has been reported!""",
        },

        21: {
            'id': 21,
            'color': 0xff4f4f,
            'title': 'MONSTER ATTACK!',
            'description': """A large reptilian creature has been spotted in the water. It seems to be attracted to areas of high pollution. There is a trail of destruction wherever it goes.

All you can do is wait till he leaves, then rebuild from the rubble.""",
            'sprite': micropolisengine.SPRITE_MONSTER,
        },

        22: {
            'id': 22,
            'color': 0xff4f4f,
            'title': 'TORNADO ALERT!',
            'description': """A tornado has been reported! There's nothing you can do to stop it, so you'd better prepare to clean up after the disaster!""",
            'sprite': micropolisengine.SPRITE_TORNADO,
        },

        23: {
            'id': 23,
            'color': 0xff4f4f,
            'title': 'EARTHQUAKE!',
            'description': """A major earthquake has occurred! Put out the fires as quickly as possible, before they spread, then reconnect the power grid and rebuild the city.""",
        },

        24: {
            'id': 24,
            'color': 0xff4f4f,
            'title': 'PLANE CRASH!',
            'description': """A plane has crashed!""",
        },

        25: {
            'id': 25,
            'color': 0xff4f4f,
            'title': 'SHIPWRECK!',
            'description': """A ship has wrecked!""",
        },

        26: {
            'id': 26,
            'color': 0xff4f4f,
            'title': 'TRAIN CRASH!',
            'description': """A train has crashed!""",
        },

        27: {
            'id': 27,
            'color': 0xff4f4f,
            'title': 'HELICOPTER CRASH!',
            'description': """A helicopter has crashed!""",
        },

        30: {
            'id': 30,
            'color': 0xff4f4f,
            'title': 'FIREBOMBING REPORTED!',
            'description': """Firebombs are falling!!""",
        },

        # TODO: 32 explosion detected

        35: {
            'id': 35,
            'color': 0x7fff7f,
            'title': 'TOWN',
            'description': """Congratulations, your village has grown to town status. You now have 2,000 citizens.""",
        },

        36: {
            'id': 36,
            'color': 0x7fff7f,
            'title': 'CITY',
            'description': """Your town has grown into a full sized city, with a current population of 10,000. Keep up the good work!""",
        },

        37: {
            'id': 37,
            'color': 0x7fff7f,
            'title': 'CAPITAL',
            'description': """Your city has become a capital. The current population here is 50,000. Your political future looks bright.""",
        },

        38: {
            'id': 38,
            'color': 0x7fff7f,
            'title': 'METROPOLIS',
            'description': """Your capital city has now achieved the status of metropolis. The current population is 100,000. With your management skills, you should seriously consider running for governor.""",
        },

        39: {
            'id': 39,
            'color': 0x7fff7f,
            'title': 'MEGALOPOLIS',
            'description': """Congratulation, you have reached the highest category of urban development, the megalopolis.

If you manage to reach this level, send us email at micropolis@laptop.org or send us a copy of your city. We might do something interesting with it.""",
        },

        41: {
            'id': 41,
            'color': 0xff4f4f,
            'title': 'HEAVY TRAFFIC!',
            'description': """Sky Watch One reporting heavy traffic!""",
            'sprite': micropolisengine.SPRITE_HELICOPTER,
        },

        42: {
            'id': 42,
            'color': 0xff4f4f,
            'title': 'FLOODING REPORTED!',
            'description': """Flooding has been been reported along the water's edge!""",
        },

        43: {
            'id': 43,
            'color': 0xff4f4f,
            'title': 'NUCLEAR MELTDOWN!',
            'description': """A nuclear meltdown has occured at your power plant. You are advised to avoid the area until the radioactive isotopes decay.

Many generations will confront this problem before it goes away, so don't hold your breath.""",
        },


        44: {
            'id': 44,
            'color': 0xff4f4f,
            'title': 'RIOTS!',
            'description': """The citizens are rioting in the streets, setting cars and houses on fire, and bombing government buildings and businesses!

All media coverage is blacked out, while the fascist pigs beat the poor citizens into submission.""",
        },

        45: {
            'id': 48,
            'color': 0x7f7fff,
            'title': 'Start a New City',
            'description': """Build your very own city from the ground up, starting with this map of uninhabited land.""",
        },

        46: {
            'id': 49,
            'color': 0x7f7fff,
            'title': 'Restore a Saved City',
            'description': """The city was saved.""",
        },

        47: {
            'id': 100,
            'color': 0x7fff7f,
            'title': 'YOU\'RE A WINNER!',
            'description': """Your mayorial skill and city planning expertise have earned you the KEY TO THE CITY. Local residents will erect monuments to your glory and name their first-born children after you. Why not run for governor?""",
            'view': 'todo: show key to the city image',
        },

        48: {
            'id': 200,
            'color': 0xff4f4f,
            'title': 'IMPEACHMENT NOTICE!',
            'description': """The entire population of this city has finally had enough of your inept planning and incompetant management. An angry mob -- led by your mother -- has been spotted in the vicinity of city hall.

You should seriously consider taking an extended vacation -- NOW. (Or read the manual and try again.)""",
            'view': 'todo: show impeachment image',
        },

        49: {
            'id': 300,
            'color': 0xffd700,
            'title': 'About Micropolis',
            'description': """Micropolis Copyright (C) 2007 by Electronic Arts.
Based on the original SimCity concept, design and code by Will Wright and Fred Haslam.
User Interface Designed and created by Don Hopkins.
Ported to Linux, Optimized and Adapted for OLPC by Don Hopkins.
Licensed under the GNU General Public License, version 3, with additional conditions.""",
            'view': 'todo: show micropolis logo',
        },

        50: {
            'id': 1,
            'color': 0x7f7fff,
            'title': 'DULLSVILLE, USA  1900',
            'description': """Things haven't changed much around here in the last hundred years or so and the residents are beginning to get bored. They think Dullsville could be the next great city with the right leader.

It is your job to attract new growth and development, turning Dullsville into a Metropolis within 30 years.""",
        },

        51: {
            'id': 2,
            'color': 0x7f7fff,
            'title': 'SAN FRANCISCO, CA.  1906',
            'description': """Damage from the earthquake was minor compared to that of the ensuing fires, which took days to control. 1500 people died.

Controlling the fires should be your initial concern. Then clear the rubble and start rebuilding. You have 5 years.""",
        },

        52: {
            'id': 3,
            'color': 0x7f7fff,
            'title': 'HAMBURG, GERMANY  1944',
            'description': """Allied fire-bombing of German cities in WWII caused tremendous damage and loss of life. People living in the inner cities were at greatest risk.

You must control the firestorms during the bombing and then rebuild the city after the war. You have 5 years.""",
        },

        53: {
            'id': 4,
            'color': 0x7f7fff,
            'title': 'BERN, SWITZERLAND  1965',
            'description': """The roads here are becoming more congested every day, and the residents are upset. They demand that you do something about it.

Some have suggested a mass transit system as the answer, but this would require major rezoning in the downtown area. You have 10 years.""",
        },

        54: {
            'id': 5,
            'color': 0x7f7fff,
            'title': 'TOKYO, JAPAN  1957',
            'description': """A large reptilian creature has been spotted heading for Tokyo bay. It seems to be attracted to the heavy levels of industrial pollution there.

Try to control the fires, then rebuild the industrial center. You have 5 years.""",
        },

        55: {
            'id': 6,
            'color': 0x7f7fff,
            'title': 'DETROIT, MI.  1972',
            'description': """By 1970, competition from overseas and other economic factors pushed the once "automobile capital of the world" into recession. Plummeting land values and unemployment then increased crime in the inner-city to chronic levels.

You have 10 years to reduce crime and rebuild the industrial base of the city."""
        },

        56: {
            'id': 7,
            'color': 0x7f7fff,
            'title': 'BOSTON, MA.  2010',
            'description': """A major meltdown is about to occur at one of the new downtown nuclear reactors. The area in the vicinity of the reactor will be severly contaminated by radiation, forcing you to restructure the city around it.

You have 5 years to get the situation under control.""",
        },

        57: {
            'id': 8,
            'color': 0x7f7fff,
            'title': 'RIO DE JANEIRO, BRAZIL  2047',
            'description': """In the mid-21st century, the greenhouse effect raised global temperatures 6 degrees F. Polar icecaps melted and raised sea levels worldwide. Coastal areas were devastated by flood and erosion.

You have 10 years to turn this swamp back into a city again.""",
        },

    }


    def __init__(
            self,
            running=True,
            timeDelay=10,
            *args,
            **kwargs):
        print "MicropolisModel.__init__", self, "calling micropolisengine.Micropolis.__init__", micropolisengine.Micropolis.__init__, args, kwargs

        micropolisengine.Micropolis.__init__(self, *args, **kwargs)

        self.running = running
        self.timeDelay = timeDelay
        self.timerActive = False
        self.timerId = None
        self.views = []
        self.interests = {}
        self.powerDataImage = None
        self.trafficDataImage = None
        self.dataTileEngine = tileengine.TileEngine()
        self.robots = []

        # NOTE: Because of a bug in SWIG, printing out the wrapped objects results in a crash.
        # So don't do that! I hope this bug in SWIG gets fixed.
        # TODO: Report SWIG bug, if it's not already known or fixed.

        # Hook the engine up so it has a handle on its Python object side.
        self.userData = micropolisengine.getPythonCallbackData(self)
        #print "USERDATA"#, self.userData

        # Hook up the language independent callback mechanism to our low level C++ Python dependent callback handler.
        self.callbackHook = micropolisengine.getPythonCallbackHook()
        #print "CALLBACKHOOK"#, self.callbackHook

        # Hook up the Python side of the callback handler, defined in our scripted subclass of the SWIG wrapper.
        self._invokeCallback = self.invokeCallback # Cache to prevent GC
        self.callbackData = micropolisengine.getPythonCallbackData(self._invokeCallback)
        #print "CALLBACKDATA"#, self.callbackData

        self.dataImageColorMap = \
            cairo.ImageSurface.create_from_png('images/simEngine/transparentYellowOrangeRed.png')

        if self.running:
            self.startTimer()

        print "MicropolisModel.__init__ done", self


    def __del__(
        self):

        self.stopTimer()

        micropolisengine.Micropolis.__del__(self)


    # TODO: Internationalize
    def getMonthName(self, monthIndex):
        return [
            "Jan",
            "Feb",
            "Mar",
            "Apr",
            "May",
            "Jun",
            "Jul",
            "Aug",
            "Sep",
            "Oct",
            "Nov",
            "Dec",
        ][monthIndex];


    def getCityDate(self):
        return (
            self.getMonthName(self.cityMonth) +
            ' ' +
            str(self.cityYear))


    def formatMoney(
        self,
        money):
        return '$' + self.formatNumber(money)


    def formatPercent(
        self,
        percent):
        return str(int(0.5 + (percent * 100))) + "%"


    def formatDelta(
        self,
        delta):
        if delta < 0:
            return self.formatNumber(delta)
        else:
            return '+' + self.formatNumber(delta)


    def formatNumber(
        self,
        number):
        negative = number < 0
        if negative:
            number = -number
        s = str(number)
        if len(s) > 9:
            s = s[-12:-9] + ',' + s[-9:-6] + ',' + s[-6:-3] + ',' + s[-3:]
        elif len(s) > 6:
            s = s[:-6] + ',' + s[-6:-3] + ',' + s[-3:]
        elif len(s) > 3:
            s = s[:-3] + ',' + s[-3:]
        if negative:
            s = '-' + s
        return s


    def addView(self, view):
        self.views.append(view)


    def removeView(self, view):
        self.views.remove(view)


    def getDataImage(self, name):
        # @todo: cache images
        if name == 'power':
            return self.getPowerDataImage()
        elif name == 'traffic':
            return self.getTrafficDataImage()

        return None


    def getTrafficDataImage(self):
        image = self.trafficDataImage
        if not image:
            image = self.makeImage(micropolisengine.WORLD_W_2, micropolisengine.WORLD_H_2)
            self.trafficDataImage = image
        tengine = self.dataTileEngine

        buffer = self.getTrafficDensityMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = micropolisengine.WORLD_W_2
        tengine.height = micropolisengine.WORLD_H_2

        # Unsigned short tile values, in column major order.
        tengine.typeCode = 'b'
        tengine.colBytes = micropolisengine.WORLD_H_2
        tengine.rowBytes = 1

        tengine.renderPixels(
            image,
            self.dataImageColorMap,
            None,
            None,
            0,
            0,
            micropolisengine.WORLD_W_2,
            micropolisengine.WORLD_H_2)

        return image


    def getPowerDataImage(self):
        image = self.powerDataImage
        if not image:
            image = self.makeImage(micropolisengine.WORLD_W, micropolisengine.WORLD_H)
            self.powerDataImage = image
        tengine = self.dataTileEngine

        buffer = self.getPowerMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = micropolisengine.WORLD_W
        tengine.height = micropolisengine.WORLD_H

        # Unsigned short tile values, in column major order.
        tengine.typeCode = 'b'
        tengine.colBytes = micropolisengine.WORLD_H
        tengine.rowBytes = 1
        getTile = self.getTile

        from micropolisengine import ZONEBIT, PWRBIT, ALLBITS

        def tileFunction(row, col, tile):
            if tile > 0:
                tile = 255
            return tile

        tengine.renderPixels(
            image,
            self.dataImageColorMap,
            tileFunction,
            None,
            0,
            0,
            micropolisengine.WORLD_W,
            micropolisengine.WORLD_H)

        return image


    def makeImage(self, width, height):
        return self.dataImageColorMap.create_similar(
            cairo.CONTENT_COLOR_ALPHA, width, height)


    def startTimer(
        self):

        if self.timerActive:
            return

        self.timerId = gobject.timeout_add(self.timeDelay, self.tickTimer)
        self.timerActive = True


    def stopTimer(
        self):

        # FIXME: Is there some way to immediately cancel self.timerId?

        self.timerActive = False


    def tickTimer(
        self):

        if not self.timerActive:
            return False

        self.stopTimer()

        self.tickEngine()

        for view in self.views:
            view.tickActiveTool()

        for view in self.views:
            view.tickTimer()

        if self.running:
            self.startTimer()

        return False


    def tickEngine(self):

        self.simTick()
        if self.doAnimation and not self.tilesAnimated:
            self.animateTiles()


    def sendUpdate(self, name, *args):
        #print "SENDUPDATE", name, args
        interests = self.interests
        if name in interests:
            a = interests[name]
            for view in a:
                view.update(name, *args)


    def expressInterest(self, view, names):
        interests = self.interests
        for name in names:
            if name not in interests:
                a = []
                interests[name] = a
            else:
                a = interests[name]
            if view not in a:
                a.append(view)


    def revokeInterest(self, view, names):
        interests = self.interests
        for name in names:
            if name in interests:
                a = interest[name]
                if view in a:
                    a.remove(view)


    def invokeCallback(self, micropolis, name, *args):
        #print "==== MicropolisDrawingArea invokeCallback", "SELF", sys.getrefcount(self), self, "micropolis", sys.getrefcount(micropolis), micropolis, "name", name
        # In this case, micropolis is the same is self, so ignore it.
        handler = getattr(self, 'handle_' + name, None)
        if handler:
            #print "Calling handler", handler, args
            handler(*args)
            #print "Called handler."
        else:
            print "No handler for", name


    def addRobot(self, robot):
        robots = self.robots
        self.removeRobot(robot)
        robots.append(robot)
        robot.engine = self


    def removeRobot(self, robot):
        robots = self.robots
        if robot in robots:
            robot.engine = None
            robots.remove(robot)


    def simRobots(self):
        for robot in self.robots:
            robot.simulate()


    def __repr__(self):
        return "<MicropolisModel>"


    def handle_UIAutoGoto(self, x, y):
        print "handle_UIAutoGoto(self, x, y)", (self, x, y)


    def handle_UIDidGenerateNewCity(self):
        print "handle_UIDidGenerateNewCity(self)", (self,)


    def handle_UIDidLoadCity(self):
        print "handle_UIDidLoadCity(self)", (self,)


    def handle_UIDidLoadScenario(self):
        print "handle_UIDidLoadScenario(self)", (self,)


    def handle_UIDidSaveCity(self):
        print "handle_UIDidSaveCity(self)", (self,)


    def handle_UIDidTool(self, name, x, y):
        print "handle_UIDidTool(self, name, x, y)", (self, name, x, y)


    def handle_UIDidntLoadCity(self, msg):
        print "handle_UIDidntLoadCity(self, msg)", (self, msg)


    def handle_UIDidntSaveCity(self, msg):
        print "handle_UIDidntSaveCity(self, msg)", (self, msg)


    def handle_UIDoPendTool(self, tool, x, y):
        print "handle_DoPendTool(self, tool, x, y)", (self, tool, x, y)


    def handle_UIDropFireBombs(self):
        print "handle_DropFireBombs(self)", (self,)


    def handle_UIInitializeSound(self):
        print "handle_UIInitializeSound(self)", (self,)


    def handle_UILoseGame(self):
        print "handle_UILoseGame(self)", (self,)


    def handle_UIMakeSound(self, channel, sound, x, y):
        #print "handle_UIMakeSound(self, channel, sound, x, y)", (self, channel, sound, x, y)
        pass # print "SOUND", channel, sound, x, y


    def handle_UINewGame(self):
        print "handle_UINewGame(self)", (self,)


    def handle_UIPlayNewCity(self):
        print "handle_UIPlayNewCity(self)", (self,)


    def handle_UIPopUpMessage(self, msg):
        print "handle_UIPopUpMessage(self, msg)", (self, msg)


    def handle_UIReallyStartGame(self):
        print "handle_UIReallyStartGame(self)", (self,)


    def handle_UISaveCityAs(self):
        print "handle_UISaveCityAs(self)", (self,)


    def handle_UISetBudget(self, flowStr, previousStr, currentStr, collectedStr, tax):
        pass # print "handle_UISetBudget(self, flowStr, previousStr, currentStr, collectedStr, tax)", (self, flowStr, previousStr, currentStr, collectedStr, tax)


    def handle_UISetBudgetValues(self, roadGot, roadWant, roadPercent, policeGot, policeWant, policePercent, fireGot, fireWant, firePercent):
        pass # print "handle_UISetBudgetValues(self, roadGot, roadWant, roadPercent, policeGot, policeWant, policePercent, fireGot, fireWant, firePercent)", (self, roadGot, roadWant, roadPercent, policeGot, policeWant, policePercent, fireGot, fireWant, firePercent)


    def handle_UISetCityName(self, CityName):
        print "handle_UISetCityName(self, CityName)", (self, CityName)


    def handle_UISetDate(self, str, m, y):
        #print "handle_UISetDate(self, str, m, d)", (self, str, m, y)
        pass#print "DATE", str, m, y


    def handle_UISetDemand(self, r, c, i):
        #print "handle_UISetDemand(self, r, c, i)", (self, r, c, i)
        self.r = r
        self.c = c
        self.i = i
        for demand in self.demands:
            demand.update()


    def handle_UISetEvaluation(self, *args):
        #print "handle_UISetEvaluation(self, args)
        self.evaluation = args
        for evaluation in self.evaluations:
            evaluation.update()


    def handle_UISetFunds(self, funds):
        #print "handle_UISetFunds(self, funds)", (self, funds)
        pass # print "FUNDS", funds


    def handle_UISetGameLevel(self, GameLevel):
        print "handle_UISetGameLevel(self, GameLevel)", (self, GameLevel)


    def handle_UISetMapState(self, state):
        print "handle_UISetMapState(self, state)", (self, state)


    def handle_UISetMessage(self, str):
        #print "handle_UISetMessage(self, str)", (self, str)
        print "MESSAGE", str


    def handle_UISetOptions(self, autoBudget, gotoGoto, autoBulldoze, noDisasters, sound, doAnimation, doMessages, doNotices):
        print "handle_UISetOptions(self, autoBudget, gotoGoto, autoBulldoze, noDisasters, sound, doAnimation, doMessages, doNotices)", (self, autoBudget, gotoGoto, autoBulldoze, noDisasters, sound, doAnimation, doMessages, doNotices)

    def handle_UISetSpeed(self, speed):
        print "handle_UISetSpeed(self, speed)", (self, speed)


    def handle_UIShowBudgetAndWait(self):
        print "handle_UIShowBudgetAndWait(self)", (self,)


    def handle_UIShowPicture(self, id):
        #print "handle_UIShowPicture(self, id)", (self, id)
        print "SHOWPICTURE", id


    def handle_UIShowZoneStatus(self, str, s0, s1, s2, s3, s4, x, y):
        print "handle_UIShowZoneStatus(self, str, s0, s1, s2, s3, s4, x, y)", (self, str, s0, s1, s2, s3, s4, x, y)


    def handle_UIStartEarthquake(self, magnitude):
        print "handle_UIStartEarthquake(self, magnitude)", (self, magnitude,)


    def handle_UIStartLoad(self):
        print "handle_UIStartLoad(self)", (self,)


    def handle_UIStartScenario(self, scenario):
        print "handle_UIStartScenario(self, scenario)", (self, scenario)


    def handle_UIWinGame(self):
        print "handle_UIWinGame(self)", (self,)


    def handle_UIUpdate(self, name, *args):
        #print "handle_UIUpdate(self, name, *args)", (self, name, args)

        self.sendUpdate(name, *args)


    def handle_UISimRobots(self):
        #print "handle_UISimRobots(self)", (self,)
        self.simRobots()


########################################################################


def CreateTestEngine():

    # Get our nice scriptable subclass of the SWIG Micropolis wrapper object.
    engine = MicropolisModel()

    engine.resourceDir = 'res'
    engine.initGame()

    return engine


########################################################################
