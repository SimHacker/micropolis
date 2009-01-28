# micropolisbudgetpanel.py
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
# MicropolisBudgetPanel


class MicropolisBudgetPanel(gtk.Frame):


    def __init__(
        self,
        engine=None,
        **args):

        gtk.Frame.__init__(
            self,
            **args)

        self.engine = engine

        self.engine.expressInterest(
            self,
            ('taxrate', 'budget',))

        # Views

        hbox1 = gtk.HBox(False, 5)
        self.hbox1 = hbox1
        self.add(hbox1)

        spacer1 = gtk.VBox()
        spacer1.set_size_request(5, 5)
        hbox1.pack_start(spacer1, False, False, 0)

        vbox1 = gtk.VBox(False, 5)
        self.vbox1 = vbox1
        hbox1.pack_start(vbox1, False, False, 1)
        vbox1.set_size_request(200, 0)

        spacer2 = gtk.HBox()
        spacer2.set_size_request(5, 5)
        vbox1.pack_start(spacer2, False, False, 0)

        labelTaxRate = gtk.Label('')
        self.labelTaxRate = labelTaxRate
        self.vbox1.pack_start(labelTaxRate, False, False, 1)
        labelTaxRate.show()

        scaleTaxRate = gtk.HScale()
        self.scaleTaxRate = scaleTaxRate
        scaleTaxRate = scaleTaxRate
        scaleTaxRate.set_digits(0)
        scaleTaxRate.set_draw_value(False)
        scaleTaxRate.set_value_pos(1)
        scaleTaxRate.set_range(0, 20)
        scaleTaxRate.set_increments(1, 5)
        scaleTaxRate.set_value(engine.cityTax)
        scaleTaxRate.connect('value-changed', self.taxScaleChanged)
        vbox1.pack_start(scaleTaxRate, False, False, 1)
        #scaleTaxRate.set_size_request(200, 20)
        scaleTaxRate.show()
        self.update('taxrate')

        vbox2 = gtk.VBox(False, 5)
        self.vbox2 = vbox2
        hbox1.pack_start(vbox2, False, False, 2)
        vbox2.set_size_request(200, 0)

        spacer3 = gtk.HBox()
        spacer3.set_size_request(5, 5)
        vbox2.pack_start(spacer3, False, False, 0)

        labelFirePercent = gtk.Label('')
        self.labelFirePercent = labelFirePercent
        self.vbox2.pack_start(labelFirePercent, False, False, 1)
        labelFirePercent.show()

        scaleFirePercent = gtk.HScale()
        self.scaleFirePercent = scaleFirePercent
        scaleFirePercent = scaleFirePercent
        scaleFirePercent.set_digits(0)
        scaleFirePercent.set_draw_value(False)
        scaleFirePercent.set_value_pos(1)
        scaleFirePercent.set_range(0, 100)
        scaleFirePercent.set_increments(1, 10)
        scaleFirePercent.set_value(engine.cityTax)
        scaleFirePercent.connect('value-changed', self.fireScaleChanged)
        vbox2.pack_start(scaleFirePercent, False, False, 1)
        scaleFirePercent.show()

        labelPolicePercent = gtk.Label('')
        self.labelPolicePercent = labelPolicePercent
        self.vbox2.pack_start(labelPolicePercent, False, False, 1)
        labelPolicePercent.show()

        scalePolicePercent = gtk.HScale()
        self.scalePolicePercent = scalePolicePercent
        scalePolicePercent = scalePolicePercent
        scalePolicePercent.set_digits(0)
        scalePolicePercent.set_draw_value(False)
        scalePolicePercent.set_value_pos(1)
        scalePolicePercent.set_range(0, 100)
        scalePolicePercent.set_increments(1, 10)
        scalePolicePercent.set_value(engine.cityTax)
        scalePolicePercent.connect('value-changed', self.policeScaleChanged)
        vbox2.pack_start(scalePolicePercent, False, False, 1)
        scalePolicePercent.show()

        labelRoadPercent = gtk.Label('')
        self.labelRoadPercent = labelRoadPercent
        self.vbox2.pack_start(labelRoadPercent, False, False, 1)
        labelRoadPercent.show()

        scaleRoadPercent = gtk.HScale()
        self.scaleRoadPercent = scaleRoadPercent
        scaleRoadPercent = scaleRoadPercent
        scaleRoadPercent.set_digits(0)
        scaleRoadPercent.set_draw_value(False)
        scaleRoadPercent.set_value_pos(1)
        scaleRoadPercent.set_range(0, 100)
        scaleRoadPercent.set_increments(1, 10)
        scaleRoadPercent.set_value(engine.cityTax)
        scaleRoadPercent.connect('value-changed', self.roadScaleChanged)
        vbox2.pack_start(scaleRoadPercent, False, False, 1)
        scaleRoadPercent.show()

        self.update('budget')


    def update(
        self,
        name,
        *args):

        #print "MicropolisBudgetPanel update", self, name, args

        engine = self.engine

        if name  == 'taxrate':

            taxRate = engine.cityTax
            scaleTaxRate = self.scaleTaxRate
            if scaleTaxRate.get_value() != taxRate:
                scaleTaxRate.set_value(taxRate)
            self.setTaxRateLabel(taxRate)

        elif name == 'budget':

            formatMoney = engine.formatMoney
            formatPercent = engine.formatPercent

            totalFunds = engine.totalFunds
            taxFund = engine.taxFund
            fireFund = engine.fireFund
            fireValue = engine.fireValue
            firePercent = engine.firePercent
            policeFund = engine.policeFund
            policeValue = engine.policeValue
            policePercent = engine.policePercent
            roadFund = engine.roadFund
            roadValue = engine.roadValue
            roadPercent = engine.roadPercent

            cashFlow = (
                taxFund - 
                fireValue - 
                policeValue - 
                roadValue)

            cashFlow2 = cashFlow

            if cashFlow == 0:
                cashFlowString = "$0"
            elif cashFlow > 0:
                cashFlowString = "+" + formatMoney(cashFlow)
            else:
                cashFlowString = "-" + formatMoney(-cashFlow)

            previousString = formatMoney(totalFunds)
            currentString = formatMoney(cashFlow2 + totalFunds)
            collectedString = formatMoney(taxFund)

            fireWantString = formatMoney(fireFund)
            firePercentString = formatPercent(firePercent)
            fireGotString = formatMoney(int(fireFund * firePercent))

            policeWantString = formatMoney(policeFund)
            policePercentString = formatPercent(policePercent)
            policeGotString = formatMoney(int(fireFund * roadPercent))

            roadWantString = formatMoney(roadFund)
            roadPercentString = formatPercent(roadPercent)
            roadGotString = formatMoney(int(roadFund * roadPercent))

            print (
                "Cash Flow: " + cashFlowString + "\n" +
                "Previous: " + previousString + "\n" +
                "Current: " + currentString + "\n" +
                "Collected: " + collectedString + "\n" +
                "Fire Want: " + fireWantString + "\n" +
                "Fire Percent: " + firePercentString + "\n" +
                "Fire Got: " + fireGotString + "\n" +
                "Police Want: " + policeWantString + "\n" +
                "Police Percent: " + policePercentString + "\n" +
                "Police Got: " + policeGotString + "\n" +
                "Road Want: " + roadWantString + "\n" +
                "Road Percent: " + roadPercentString + "\n" +
                "Road Got: " + roadGotString + "\n"
            )


    def taxScaleChanged(self, scale):
        engine = self.engine
        tax = int(scale.get_value())
        if tax != engine.cityTax:
            engine.setCityTax(tax)


    def setTaxRateLabel(self, tax):
        label = 'Tax Rate: ' + str(tax) + '%'
        self.labelTaxRate.set_text(label)


    def fireScaleChanged(self, scale):
        engine = self.engine
        fire = scale.get_value() / 100.0
        engine.firePercent = fire


    def setFireRateLabel(self, fire):
        label = 'Fire Rate: ' + self.engine.formatPercent(fire)
        self.labelFireRate.set_text(label)


    def policeScaleChanged(self, scale):
        engine = self.engine
        police = scale.get_value() / 100.0
        engine.policePercent = police


    def setPoliceRateLabel(self, police):
        label = 'Police Rate: ' + self.engine.formatPercent(police)
        self.labelPoliceRate.set_text(label)


    def roadScaleChanged(self, scale):
        engine = self.engine
        road = scale.get_value() / 100.0
        engine.roadPercent = road


    def setRoadRateLabel(self, road):
        label = 'Road Rate: ' + self.engine.formatPercent(road)
        self.labelRoadRate.set_text(label)


########################################################################
