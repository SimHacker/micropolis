# micropoliscontrolpanel.py
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
# MicropolisControlPanel


class MicropolisControlPanel(gtk.Frame):


    speedElbow = 50
    speedSlope = 20
    speeds = [
        # Speed, Passes, Label
        (0, 0, 'Paused',),
        (1, 1, 'Ultra Slow',),
        (2, 1, 'Very Slow',),
        (3, 1, 'Slow',),
        (3, 5, 'Medium',),
        (3, 10, 'Fast',),
        (3, 50, 'Very Fast',),
        (3, 100, 'Ultra Fast',),
        (3, 500, 'Ridiculously fast',),
        (3, 1000, 'Outrageously fast',),
        (3, 5000, 'Astronomically fast',),
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

        self.engine.expressInterest(
            self,
            ('speed', 'passes', 'options'))

        # Views

        hbox1 = gtk.HBox(False, 5)
        self.hbox1 = hbox1
        self.add(hbox1)

        spacer1 = gtk.VBox()
        spacer1.set_size_request(5, 5)
        hbox1.pack_start(spacer1, False, False, 0)

        vbox1 = gtk.VBox(False, 5)
        self.vbox1 = vbox1
        hbox1.pack_start(vbox1, False, False, 0)
        vbox1.set_size_request(200, -1)

        spacer2 = gtk.HBox()
        spacer2.set_size_request(5, 5)
        vbox1.pack_start(spacer2, False, False, 0)

        labelSpeed = gtk.Label('')
        self.labelSpeed = labelSpeed
        vbox1.pack_start(labelSpeed, False, False, 0)
        labelSpeed.show()

        scaleSpeed = gtk.HScale()
        self.scaleSpeed = scaleSpeed
        scaleSpeed.set_digits(0)
        scaleSpeed.set_draw_value(False)
        scaleSpeed.set_value_pos(1)
        scaleSpeed.set_range(0, len(self.speeds) - 1)
        scaleSpeed.set_increments(1, 3)
        scaleSpeed.connect('value-changed', self.speedScaleChanged)
        vbox1.pack_start(scaleSpeed, False, False, 0)
        scaleSpeed.show()
        self.update('speed')

        vbox2 = gtk.VBox(False, 0)
        self.vbox2 = vbox2
        hbox1.pack_start(vbox2, False, False, 0)

        spacer3 = gtk.HBox()
        spacer3.set_size_request(5, 5)
        vbox2.pack_start(spacer3, False, False, 0)

        self.checkButtonDisasters = gtk.CheckButton("Disasters")
        self.checkButtonDisasters.connect('toggled', self.doOptionsCheckbox, 'disasters')
        self.vbox2.pack_start(self.checkButtonDisasters, False, False, 0)

        self.checkButtonAutoBudget = gtk.CheckButton("Auto Budget")
        self.checkButtonAutoBudget.connect('toggled', self.doOptionsCheckbox, 'autobudget')
        self.vbox2.pack_start(self.checkButtonAutoBudget, False, False, 0)

        self.checkButtonAutoBulldoze = gtk.CheckButton("Auto Bulldoze")
        self.checkButtonAutoBulldoze.connect('toggled', self.doOptionsCheckbox, 'autobulldoze')
        self.vbox2.pack_start(self.checkButtonAutoBulldoze, False, False, 0)

        self.checkButtonAutoGoto = gtk.CheckButton("Auto Goto")
        self.checkButtonAutoGoto.connect('toggled', self.doOptionsCheckbox, 'autogoto')
        self.vbox2.pack_start(self.checkButtonAutoGoto, False, False, 0)

        vbox3 = gtk.VBox(False, 0)
        self.vbox3 = vbox3
        hbox1.pack_start(vbox3, False, False, 0)

        spacer4 = gtk.HBox()
        spacer4.set_size_request(5, 5)
        vbox3.pack_start(spacer4, False, False, 0)

        self.checkButtonSound = gtk.CheckButton("Sound")
        self.checkButtonSound.connect('toggled', self.doOptionsCheckbox, 'sound')
        self.vbox3.pack_start(self.checkButtonSound, False, False, 0)

        self.checkButtonAnimation = gtk.CheckButton("Animation")
        self.checkButtonAnimation.connect('toggled', self.doOptionsCheckbox, 'animation')
        self.vbox3.pack_start(self.checkButtonAnimation, False, False, 0)

        self.checkButtonMessages = gtk.CheckButton("Messages")
        self.checkButtonMessages.connect('toggled', self.doOptionsCheckbox, 'messages')
        self.vbox3.pack_start(self.checkButtonMessages, False, False, 0)

        self.checkButtonNotices = gtk.CheckButton("Notices")
        self.checkButtonNotices.connect('toggled', self.doOptionsCheckbox, 'notices')
        self.vbox3.pack_start(self.checkButtonNotices, False, False, 0)

        self.update('options')

        vbox4 = gtk.VBox(False, 0)
        self.vbox4 = vbox4
        hbox1.pack_start(vbox4, False, False, 0)

        buttonAbout = gtk.Button("About")
        self.buttonAbout = buttonAbout
        buttonAbout.connect('clicked', lambda item: self.target.aboutDialog())
        vbox4.pack_start(buttonAbout, False, False, 0)

        buttonSaveCity = gtk.Button("Save City")
        self.buttonSaveCity = buttonSaveCity
        buttonSaveCity.connect('clicked', lambda item: self.target.saveCityDialog())
        vbox4.pack_start(buttonSaveCity, False, False, 0)

        buttonSaveCityAs = gtk.Button("Save City As...")
        self.buttonSaveCityAs = buttonSaveCityAs
        buttonSaveCityAs.connect('clicked', lambda item: self.target.saveCityAsDialog())
        vbox4.pack_start(buttonSaveCityAs, False, False, 0)

        buttonNewCity = gtk.Button("New City")
        self.buttonNewCity = buttonNewCity
        buttonNewCity.connect('clicked', lambda item: self.target.newCityDialog())
        vbox4.pack_start(buttonNewCity, False, False, 0)

        buttonQuit = gtk.Button("Quit")
        self.buttonQuit = buttonQuit
        buttonQuit.connect('clicked', lambda item: self.target.quitDialog())
        vbox4.pack_start(buttonQuit, False, False, 0)


    def update(
        self,
        name,
        *args):

        #print "MicropolisControlPanel update", self, name, args

        engine = self.engine

        if name in ('speed', 'passes',):
            scaleSpeed = self.scaleSpeed
            simSpeed = engine.simSpeed
            simPasses = engine.simPasses
            #print "SIMULATOR CHANGED SPEED/PASSES", simSpeed, simPasses
            found = None
            i = 0
            for speed, passes, label in self.speeds:
                if ((speed == simSpeed) and
                    (passes == simPasses)):
                    if scaleSpeed.get_value() != i:
                        scaleSpeed.set_value(i)
                    print "found speed", speed, "passes", passes, "i", i, "label", label
                    self.setSpeedLabel(label)
                    return
                i += 1
            i = 0
            for speed, passes, label in self.speeds:
                if (speed == simSpeed):
                    if scaleSpeed.get_value() != i:
                        scaleSpeed.set_value(i)
                    print "found speed", speed, "any passes", passes, "current", engine.simPasses, "i", i, "label", label
                    if passes != engine.simPasses:
                        engine.setPasses(passes)
                    self.setSpeedLabel(label)
                    return
                i += 1
            i = 3 # Default: "slow", fast speed, no skips
            if scaleSpeed.get_value() != i:
                scaleSpeed.set_value()
            self.setSpeedLabel(self.speeds[i][2])
            print "default", i, self.speeds[i][2]
        elif name == 'options':
            self.checkButtonDisasters.set_property('active', engine.enableDisasters)
            self.checkButtonAutoBudget.set_property('active', engine.autoBudget)
            self.checkButtonAutoBulldoze.set_property('active', engine.autoBulldoze)
            self.checkButtonAutoGoto.set_property('active', engine.autoGoto)
            self.checkButtonSound.set_property('active', engine.enableSound)
            self.checkButtonAnimation.set_property('active', engine.doAnimation)
            self.checkButtonMessages.set_property('active', engine.doMessages)
            self.checkButtonNotices.set_property('active', engine.doNotices)


    def setSpeedLabel(self, label):
        label = 'Speed: ' + label
        self.labelSpeed.set_text(label)


    def speedScaleChanged(self, scale):
        engine = self.engine
        i = int(scale.get_value())
        speed, passes, label = self.speeds[i]
        if speed != engine.simSpeed:
            engine.setSpeed(speed)
        if passes != engine.simPasses:
            engine.setPasses(passes)
        self.setSpeedLabel(label)


    def doOptionsCheckbox(self, item, option):
        engine = self.engine
        flag = item.get_property('active')
        #print "OPTIONCHANGED", item, option, flag
        if option == 'disasters':
            engine.setEnableDisasters(flag)
        elif option == 'autobudget':
            engine.setAutoBudget(flag)
        elif option == 'autobulldoze':
            engine.setAutoBulldoze(flag)
        elif option == 'autogoto':
            engine.setAutoGoto(flag)
        elif option == 'sound':
            engine.setEnableSound(flag)
        elif option == 'animation':
            engine.setDoAnimation(flag)
        elif option == 'messages':
            engine.setDoMessages(flag)
        elif option == 'notices':
            engine.setDoNotices(flag)


########################################################################
