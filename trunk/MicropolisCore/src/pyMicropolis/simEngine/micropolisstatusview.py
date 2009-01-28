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
import micropolisnoticepanel
import micropolismessagespanel
import micropolisdrawingarea
import micropolisevaluationpanel
import micropolishistorypanel
import micropolisbudgetpanel
import micropolismappanel
import micropoliscontrolpanel
import micropolisdisasterspanel


########################################################################
# MicropolisStatusView


class MicropolisStatusView(gtk.HPaned):


    def __init__(
        self,
        engine=None,
        centerOnTileHandler=None,
        **args):

        gtk.HPaned.__init__(
            self,
            **args)

        self.engine = engine

        # Views

        self.vbox2 = gtk.VBox(False, 0)
        self.pack1(self.vbox2, resize=False, shrink=False)

        self.gaugeView = micropolisgaugeview.MicropolisGaugeView(engine=self.engine)
        self.vbox2.pack_start(self.gaugeView, False, False, 0)

        self.tileView = \
            micropolisdrawingarea.MiniMicropolisDrawingArea(
                engine=self.engine)
        self.tileView.panTo(0, 0)
        self.tileView.setScale(
            1.0 / micropolisengine.EDITOR_TILE_SIZE)
        self.tileView.set_size_request(
            micropolisengine.WORLD_W, 
            micropolisengine.WORLD_H)
        engine.addView(self.tileView)

        self.vbox2.pack_start(self.tileView, True, True, 1)

        self.hbox1 = gtk.HBox(False, 0)
        self.pack2(self.hbox1, resize=False, shrink=False)

        self.notebook = gtk.Notebook()
        self.hbox1.pack_start(self.notebook, True, True, 0)

        self.noticePanel = micropolisnoticepanel.MicropolisNoticePanel(
            engine=engine,
            centerOnTileHandler=centerOnTileHandler)
        self.noticeViewLabel = gtk.Label('Notice')
        self.notebook.append_page(self.noticePanel, self.noticeViewLabel)

        self.messagesPanel = micropolismessagespanel.MicropolisMessagesPanel(
            engine=engine)
        self.messagesViewLabel = gtk.Label('Messages')
        self.notebook.append_page(self.messagesPanel, self.messagesViewLabel)

        self.evaluationPanel = micropolisevaluationpanel.MicropolisEvaluationPanel(
            engine=engine)
        self.evaluationLabel = gtk.Label("Evaluation")
        self.notebook.append_page(self.evaluationPanel, self.evaluationLabel)

        self.historyPanel = micropolishistorypanel.MicropolisHistoryPanel(
            engine=engine)
        self.historyLabel = gtk.Label("History")
        self.notebook.append_page(self.historyPanel, self.historyLabel)

        self.budgetPanel = micropolisbudgetpanel.MicropolisBudgetPanel(
            engine=engine)
        self.budgetLabel = gtk.Label("Budget")
        self.notebook.append_page(self.budgetPanel, self.budgetLabel)

        self.mapPanel = micropolismappanel.MicropolisMapPanel(
            engine=engine)
        self.mapLabel = gtk.Label("Map")
        self.notebook.append_page(self.mapPanel, self.mapLabel)

        self.controlPanel = micropoliscontrolpanel.MicropolisControlPanel(
            engine=engine)
        self.controlLabel = gtk.Label("Control")
        self.notebook.append_page(self.controlPanel, self.controlLabel)

        self.disastersPanel = micropolisdisasterspanel.MicropolisDisastersPanel(
            engine=engine)
        self.disastersLabel = gtk.Label("Disasters")
        self.notebook.append_page(self.disastersPanel, self.disastersLabel)

        self.set_position(self.gaugeView.viewWidth)


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
                

    def doMicropolisMenu(self, item, command):
        #print "DOMICROPOLISMENU", command
        engine = self.engine
        if command == 'about':
            engine.sendMessage(micropolisengine.MESSAGE_ABOUT_MICROPOLIS)
        elif command == 'savecity':
            pass
        elif command == 'savecityas':
            pass
        elif command == 'newcity':
            pass
        elif command == 'quit':
            pass


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
