# micropolisdisasterspanel.py
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
# Micropolis Status View
# Don Hopkins


########################################################################
# Import stuff


import gtk
import cairo
import pango
import micropolisengine
import micropolisview


########################################################################
# MicropolisDisastersPanel


class MicropolisDisastersPanel(gtk.Frame):


    def __init__(
        self,
        engine=None,
        **args):

        gtk.Frame.__init__(
            self,
            **args)

        self.engine = engine

        # Views

        hbox1 = gtk.HBox(False, 0)
        self.hbox1 = hbox1
        self.add(hbox1)

        spacer1 = gtk.VBox()
        spacer1.set_size_request(5, 5)
        hbox1.pack_start(spacer1, False, False, 0)

        vbox1 = gtk.VButtonBox()
        self.vbox1 = vbox1
        hbox1.pack_start(vbox1, False, False, 1)

        spacer2 = gtk.VBox()
        spacer2.set_size_request(5, 5)
        vbox1.pack_start(spacer2, False, False, 0)

        buttonMonster = gtk.Button("Monster")
        self.buttonMonster = buttonMonster
        buttonMonster.connect('clicked', lambda item: engine.makeMonster())
        vbox1.pack_start(buttonMonster, False, False, 1)

        buttonFire = gtk.Button("Fire")
        self.buttonFire = buttonFire
        buttonFire.connect('clicked', lambda item: engine.setFire())
        vbox1.pack_start(buttonFire, False, False, 2)

        buttonFlood = gtk.Button("Flood")
        self.buttonFlood = buttonFlood
        buttonFlood.connect('clicked', lambda item: engine.makeFlood())
        vbox1.pack_start(buttonFlood, False, False, 3)

        spacer3 = gtk.VBox()
        spacer3.set_size_request(5, 5)
        vbox1.pack_start(spacer3, False, False, 4)

        vbox2 = gtk.VButtonBox()
        self.vbox2 = vbox2
        hbox1.pack_start(vbox2, False, False, 2)

        spacer4 = gtk.VBox()
        spacer4.set_size_request(5, 5)
        vbox2.pack_start(spacer4, False, False, 0)

        buttonTornado = gtk.Button("Tornado")
        self.buttonTornado = buttonTornado
        buttonTornado.connect('clicked', lambda item: engine.makeTornado())
        vbox2.pack_start(buttonTornado, False, False, 0)

        buttonMeltdown = gtk.Button("Meltdown")
        self.buttonMeltdown = buttonMeltdown
        buttonMeltdown.connect('clicked', lambda item: engine.makeMeltdown())
        vbox2.pack_start(buttonMeltdown, False, False, 1)

        buttonEarthquake = gtk.Button("Earthquake")
        self.buttonEarthquake = buttonEarthquake
        buttonEarthquake.connect('clicked', lambda item: engine.makeEarthquake())
        vbox2.pack_start(buttonEarthquake, False, False, 2)

        spacer5 = gtk.VBox()
        spacer5.set_size_request(5, 5)
        vbox2.pack_start(spacer5, False, False, 0)


########################################################################
