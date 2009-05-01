# micropolisgenericengine.py
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
# Micropolis Generic Engine
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
from micropolisengine import WORLD_W, WORLD_H
from micropolisengine import WORLD_W_2, WORLD_H_2
from micropolisengine import WORLD_W_4, WORLD_H_4
from micropolisengine import WORLD_W_8, WORLD_H_8
import xml.dom.minidom
from xml.dom.minidom import Node
import pprint
from StringIO import StringIO


########################################################################
# Globals


__version__ = "0.9"


########################################################################
# Utilities


def GetElementText(el, default=u''):
    if not el:
        return default
    text = default
    foundText = False
    el = el.firstChild
    while el:
        if el.nodeType == el.TEXT_NODE:
            value = el.nodeValue
            if not foundText:
                # Replace the default with the first text.
                foundText = True
                text = value
            else:
                text += value
        el = el.nextSibling
    return text


def GetElementBool(el, default=False):
    if not el:
        return default
    text = GetElementText(el).lower()
    if text == 'true':
        return True
    if text == 'false':
        return False
    return default


def GetElementInt(el, default=0):
    if not el:
        return default
    try:
        val = int(GetElementText(el))
    except:
        val = default
    return val


def GetElementFloat(el, default=0.0):
    if not el:
        return default
    try:
        val = float(GetElementText(el))
    except:
        val = default
    return val


def GetSubElement(el, key):
    # Isn't there a faster way to do this? (Only searching direct children.)
    el = el.firstChild
    while el:
        if ((el.nodeType == el.ELEMENT_NODE) and
            (el.nodeName == key)):
            return el
        el = el.nextSibling
    return None


def GetSubElementText(el, key, default=u''):
    return GetElementText(GetSubElement(el, key), default)


def GetSubElementBool(el, key, default=False):
    return GetElementBool(GetSubElement(el, key), default)


def GetSubElementInt(el, key, default=0):
    return GetElementInt(GetSubElement(el, key), default)


def GetSubElementFloat(el, key, default=0.0):
    return GetElementFloat(GetSubElement(el, key), default)


def SetSubElementText(el, key, value):
    subEl = el.createElement(key)
    text = el.createTextNode(value)
    subEl.appendChild(text)
    el.appendChild(subEl)


def SetSubElementBool(el, key, value):
    if value:
        value = 'true'
    else:
        value = 'false'
    SetSubElementText(el, key, value)


def SetSubElementInt(el, key, value):
    SetSubElementText(el, key, str(value))


def SetSubElementFloat(el, key, value):
    SetSubElementText(el, key, str(value))


########################################################################
# MicropolisGenericEngine Class


