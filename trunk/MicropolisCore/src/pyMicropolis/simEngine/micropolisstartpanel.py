# micropolisstartpanel.py
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
# Micropolis Start View
# Don Hopkins


########################################################################
# Import stuff


import gtk
import cairo
import pango
import micropolisengine
import micropolisdrawingarea


########################################################################
# MicropolisStartPanel


class MicropolisStartPanel(gtk.Frame):


    scenarios = [
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
        engine=None,
        target=None,
        **args):

        gtk.Frame.__init__(
            self,
            **args)

        self.engine = engine
        self.target = target

        controls = []
        self.controls = controls

        # Views

        hbox1 = gtk.HBox(False, 5)
        self.hbox1 = hbox1
        self.add(hbox1)

        vbox1 = gtk.VBox(False, 5)
        self.vbox1 = vbox1
        hbox1.pack_start(vbox1, True, True, 0)

        hbox2 = gtk.HBox(False, 5)
        self.hbox2 = hbox2
        vbox1.pack_start(hbox2, False, False, 10)

        mediumMapView = micropolisdrawingarea.MediumMicropolisDrawingArea(
                engine=self.engine)
        self.mediumMapView = mediumMapView
        mediumMapView.set_size_request(
            micropolisengine.WORLD_W * 3,
            micropolisengine.WORLD_H * 3)

        hbox2.pack_start(mediumMapView, False, False, 5)

        vbox2 = gtk.VBox(False, 5)
        self.vbox2 = vbox2
        hbox2.pack_start(vbox2, True, True, 0)

        markup = (
            '<b><span size="xx-large">Micropolis</span></b>'
        )

        labelMicropolis = gtk.Label()
        self.labelMicropolis = labelMicropolis
        labelMicropolis.set_markup(markup)
        vbox2.pack_start(labelMicropolis, False, False, 5)

        markup = (
            '<b><span size="x-large">'
            'Pick a City'
            '</span></b>'
        )

        labelTitle = gtk.Label()
        self.labelTitle = labelTitle
        labelTitle.set_markup(markup)
        vbox2.pack_start(labelTitle, False, False, 5)


        markup = (
            '<span size="large">'
            'You may pick a scenario, generate a random city, or load a city from disk.\n\n'
            'After you have picked a city, press the button at the bottom to play.\n'
            '</span>'
        )

        labelDescription = gtk.Label()
        self.labelDescription = labelDescription
        labelDescription.set_line_wrap(True)
        labelDescription.set_markup(markup)
        vbox2.pack_start(labelDescription, True, True, 5)

        vbox3 = gtk.VBox(False, 0)
        self.vbox3 = vbox3
       
        vbox1.pack_start(vbox3, False, False, 5)

        scenariosPerGroup = 2
        scenarioInGroup = scenariosPerGroup

        scenarioGroup = None

        hbox2 = gtk.HBox(False, 5)
        self.hbox2 = hbox2

        vbox1.pack_start(hbox2, False, False, 10)

        for scenario in self.scenarios:

            if scenarioInGroup == scenariosPerGroup:
                scenarioInGroup = 0
                scenarioGroup = gtk.VBox(False, 5)
                hbox2.pack_start(scenarioGroup, True, True, 0)
            scenarioInGroup += 1

            vbox = gtk.VBox(False, 5)
            controls.append(vbox)
            scenarioGroup.pack_start(vbox, False, False, 0)

            button = gtk.Button(scenario['title'])
            controls.append(button)
            button.scenario = scenario
            button.connect('clicked', lambda item: self.startScenario(item.scenario))
            vbox.pack_start(button, False, False, 0)

            image = gtk.Image()
            image.set_from_file(
                'images/simEngine/scenario' +
                str(scenario['id']) +
                'Icon.png')
            vbox.pack_start(image, False, False, 0)

        markup = (
            '<span size="large">'
            '\nYou can also load a city from disk:'
            '</span>'
        )

        labelDescription1 = gtk.Label()
        self.labelDescription1 = labelDescription1
        labelDescription1.set_markup(markup)
        vbox1.pack_start(labelDescription1, False, False, 0)

        button = gtk.Button('Load City')
        controls.append(button)
        button.connect('clicked', lambda item: target.loadCityDialog())
        vbox1.pack_start(button, False, False, 0)

        mediumMapView = micropolisdrawingarea.MediumMicropolisDrawingArea(
                engine=self.engine)
        self.mediumMapView = mediumMapView
        vbox1.pack_start(mediumMapView, False, False, 0)

        markup = (
            '<span size="large">'
            '\nYou can also randomly generate a new city:'
            '</span>'
        )

        labelDescription2 = gtk.Label()
        self.labelDescription2 = labelDescription2
        labelDescription2.set_markup(markup)
        vbox1.pack_start(labelDescription2, False, False, 0)

        button = gtk.Button('Generate City')
        controls.append(button)
        button.connect('clicked', lambda item: target.generateCity())
        vbox1.pack_start(button, False, False, 0)

        # Make the medium map view.

        mediumMapView = micropolisdrawingarea.MediumMicropolisDrawingArea(
                engine=self.engine)
        self.mediumMapView = mediumMapView
        vbox1.pack_start(mediumMapView, False, False, 0)

        markup = (
            '<span size="large">'
            '\nAfter selecting a city, press this button to play:'
            '</span>'
        )

        labelDescription3 = gtk.Label()
        self.labelDescription3 = labelDescription3
        labelDescription3.set_markup(markup)
        vbox1.pack_start(labelDescription3, False, False, 0)

        button = gtk.Button('Play with This City')
        controls.append(button)
        button.connect('clicked', lambda item: target.playCity())
        vbox1.pack_start(button, False, False, 0)


    def startScenario(self, scenario):
        print "STARTSCENARIO", scenario
        title = scenario['title']
        description = scenario['description']
        print title
        print description

        titleMarkup = (
            '<b><span size="xx-large">' +
            title +
            '</span></b>'
        )

        descriptionMarkup = (
            '<span size="large">' +
            description +
            '</span>'
        )

        self.labelTitle.set_markup(titleMarkup)
        self.labelDescription.set_markup(descriptionMarkup)
        self.target.startScenario(scenario['id'])


########################################################################
