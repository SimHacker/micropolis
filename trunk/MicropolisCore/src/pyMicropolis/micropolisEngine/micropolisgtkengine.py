# micropolisgtkengine.py
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
# Micropolis GTK Engine
# Don Hopkins


########################################################################
# Import stuff


import micropolisgenericengine
import micropolisengine
import gobject


########################################################################
# MicropolisGTKEngine Class


class MicropolisGTKEngine(micropolisgenericengine.MicropolisGenericEngine):


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


    def __init__(self, *args, **kw):

        micropolisgenericengine.MicropolisGenericEngine.__init__(self, *args, **kw)

        # This must be called at the end of the concrete subclass's
        # init, so it happens last.
        self.initGamePython()


    def __repr__(self):
        return "<MicropolisGTKEngine>"


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

        if self.running:
            self.startTimer()

        return False


    def tickEngine(self):

        self.simTick()
        if self.doAnimation and not self.tilesAnimated:
            self.animateTiles()

        self.simUpdate()

        self.sendUpdate('tick')
        self.sendUpdate('editor')
        self.sendUpdate('map')


    def handle_didGenerateMap(self):
        print "handle_didGenerateMap(self)", (self,)
        self.sendUpdate('tick')
        self.sendUpdate('editor')
        self.sendUpdate('map')


    def handle_didLoadCity(self):
        print "handle_didLoadCity(self)", (self,)
        self.sendUpdate('tick')
        self.sendUpdate('editor')
        self.sendUpdate('map')


    def handle_didLoadScenario(self):
        print "handle_didLoadScenario(self)", (self,)
        self.sendUpdate('tick')
        self.sendUpdate('editor')
        self.sendUpdate('map')


    def handle_simulateChurch(self, x, y, tile):
        pass


########################################################################


def CreateGTKEngine(**params):

    # Get our nice scriptable subclass of the SWIG Micropolis wrapper object.
    engine = MicropolisGTKEngine(**params)

    return engine


########################################################################