class MicropolisGenericEngine(micropolisengine.Micropolis):


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


    scenarios = [
        None, # SC_NONE
        {
            'id': micropolisengine.SC_DULLSVILLE,
            'title': 'Dullsville, USA  1900',
            'description': """Things haven't changed much around here in the last hundred years or so and the residents are beginning to get bored. They think Dullsville could be the next great city with the right leader.

It is your job to attract new growth and development, turning Dullsville into a Metropolis within 30 years.""",
        },
        {
            'id': micropolisengine.SC_SAN_FRANCISCO,
            'title': 'San Francisco, CA.  1906',
            'description': """Damage from the earthquake was minor compared to that of the ensuing fires, which took days to control. 1500 people died.

Controlling the fires should be your initial concern. Then clear the rubble and start rebuilding. You have 5 years.""",
        },
        {
            'id': micropolisengine.SC_HAMBURG,
            'title': 'Hamburg, Germany  1944',
            'description': """Allied fire-bombing of German cities in WWII caused tremendous damage and loss of life. People living in the inner cities were at greatest risk.

You must control the firestorms during the bombing and then rebuild the city after the war. You have 5 years.""",
        },
        {
            'id': micropolisengine.SC_BERN,
            'title': 'Bern, Switzerland  1965',
            'description': """The roads here are becoming more congested every day, and the residents are upset. They demand that you do something about it.

Some have suggested a mass transit system as the answer, but this would require major rezoning in the downtown area. You have 10 years.""",
        },
        {
            'id': micropolisengine.SC_TOKYO,
            'title': 'Tokyo, Japan  1957',
            'description': """A large reptilian creature has been spotted heading for Tokyo bay. It seems to be attracted to the heavy levels of industrial pollution there.

Try to control the fires, then rebuild the industrial center. You have 5 years.""",
        },
        {
            'id': micropolisengine.SC_DETROIT,
            'title': 'Detroit, MI.  1972',
            'description': """By 1970, competition from overseas and other economic factors pushed the once "automobile capital of the world" into recession. Plummeting land values and unemployment then increased crime in the inner-city to chronic levels.

You have 10 years to reduce crime and rebuild the industrial base of the city."""
        },
        {
            'id': micropolisengine.SC_BOSTON,
            'title': 'Boston, MA.  2010',
            'description': """A major meltdown is about to occur at one of the new downtown nuclear reactors. The area in the vicinity of the reactor will be severly contaminated by radiation, forcing you to restructure the city around it.

You have 5 years to get the situation under control.""",
        },
        {
            'id': micropolisengine.SC_RIO,
            'title': 'Rio de Janeiro, Brazil  2047',
            'description': """In the mid-21st century, the greenhouse effect raised global temperatures 6 degrees F. Polar icecaps melted and raised sea levels worldwide. Coastal areas were devastated by flood and erosion.

You have 10 years to turn this swamp back into a city again.""",
        },
    ]


    def __init__(
            self,
            running=False,
            timeDelay=10,
            *args,
            **kwargs):
        #print "MicropolisGenericEngine.__init__", self, "calling micropolisengine.Micropolis.__init__", micropolisengine.Micropolis.__init__, args, kwargs

        micropolisengine.Micropolis.__init__(self, *args, **kwargs)

        self.resourceDir = 'res'
        self.running = running
        self.timeDelay = timeDelay
        self.timerActive = False
        self.timerId = None
        self.views = []
        self.interests = {}
        self.residentialImage = None
        self.commercialImage = None
        self.industrialImage = None
        self.transportationImage = None
        self.populationDensityImage = None
        self.rateOfGrowthImage = None
        self.landValueImage = None
        self.crimeRateImage = None
        self.pollutionDensityImage = None
        self.trafficDensityImage = None
        self.powerGridImage = None
        self.fireCoverageImage = None
        self.policeCoverageImage = None
        self.dataTileEngine = tileengine.TileEngine()
        self.robots = []
        self.saveFileDir = None
        self.saveFileName = None
        self.metaFileName = None
        self.title = ''
        self.description = ''
        self.readOnly = False
        self.gameMode = 'stopped'


        # NOTE: Because of a bug in SWIG, printing out the wrapped
        # objects results in a crash.  So don't do that! I hope this
        # bug in SWIG gets fixed.
        # TODO: Report SWIG bug, if it's not already known or fixed.

        # Hook the engine up so it has a handle on its Python object side.
        self.userData = micropolisengine.getPythonCallbackData(self)
        #print "USERDATA"#, self.userData

        # Hook up the language independent callback mechanism to our
        # low level C++ Python dependent callback handler.

        self.callbackHook = micropolisengine.getPythonCallbackHook()
        #print "CALLBACKHOOK"#, self.callbackHook

        # Hook up the Python side of the callback handler, defined in
        # our scripted subclass of the SWIG wrapper.

        self._invokeCallback = self.invokeCallback # Cache to prevent GC
        self.callbackData = \
            micropolisengine.getPythonCallbackData(
                self._invokeCallback)
        #print "CALLBACKDATA"#, self.callbackData

        self.dataColorMap = \
            cairo.ImageSurface.create_from_png(
                'images/simEngine/dataColorMap.png')

        self.rateColorMap = \
            cairo.ImageSurface.create_from_png(
                'images/simEngine/rateColorMap.png')

        self.powerGridColorMap = \
            cairo.ImageSurface.create_from_png(
                'images/simEngine/powerGridColorMap.png')

        self.terrainColorMap = \
            cairo.ImageSurface.create_from_png(
                'images/simEngine/terrainColorMap.png')

        self.expressInterest(
            self,
            ('paused',))

        self.initGame()
        self.initGamePython()

        if self.running:
            self.startTimer()

        #print "MicropolisEngine.__init__ done", self


    def initGamePython(self):
        pass


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
        views = self.views
        if view not in views:
            self.views.append(view)
            print "ADDVIEW", view


    def removeView(self, view):
        views = self.views
        if view in views:
            views.remove(view)
            print "REMOVEVIEW", view


    def getDataImageAlphaSize(self, name):
        # @todo: cache images
        if name == 'all':
            return self.getAllImageAlphaSize()
        elif name == 'residential':
            return self.getResidentialImageAlphaSize()
        elif name == 'commercial':
            return self.getCommercialImageAlphaSize()
        elif name == 'industrial':
            return self.getIndustrialImageAlphaSize()
        elif name == 'transportation':
            return self.getTransportationImageAlphaSize()
        elif name == 'populationdensity':
            return self.getPopulationDensityImageAlphaSize()
        elif name == 'rateofgrowth':
            return self.getRateOfGrowthImageAlphaSize()
        elif name == 'landvalue':
            return self.getLandValueImageAlphaSize()
        elif name == 'crimerate':
            return self.getCrimeRateImageAlphaSize()
        elif name == 'pollutiondensity':
            return self.getPollutionDensityImageAlphaSize()
        elif name == 'trafficdensity':
            return self.getTrafficDensityImageAlphaSize()
        elif name == 'powergrid':
            return self.getPowerGridImageAlphaSize()
        elif name == 'firecoverage':
            return self.getFireCoverageImageAlphaSize()
        elif name == 'policecoverage':
            return self.getPoliceCoverageImageAlphaSize()
        else:
            print "MicropolisGenericEngine: getImageAlphaSize: Invalid data image name:", name
            return None, 0.0, 0.0, 0.0


    def getAllImageAlphaSize(self):
        return None, 1.0, 1.0, 1.0


    def getResidentialImageAlphaSize(self):
        image = self.residentialImage
        if not image:
            image = self.makeImage(WORLD_W, WORLD_H)
            self.residentialImage = image
        tengine = self.dataTileEngine

        buffer = self.getMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W
        tengine.height = WORLD_H

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = micropolisengine.BYTES_PER_TILE * micropolisengine.WORLD_H
        tengine.rowBytes = micropolisengine.BYTES_PER_TILE

        from micropolisengine import \
            ZONEBIT, PWRBIT, CONDBIT, ALLBITS, LOMASK, RIVER, \
            COMBASE

        transparent = 0
        land = 1
        water = 2

        def tileFunction(col, row, tile):
            tileMasked = tile & LOMASK
            if tileMasked == RIVER:
                result = water
            elif tileMasked >= COMBASE:
                result = land
            else:
                result = transparent
            return result

        tengine.renderPixels(
            image,
            self.terrainColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W,
            WORLD_H)

        return image, 0.8, 1.0, 1.0


    def getCommercialImageAlphaSize(self):
        image = self.commercialImage
        if not image:
            image = self.makeImage(WORLD_W, WORLD_H)
            self.commercialImage = image
        tengine = self.dataTileEngine

        buffer = self.getMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W
        tengine.height = WORLD_H

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = micropolisengine.BYTES_PER_TILE * micropolisengine.WORLD_H
        tengine.rowBytes = micropolisengine.BYTES_PER_TILE

        from micropolisengine import \
            ZONEBIT, PWRBIT, CONDBIT, ALLBITS, LOMASK, RIVER, \
            COMBASE, COMLAST, LVRAIL6

        transparent = 0
        land = 1
        water = 2

        def tileFunction(col, row, tile):
            tileMasked = tile & LOMASK
            if tileMasked == RIVER:
                result = water
            elif ((tileMasked > COMLAST) or
                  ((tileMasked >= LVRAIL6) and
                   (tileMasked < COMBASE))):
                result = land
            else:
                result = transparent
            return result

        tengine.renderPixels(
            image,
            self.terrainColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W,
            WORLD_H)

        return image, 0.8, 1.0, 1.0


    def getIndustrialImageAlphaSize(self):
        image = self.industrialImage
        if not image:
            image = self.makeImage(WORLD_W, WORLD_H)
            self.industrialImage = image
        tengine = self.dataTileEngine

        buffer = self.getMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W
        tengine.height = WORLD_H

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = micropolisengine.BYTES_PER_TILE * micropolisengine.WORLD_H
        tengine.rowBytes = micropolisengine.BYTES_PER_TILE

        from micropolisengine import \
            ZONEBIT, PWRBIT, CONDBIT, ALLBITS, LOMASK, RIVER, \
            RESBASE, INDBASE, PORTBASE, SMOKEBASE, TINYEXP, \
            TINYEXPLAST, FOOTBALLGAME1

        transparent = 0
        land = 1
        water = 2

        def tileFunction(col, row, tile):
            tileMasked = tile & LOMASK
            if tileMasked == RIVER:
                result = water
            elif (((tileMasked >= RESBASE) and (tileMasked < INDBASE)) or
                  ((tileMasked >= PORTBASE) and (tileMasked < SMOKEBASE)) or
                  ((tileMasked >= TINYEXP) and (tileMasked <= TINYEXPLAST)) or
                  (tileMasked >= FOOTBALLGAME1)):
                result = land
            else:
                result = transparent
            return result

        tengine.renderPixels(
            image,
            self.terrainColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W,
            WORLD_H)

        return image, 0.8, 1.0, 1.0


    def getTransportationImageAlphaSize(self):
        image = self.commercialImage
        if not image:
            image = self.makeImage(WORLD_W, WORLD_H)
            self.commercialImage = image
        tengine = self.dataTileEngine

        buffer = self.getMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W
        tengine.height = WORLD_H

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = micropolisengine.BYTES_PER_TILE * micropolisengine.WORLD_H
        tengine.rowBytes = micropolisengine.BYTES_PER_TILE

        from micropolisengine import \
            ZONEBIT, PWRBIT, CONDBIT, ALLBITS, LOMASK, RIVER, \
            RESBASE, BRWXXX7, LVPOWER10, UNUSED_TRASH6

        transparent = 0
        land = 1
        water = 2

        # FIXME: Exclude the smokestack animations used by the coal power plant.

        def tileFunction(col, row, tile):
            tileMasked = tile & LOMASK
            if tileMasked == RIVER:
                result = water
            elif ((tileMasked >= RESBASE) or
                  ((tileMasked >= BRWXXX7) and (tileMasked <= LVPOWER10)) or
                  (tileMasked == UNUSED_TRASH6)):
                result = land
            else:
                result = transparent
            return result

        tengine.renderPixels(
            image,
            self.terrainColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W,
            WORLD_H)

        return image, 0.8, 1.0, 1.0


    def getPopulationDensityImageAlphaSize(self):
        image = self.populationDensityImage
        if not image:
            image = self.makeImage(WORLD_W_2, WORLD_H_2)
            self.populationDensityImage = image
        tengine = self.dataTileEngine

        buffer = self.getPopulationDensityMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_2
        tengine.height = WORLD_H_2

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_BYTE_UNSIGNED
        tengine.colBytes = WORLD_H_2
        tengine.rowBytes = 1

        tengine.renderPixels(
            image,
            self.dataColorMap,
            None,
            None,
            0,
            0,
            WORLD_W_2,
            WORLD_H_2)

        return image, 0.5, 1.0, 1.0


    def getRateOfGrowthImageAlphaSize(self):
        image = self.rateOfGrowthImage
        if not image:
            image = self.makeImage(WORLD_W_8, WORLD_H_8)
            self.rateOfGrowthImage = image
        tengine = self.dataTileEngine

        buffer = self.getRateOfGrowthMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_8
        tengine.height = WORLD_H_8

        # Signed short tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_SIGNED
        tengine.colBytes = WORLD_H_8 * 2
        tengine.rowBytes = 2

        def tileFunction(col, row, tile):
            return max(0, min(int(((tile * 256) / 200) + 128), 255))

        tengine.renderPixels(
            image,
            self.rateColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W_8,
            WORLD_H_8)

        ratio = (float(WORLD_H) / 8.0) / float(WORLD_H_8)
        return image, 0.5, 1.0, ratio


    def getLandValueImageAlphaSize(self):
        image = self.landValueImage
        if not image:
            image = self.makeImage(WORLD_W_2, WORLD_H_2)
            self.landValueImage = image
        tengine = self.dataTileEngine

        buffer = self.getLandValueMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_2
        tengine.height = WORLD_H_2

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_BYTE_UNSIGNED
        tengine.colBytes = WORLD_H_2
        tengine.rowBytes = 1

        tengine.renderPixels(
            image,
            self.dataColorMap,
            None,
            None,
            0,
            0,
            WORLD_W_2,
            WORLD_H_2)

        return image, 0.5, 1.0, 1.0


    def getCrimeRateImageAlphaSize(self):
        image = self.crimeRateImage
        if not image:
            image = self.makeImage(WORLD_W_2, WORLD_H_2)
            self.crimeRateImage = image
        tengine = self.dataTileEngine

        buffer = self.getCrimeRateMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_2
        tengine.height = WORLD_H_2

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_BYTE_UNSIGNED
        tengine.colBytes = WORLD_H_2
        tengine.rowBytes = 1

        tengine.renderPixels(
            image,
            self.dataColorMap,
            None,
            None,
            0,
            0,
            WORLD_W_2,
            WORLD_H_2)

        return image, 0.5, 1.0, 1.0


    def getPollutionDensityImageAlphaSize(self):
        image = self.pollutionDensityImage
        if not image:
            image = self.makeImage(WORLD_W_2, WORLD_H_2)
            self.pollutionDensityImage = image
        tengine = self.dataTileEngine

        buffer = self.getPollutionDensityMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_2
        tengine.height = WORLD_H_2

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_BYTE_UNSIGNED
        tengine.colBytes = WORLD_H_2
        tengine.rowBytes = 1

        tengine.renderPixels(
            image,
            self.dataColorMap,
            None,
            None,
            0,
            0,
            WORLD_W_2,
            WORLD_H_2)

        return image, 0.5, 1.0, 1.0


    def getTrafficDensityImageAlphaSize(self):
        image = self.trafficDensityImage
        if not image:
            image = self.makeImage(WORLD_W_2, WORLD_H_2)
            self.trafficDensityImage = image
        tengine = self.dataTileEngine

        buffer = self.getTrafficDensityMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_2
        tengine.height = WORLD_H_2

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_BYTE_UNSIGNED
        tengine.colBytes = WORLD_H_2
        tengine.rowBytes = 1

        tengine.renderPixels(
            image,
            self.dataColorMap,
            None,
            None,
            0,
            0,
            WORLD_W_2,
            WORLD_H_2)

        return image, 0.5, 1.0, 1.0


    def getPowerGridImageAlphaSize(self):
        image = self.powerGridImage
        if not image:
            image = self.makeImage(WORLD_W, WORLD_H)
            self.powerGridImage = image
        tengine = self.dataTileEngine

        buffer = self.getPowerGridMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W
        tengine.height = WORLD_H

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_BYTE_UNSIGNED
        tengine.colBytes = WORLD_H
        tengine.rowBytes = 1

        from micropolisengine import \
            ZONEBIT, PWRBIT, CONDBIT, ALLBITS, LOMASK, LASTFIRE

        transparent = 0
        unpowered = 1
        powered = 2
        conductive = 3
        getTile = self.getTile

        def tileFunction(col, row, tile):
            t = getTile(col, row)
            tileMasked = t & LOMASK
            if tileMasked < LASTFIRE:
                result = transparent
            elif t & ZONEBIT:
                if t & PWRBIT:
                    result = powered
                else:
                    result = unpowered
            else:
                if t & CONDBIT:
                    result = conductive
                else:
                    result = transparent
            return result

        tengine.renderPixels(
            image,
            self.powerGridColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W,
            WORLD_H)

        return image, 0.85, 1.0, 1.0


    def getFireCoverageImageAlphaSize(self):
        image = self.fireCoverageImage
        if not image:
            image = self.makeImage(WORLD_W_8, WORLD_H_8)
            self.fireCoverageImage = image
        tengine = self.dataTileEngine

        buffer = self.getFireCoverageMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_8
        tengine.height = WORLD_H_8

        # Unsigned short tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = WORLD_H_8 * 2
        tengine.rowBytes = 2

        def tileFunction(col, row, tile):
            return max(0, min(tile, 255))

        tengine.renderPixels(
            image,
            self.dataColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W_8,
            WORLD_H_8)

        ratio = (float(WORLD_H) / 8.0) / float(WORLD_H_8)
        return image, 0.5, 1.0, ratio


    def getPoliceCoverageImageAlphaSize(self):
        image = self.policeCoverageImage
        if not image:
            image = self.makeImage(WORLD_W_8, WORLD_H_8)
            self.policeCoverageImage = image
        tengine = self.dataTileEngine

        buffer = self.getPoliceCoverageMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_8
        tengine.height = WORLD_H_8

        # Unsigned short tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = WORLD_H_8 * 2
        tengine.rowBytes = 2

        def tileFunction(col, row, tile):
            return max(0, min(tile, 255))

        tengine.renderPixels(
            image,
            self.dataColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W_8,
            WORLD_H_8)

        ratio = (float(WORLD_H) / 8.0) / float(WORLD_H_8)
        return image, 0.5, 1.0, ratio


    def makeImage(self, width, height):
        return self.dataColorMap.create_similar(
            cairo.CONTENT_COLOR_ALPHA, width, height)


    def startTimer(
        self):

        pass # Override in subclasses.


    def stopTimer(
        self):

        pass # Override in subclasses.


    def update(
        self,
        name,
        *args):

        #print "MicropolisEngine update", self, name, args

        if name == 'paused':
            self.running = not self.simPaused
            print "PAUSED", self.simPaused, "running", self.running
            if self.running:
                self.startTimer()
            else:
                self.stopTimer()


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


    def loadMetaCity(self, metaFilePath):

        doc = xml.dom.minidom.parse(metaFilePath)
        el = doc.firstChild
        if el.nodeName != 'metaCity':
            raise Exception("Expected top level 'metaCity' element in meta city file")

        saveFileDir, metaFileName = os.path.split(os.path.abspath(metaFilePath))
        saveFileName = GetSubElementText(el, u'saveFileName', None)
        title = GetSubElementText(el, u'title', '')
        description = GetSubElementText(el, u'description', '')
        readOnly = GetSubElementBool(el, u'readOnly', False)

        self.saveFileDir = saveFileDir
        self.metaFileName = metaFileName
        self.saveFileName = saveFileName
        self.title = title
        self.description = description
        self.readOnly = readOnly

        saveFilePath = os.path.join(saveFileDir, saveFileName)

        saveFilePath = saveFilePath.encode('utf8')
        print "Loading city file:", saveFilePath
        success = self.loadCity(saveFilePath)

        if not success:
            raise Exception('Error loading city file')

        self.sendUpdate('load')


    def saveMetaCity(self, metaFileName=None):

        saveFileDir = self.saveFileDir
        metaFileName = metaFileName or self.metaFileName
        saveFileName = self.saveFileName
        readOnly = self.readOnly

        if not metaFileName:
            raise Exception('Undefined metaFileName')

        if readOnly or not saveFileDir:
            readOnly = False
            self.readOnly = readOnly
            saveFileDir = os.path.expanduser('~/cities')
            self.saveFileDir = saveFileDir
            os.makedirs(saveFileDir)

        baseName, ext = os.path.splitext(metaFileName)

        saveFileName = baseName + '.cty'
        self.saveFileName = saveFileName

        success = self.saveCityAs(saveFilePath)

        if not success:
            raise('Error writing to city file')

        doc = xml.dom.minidom.Document()
        el = doc.createElement(u'metaCity')
        
        SetSubElementText(el, u'saveFileName', saveFileName)
        SetSubElementText(el, u'title', self.title)
        SetSubElementText(el, u'description', self.description)
        SetSubElementText(el, u'readOnly', readOnly)

        metaFilePath = os.path.join(saveFileDir, metaFileName)
        f = open(metaFilePath, 'wb')
        xmlText = doc.toxml()
        print xmlText
        f.write(xmlText)
        f.close()


    def loadMetaScenario(self, id):
        if ((id <= micropolisengine.SC_NONE) or
            (id >= micropolisengine.SC_COUNT)):
            print "loadMetaScenario: Invalid scenario id:", id
            return

        scenario = self.scenarios[id]
        self.title = scenario['title']
        self.description = scenario['description']
        self.loadScenario(id)
        self.sendUpdate('load')

        self.sendUpdate('message', id, -1, -1, True, True)

    def generateNewMetaCity(self):
        self.title = 'New City'
        self.description = 'A randomly generated city.'
        self.generateNewCity()
        self.sendUpdate('load')


    def setGameMode(self, gameMode):
        self.gameMode = gameMode
        self.sendUpdate('gamemode')


    def invokeCallback(self, micropolis, name, *args):
        #print "==== MicropolisDrawingArea invokeCallback", "SELF", sys.getrefcount(self), self, "micropolis", sys.getrefcount(micropolis), micropolis, "name", name
        # In this case, micropolis is the same as self, so ignore it.
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
        return "<MicropolisEngine>"


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


    def handle_UIDropFireBombs(self):
        print "handle_DropFireBombs(self)", (self,)
        self.fireBomb()


    def handle_UILoseGame(self):
        print "handle_UILoseGame(self)", (self,)


    def handle_UIMakeSound(self, channel, sound, x, y):
        #print "handle_UIMakeSound(self, channel, sound, x, y)", (self, channel, sound, x, y)
        pass # print "SOUND", channel, sound, x, y


    def handle_UINewGame(self):
        print "handle_UINewGame(self)", (self,)


    def handle_UIPlayNewCity(self):
        print "handle_UIPlayNewCity(self)", (self,)


    def handle_UIReallyStartGame(self):
        print "handle_UIReallyStartGame(self)", (self,)


    def handle_UISaveCityAs(self):
        print "handle_UISaveCityAs(self)", (self,)


    def handle_UIShowBudgetAndWait(self):
        print "handle_UIShowBudgetAndWait(self)", (self,)
        # @todo Show budget window. Actually pause the engine here. Maybe start a timeout to un-pause it.


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


    def handle_UIInvalidateEditors(self):
        pass


    def handle_UIInvalidateMaps(self):
        pass


########################################################################
