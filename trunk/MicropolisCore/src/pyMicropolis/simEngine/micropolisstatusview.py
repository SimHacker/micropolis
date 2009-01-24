# micropolisstatusview.py
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
import micropolisgaugeview
import micropolisnoticeview
import micropolisdrawingarea


########################################################################
# MicropolisStatusView


class MicropolisStatusView(gtk.VBox):


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
        **args):

        gtk.VBox.__init__(
            self,
            False,
            5,
            **args)

        self.engine = engine

        self.engine.expressInterest(
            self,
            ('taxrate', 'speed', 'passes', 'options'))

        # Views

        self.menuBar = gtk.MenuBar()
        self.pack_start(self.menuBar, False, False, 0)

        ########################################################################
        # Micropolis menu.

        self.menuItem_Micropolis = gtk.MenuItem("Micropolis")
        self.menuBar.insert(self.menuItem_Micropolis, 1)

        self.menu_Micropolis = gtk.Menu()
        self.menuItem_Micropolis.set_submenu(self.menu_Micropolis)

        self.menuItem_Micropolis_About = gtk.MenuItem("About")
        self.menuItem_Micropolis_About.connect('activate', self.doMicropolisMenu, 'about')
        self.menu_Micropolis.append(self.menuItem_Micropolis_About)

        self.menuItem_Micropolis_SaveCity = gtk.MenuItem("Save City")
        self.menuItem_Micropolis_SaveCity.connect('activate', self.doMicropolisMenu, 'savecity')
        self.menu_Micropolis.append(self.menuItem_Micropolis_SaveCity)

        self.menuItem_Micropolis_SaveCityAs = gtk.MenuItem("Save City As...")
        self.menuItem_Micropolis_SaveCityAs.connect('activate', self.doMicropolisMenu, 'savecityas')
        self.menu_Micropolis.append(self.menuItem_Micropolis_SaveCityAs)

        self.menuItem_Micropolis_NewCity = gtk.MenuItem("New City")
        self.menuItem_Micropolis_NewCity.connect('activate', self.doMicropolisMenu, 'newcity')
        self.menu_Micropolis.append(self.menuItem_Micropolis_NewCity)

        self.menuItem_Micropolis_Quit = gtk.MenuItem("Quit")
        self.menuItem_Micropolis_Quit.connect('activate', self.doMicropolisMenu, 'quit')
        self.menu_Micropolis.append(self.menuItem_Micropolis_Quit)

        ########################################################################
        # Options menu.

        self.menuItem_Options = gtk.MenuItem("Options")
        self.menuBar.insert(self.menuItem_Options, 2)

        self.menu_Options = gtk.Menu()
        self.menuItem_Options.set_submenu(self.menu_Options)

        self.menuItem_Options_Disasters = gtk.CheckMenuItem("Disasters")
        self.menuItem_Options_Disasters.connect('toggled', self.doOptionsMenu, 'disasters')
        self.menu_Options.append(self.menuItem_Options_Disasters)

        self.menuItem_Options_AutoBudget = gtk.CheckMenuItem("Auto Budget")
        self.menuItem_Options_AutoBudget.connect('toggled', self.doOptionsMenu, 'autobudget')
        self.menu_Options.append(self.menuItem_Options_AutoBudget)

        self.menuItem_Options_AutoBulldoze = gtk.CheckMenuItem("Auto Bulldoze")
        self.menuItem_Options_AutoBulldoze.connect('toggled', self.doOptionsMenu, 'autobulldoze')
        self.menu_Options.append(self.menuItem_Options_AutoBulldoze)

        self.menuItem_Options_AutoGoto = gtk.CheckMenuItem("Auto Goto")
        self.menuItem_Options_AutoGoto.connect('toggled', self.doOptionsMenu, 'autogoto')
        self.menu_Options.append(self.menuItem_Options_AutoGoto)

        self.menuItem_Options_Sound = gtk.CheckMenuItem("Sound")
        self.menuItem_Options_Sound.connect('toggled', self.doOptionsMenu, 'sound')
        self.menu_Options.append(self.menuItem_Options_Sound)

        self.menuItem_Options_Animation = gtk.CheckMenuItem("Animation")
        self.menuItem_Options_Animation.connect('toggled', self.doOptionsMenu, 'animation')
        self.menu_Options.append(self.menuItem_Options_Animation)

        self.menuItem_Options_Messages = gtk.CheckMenuItem("Messages")
        self.menuItem_Options_Messages.connect('toggled', self.doOptionsMenu, 'messages')
        self.menu_Options.append(self.menuItem_Options_Messages)

        self.menuItem_Options_Notices = gtk.CheckMenuItem("Notices")
        self.menuItem_Options_Notices.connect('toggled', self.doOptionsMenu, 'notices')
        self.menu_Options.append(self.menuItem_Options_Notices)

        ########################################################################
        # Disasters menu.

        self.menuItem_Disasters = gtk.MenuItem("Disasters")
        self.menuBar.insert(self.menuItem_Disasters, 3)

        self.menu_Disasters = gtk.Menu()
        self.menuItem_Disasters.set_submenu(self.menu_Disasters)

        self.menuItem_Disasters_Monster = gtk.MenuItem("Monster")
        self.menuItem_Disasters_Monster.connect('activate', self.doDisastersMenu, 'monster')
        self.menu_Disasters.append(self.menuItem_Disasters_Monster)

        self.menuItem_Disasters_Fire = gtk.MenuItem("Fire")
        self.menuItem_Disasters_Fire.connect('activate', self.doDisastersMenu, 'fire')
        self.menu_Disasters.append(self.menuItem_Disasters_Fire)

        self.menuItem_Disasters_Flood = gtk.MenuItem("Flood")
        self.menuItem_Disasters_Flood.connect('activate', self.doDisastersMenu, 'flood')
        self.menu_Disasters.append(self.menuItem_Disasters_Flood)

        self.menuItem_Disasters_Meltdown = gtk.MenuItem("Meltdown")
        self.menuItem_Disasters_Meltdown.connect('activate', self.doDisastersMenu, 'meltdown')
        self.menu_Disasters.append(self.menuItem_Disasters_Meltdown)

        self.menuItem_Disasters_Tornado = gtk.MenuItem("Tornado")
        self.menuItem_Disasters_Tornado.connect('activate', self.doDisastersMenu, 'tornado')
        self.menu_Disasters.append(self.menuItem_Disasters_Tornado)

        self.menuItem_Disasters_Earthquake = gtk.MenuItem("Earthquake")
        self.menuItem_Disasters_Earthquake.connect('activate', self.doDisastersMenu, 'earthquake')
        self.menu_Disasters.append(self.menuItem_Disasters_Earthquake)

        ########################################################################
        # Windows menu.

        self.menuItem_Windows = gtk.MenuItem("Windows")
        self.menuBar.insert(self.menuItem_Windows, 5)

        self.menu_Windows = gtk.Menu()
        self.menuItem_Windows.set_submenu(self.menu_Windows)

        self.menuItem_Windows_Budget = gtk.CheckMenuItem("Budget")
        self.menuItem_Windows_Budget.set_property('active', True)
        self.menuItem_Windows_Budget.connect('toggled', self.doWindowMenu, 'budget')
        self.menu_Windows.append(self.menuItem_Windows_Budget)

        self.menuItem_Windows_Evaluation = gtk.CheckMenuItem("Evaluation")
        self.menuItem_Windows_Evaluation.set_property('active', True)
        self.menuItem_Windows_Evaluation.connect('toggled', self.doWindowMenu, 'evaluation')
        self.menu_Windows.append(self.menuItem_Windows_Evaluation)

        self.menuItem_Windows_Graph = gtk.CheckMenuItem("Graph")
        self.menuItem_Windows_Graph.set_property('active', True)
        self.menuItem_Windows_Graph.connect('toggled', self.doWindowMenu, 'graph')
        self.menu_Windows.append(self.menuItem_Windows_Graph)

        self.menuItem_Windows_Map = gtk.CheckMenuItem("Map")
        self.menuItem_Windows_Map.set_property('active', False)
        self.menuItem_Windows_Map.connect('toggled', self.doWindowMenu, 'map')
        self.menu_Windows.append(self.menuItem_Windows_Map)

        self.menuItem_Windows_Editor = gtk.CheckMenuItem("Editor")
        self.menuItem_Windows_Editor.set_property('active', True)
        self.menuItem_Windows_Editor.connect('toggled', self.doWindowMenu, 'editor')
        self.menu_Windows.append(self.menuItem_Windows_Editor)

        ########################################################################

        self.hpaned1 = gtk.HPaned()
        self.pack_start(self.hpaned1, False, False, 1)

        self.vbox2 = gtk.VBox(False, 0)
        self.hpaned1.pack1(self.vbox2, resize=False, shrink=False)

        self.gaugeView = micropolisgaugeview.MicropolisGaugeView(engine=self.engine)
        self.vbox2.pack_start(self.gaugeView, False, False, 0)

        self.vbox3 = gtk.VBox(False, 0)
        self.vbox3.set_size_request(5, 5)
        self.vbox2.pack_start(self.vbox3, False, False, 1)

        self.labelTaxRate = gtk.Label('')
        self.vbox2.pack_start(self.labelTaxRate, False, False, 2)
        self.labelTaxRate.show()

        self.scaleTaxRate = gtk.HScale()
        self.scaleTaxRate.set_digits(0)
        self.scaleTaxRate.set_draw_value(False)
        self.scaleTaxRate.set_value_pos(1)
        self.scaleTaxRate.set_range(0, 20)
        self.scaleTaxRate.set_increments(1, 5)
        self.scaleTaxRate.set_value(engine.cityTax)
        self.scaleTaxRate.connect('value-changed', self.taxScaleChanged)
        self.vbox2.pack_start(self.scaleTaxRate, False, False, 3)
        #self.scaleTaxRate.set_size_request(200, 20)
        self.scaleTaxRate.show()
        self.update('taxrate')

        self.labelSpeed = gtk.Label('')
        self.vbox2.pack_start(self.labelSpeed, False, False, 4)
        self.labelSpeed.show()

        self.scaleSpeed = gtk.HScale()
        self.scaleSpeed.set_digits(0)
        self.scaleSpeed.set_draw_value(False)
        self.scaleSpeed.set_value_pos(1)
        self.scaleSpeed.set_range(0, len(self.speeds) - 1)
        self.scaleSpeed.set_increments(1, 3)
        self.scaleSpeed.connect('value-changed', self.speedScaleChanged)
        self.vbox2.pack_start(self.scaleSpeed, False, False, 5)
        #self.scaleSpeed.set_size_request(200, 20)
        self.scaleSpeed.show()
        self.update('speed')

        self.hbox1 = gtk.HBox(False, 0)
        self.hpaned1.pack2(self.hbox1, resize=False, shrink=False)

        self.noticeView = micropolisnoticeview.MicropolisNoticeView(engine=engine, statusview=self)
        #self.noticeView.set_size_request(200, 0)
        self.hbox1.pack_start(self.noticeView, True, True, 0)

        self.cityView = micropolisdrawingarea.NoticeMicropolisDrawingArea(engine=engine)
        self.cityViewVisible = False
        self.setCityViewVisible(True, 119, 99)

        self.hpaned1.set_position(self.gaugeView.viewWidth)


    def setCityViewVisible(self, visible, tileX=-1, tileY=-1):
        engine = self.engine
        cityView = self.cityView
        #print "setCityViewVisible", visible, self.cityViewVisible, tileX, tileY
        if visible and (tileX >= 0) and (tileY >= 0):
            cityView.centerOnTile(tileX, tileY)
        if self.cityViewVisible == visible:
            return
        if visible:
            cityView.set_size_request(150, 0)
            self.hbox1.pack_start(cityView, False, False, 1)
            engine.addView(cityView)
        else:
            self.hbox1.remove(cityView)
            engine.removeView(cityView)
        self.cityViewVisible = visible


    def update(
        self,
        name,
        *args):

        #print "MicropolisStatusView update", self, name, args

        engine = self.engine

        if name == 'taxrate':
            taxRate = engine.cityTax
            if self.scaleTaxRate.get_value() != taxRate:
                self.scaleTaxRate.set_value(taxRate)
            self.setTaxRateLabel(taxRate)
        elif name in ('speed', 'passes',):
            simSpeed = engine.simSpeed
            simPasses = engine.simPasses
            #print "SIMULATOR CHANGED SPEED/PASSES", simSpeed, simPasses
            found = None
            i = 0
            for speed, passes, label in self.speeds:
                if ((speed == simSpeed) and 
                    (passes == simPasses)):
                    if self.scaleSpeed.get_value() != i:
                        self.scaleSpeed.set_value(i)
                    self.setSpeedLabel(label)
                    return
                i += 1
            if self.scaleSpeed.get_value() != simSpeed:
                self.scaleSpeed.set_value(simSpeed)
            self.setSpeedLabel(self.speeds[simSpeed][2])
                
        elif name == 'options':
            #print "======================================================================== OPTIONS"
            #print "enableDisasters", engine.enableDisasters # Disasters
            self.menuItem_Options_Disasters.set_property('active', engine.enableDisasters)
            #print "autoBudget", engine.autoBudget # AutoBudget
            self.menuItem_Options_AutoBudget.set_property('active', engine.autoBudget)
            #print "autoBulldoze", engine.autoBulldoze # AutoBulldoze
            self.menuItem_Options_AutoBulldoze.set_property('active', engine.autoBulldoze)
            #print "autoGoto", engine.autoGoto # AutoGoto
            self.menuItem_Options_AutoGoto.set_property('active', engine.autoGoto)
            #print "enableSound", engine.enableSound # Sound
            self.menuItem_Options_Sound.set_property('active', engine.enableSound)
            #print "doAnimation", engine.doAnimation # Animation
            self.menuItem_Options_Animation.set_property('active', engine.doAnimation)
            #print "doMessages", engine.doMessages # Messages
            self.menuItem_Options_Messages.set_property('active', engine.doMessages)
            #print "doNotices", engine.doNotices # Notices
            self.menuItem_Options_Notices.set_property('active', engine.doNotices)


    def doMicropolisMenu(self, item, command):
        #print "DOMICROPOLISMENU", command
        if command == 'about':
            pass
        elif command == 'savecity':
            pass
        elif command == 'savecityas':
            pass
        elif command == 'newcity':
            pass
        elif command == 'quit':
            pass


    def doOptionsMenu(self, item, option):
        engine = self.engine
        flag = item.active
        #print "OPTIONCHANGED", option, flag
        if option == 'disasters':
            print 'DISASTERS', flag
            engine.setEnableDisasters(flag)
        elif option == 'autobudget':
            print 'AUTOBUDGET', flag
            engine.setAutoBudget(flag)
        elif option == 'autobulldoze':
            print 'AUTOBULLDOZE', flag
            engine.setAutoBulldoze(flag)
        elif option == 'autogoto':
            print 'AUTOGOTO', flag
            engine.setAutoGoto(flag)
        elif option == 'sound':
            print 'SOUND', flag
            engine.setEnableSound(flag)
        elif option == 'animation':
            print 'ANIMATION', flag
            engine.setDoAnimation(flag)
        elif option == 'messages':
            print 'MESSAGES', flag
            engine.setDoMessages(flag)
        elif option == 'notices':
            print 'NOTICES', flag
            engine.setDoNotices(flag)


    def doDisastersMenu(self, item, disaster):
        engine = self.engine
        if disaster == 'monster':
            engine.makeMonster()
        elif disaster == 'fire':
            engine.setFire()
        elif disaster == 'flood':
            engine.makeFlood()
        elif disaster == 'meltdown':
            engine.makeMeltdown()
        elif disaster == 'tornado':
            engine.makeTornado()
        elif disaster == 'earthquake':
            engine.makeEarthquake()


    def doWindowMenu(self, item, window):
        flag = item.active
        #print "DOWINDOWMENU", window, flag
        if window == 'budget':
            pass
        elif window == 'evaluation':
            pass
        elif window == 'graph':
            pass
        elif window == 'map':
            pass
        elif window == 'editor':
            pass


    def setSpeedLabel(self, label):
        label = 'Speed: ' + label
        self.labelSpeed.set_text(label)


    def setTaxRateLabel(self, tax):
        label = 'Tax Rate: ' + str(tax) + '%'
        self.labelTaxRate.set_text(label)


    def taxScaleChanged(self, scale):
        engine = self.engine
        tax = int(scale.get_value())
        if tax != engine.cityTax:
            engine.setCityTax(tax)


    def speedScaleChanged(self, scale):
        engine = self.engine
        i = int(scale.get_value())
        speed, passes, label = self.speeds[i]
        if speed != engine.simSpeed:
            engine.setSpeed(speed)
        if passes != engine.simPasses:
            engine.setPasses(passes)
        self.setSpeedLabel(label)


########################################################################
