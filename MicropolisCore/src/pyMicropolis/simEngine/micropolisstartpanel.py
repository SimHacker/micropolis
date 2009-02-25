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
# Micropolis Start Panel
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

        engine.expressInterest(
            self,
            ('load', 'gamemode',))

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

        previewMapView = micropolisdrawingarea.PreviewMicropolisDrawingArea(
                engine=engine)
        self.previewMapView = previewMapView
        previewMapView.set_size_request(
            micropolisengine.WORLD_W * 3,
            micropolisengine.WORLD_H * 3)

        hbox2.pack_start(previewMapView, False, False, 5)

        vbox2 = gtk.VBox(False, 5)
        self.vbox2 = vbox2
        hbox2.pack_start(vbox2, True, True, 0)

        markup = (
            '<b><span size="xx-large">Micropolis City Simulator</span></b>'
        )

        labelMicropolis = gtk.Label()
        self.labelMicropolis = labelMicropolis
        labelMicropolis.set_markup(markup)
        vbox2.pack_start(labelMicropolis, False, False, 5)

        labelTitle = gtk.Label()
        self.labelTitle = labelTitle
        vbox2.pack_start(labelTitle, False, False, 5)

        labelDescription = gtk.Label()
        self.labelDescription = labelDescription
        labelDescription.set_line_wrap(True)
        vbox2.pack_start(labelDescription, True, True, 5)

        button = gtk.Button('Play with This City')
        controls.append(button)
        button.connect('clicked', lambda item: target.playCity())
        vbox2.pack_start(button, False, False, 0)

        hbox3 = gtk.HBox(False, 5)
        self.hbox3 = hbox3
        vbox2.pack_start(hbox3, False, False, 0)

        button = gtk.Button('Generate City')
        controls.append(button)
        button.connect('clicked', lambda item: target.generateCity())
        hbox3.pack_start(button, True, True, 0)

        button = gtk.Button('Load City')
        controls.append(button)
        button.connect('clicked', lambda item: target.loadCityDialog())
        hbox3.pack_start(button, True, True, 0)

        scenariosPerGroup = 2
        scenarioInGroup = scenariosPerGroup

        scenarioGroup = None

        hbox2 = gtk.HBox(False, 5)
        self.hbox2 = hbox2

        vbox1.pack_start(hbox2, False, False, 5)

        for scenario in engine.scenarios[1:]: # Skip SC_NONE.

            scenarioID = scenario['id']
            scenarioTitle = scenario['title']

            if scenarioInGroup == scenariosPerGroup:
                scenarioInGroup = 0
                scenarioGroup = gtk.VBox(False, 5)
                hbox2.pack_start(scenarioGroup, True, True, 0)

            scenarioInGroup += 1

            vbox = gtk.VBox(False, 5)
            controls.append(vbox)
            scenarioGroup.pack_start(vbox, False, False, 0)

            button = gtk.Button(scenarioTitle)
            button.scenarioID = scenarioID
            controls.append(button)
            button.connect('clicked', lambda item: self.startScenario(item.scenarioID))
            vbox.pack_start(button, False, False, 0)

            image = gtk.Image()
            image.set_from_file(
                'images/simEngine/scenario' +
                str(scenario['id']) +
                'Icon.png')
            vbox.pack_start(image, False, False, 0)


    def update(self, name, *args):

        engine = self.engine

        if name == 'load':

            title = engine.title
            description = engine.description
            
            titleMarkup = (
                '<b><span size="x-large">' +
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

        elif name == 'gamemode':
            
            gameMode = engine.gameMode
            previewMapView = self.previewMapView

            if gameMode == 'start':

                previewMapView.engage()
                previewMapView.updateView()

            elif gameMode == 'play':

                previewMapView.disengage()


    def startScenario(self, scenarioID):
        print "STARTSCENARIO", scenarioID

        self.target.startScenario(scenarioID)


########################################################################
