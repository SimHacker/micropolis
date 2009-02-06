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

        # Taxes Collected: $
        # Cash Flow: $
        # Previous Funds: $
        # Current Funds: $

        vbox0 = gtk.VBox(False, 5)
        self.vbox0 = vbox0
        self.add(vbox0)

        spacer0 = gtk.HBox()
        spacer0.set_size_request(5, 5)
        vbox0.pack_start(spacer0, False, False, 0)

        hbox1 = gtk.HBox(False, 5)
        self.hbox1 = hbox1
        vbox0.pack_start(hbox1, False, False, 0)

        vbox10 = gtk.VBox(False, 5)
        self.vbox10 = vbox10
        hbox1.pack_start(vbox10, True, True, 0)

        labelBudget1 = gtk.Label('Taxes Collected: $100,000,000')
        self.labelBudget1 = labelBudget1
        vbox10.pack_start(labelBudget1, False, False, 0)
        labelBudget1.show()

        labelBudget2 = gtk.Label('Cash Flow: +$100,000,000')
        self.labelBudget2 = labelBudget2
        vbox10.pack_start(labelBudget2, False, False, 0)
        labelBudget2.show()

        spacer1 = gtk.HBox()
        spacer1.set_size_request(5, 5)
        hbox1.pack_start(spacer1, False, False, 0)

        vbox11 = gtk.VBox(False, 5)
        self.vbox11 = vbox11
        hbox1.pack_start(vbox11, True, True, 0)

        labelBudget3 = gtk.Label('Previous Funds: $100,000,000')
        self.labelBudget3 = labelBudget3
        vbox11.pack_start(labelBudget3, False, False, 0)
        labelBudget3.show()

        labelBudget4 = gtk.Label('Current Funds: $100,000,000')
        self.labelBudget4 = labelBudget4
        vbox11.pack_start(labelBudget4, False, False, 0)
        labelBudget4.show()

        spacer2 = gtk.VBox()
        spacer2.set_size_request(5, 5)
        vbox0.pack_start(spacer2, False, False, 0)

        hbox2 = gtk.HBox(False, 5)
        self.hbox2 = hbox2
        vbox0.pack_start(hbox2, False, False, 0)

        vbox1 = gtk.VBox(False, 5)
        self.vbox1 = vbox1
        hbox2.pack_start(vbox1, True, True, 0)
        vbox1.set_size_request(100, 0)

        labelTaxTitle = gtk.Label('Tax Rate')
        self.labelTaxTitle = labelTaxTitle
        vbox1.pack_start(labelTaxTitle, False, False, 0)
        labelTaxTitle.show()

        labelTaxRate1 = gtk.Label('')
        self.labelTaxRate1 = labelTaxRate1
        vbox1.pack_start(labelTaxRate1, False, False, 0)
        labelTaxRate1.show()

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
        vbox1.pack_start(scaleTaxRate, False, False, 0)
        #scaleTaxRate.set_size_request(200, 20)
        scaleTaxRate.show()

        labelTaxRate2 = gtk.Label('0%')
        self.labelTaxRate2 = labelTaxRate2
        vbox1.pack_start(labelTaxRate2, False, False, 0)
        labelTaxRate2.show()

        vbox2 = gtk.VBox(False, 5)
        self.vbox2 = vbox2
        hbox2.pack_start(vbox2, True, True, 0)
        vbox2.set_size_request(100, 0)

        labelFireTitle = gtk.Label('Fire Station Fund')
        self.labelFireTitle = labelFireTitle
        vbox2.pack_start(labelFireTitle, False, False, 0)
        labelFireTitle.show()

        labelFireRequest = gtk.Label('Request: $100,000')
        self.labelFireRequest = labelFireRequest
        vbox2.pack_start(labelFireRequest, False, False, 0)
        labelFireRequest.show()

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
        vbox2.pack_start(scaleFirePercent, False, False, 0)
        scaleFirePercent.show()

        labelFirePercent = gtk.Label('100%')
        self.labelFirePercent = labelFirePercent
        vbox2.pack_start(labelFirePercent, False, False, 0)
        labelFirePercent.show()

        vbox3 = gtk.VBox(False, 5)
        self.vbox3 = vbox3
        hbox2.pack_start(vbox3, True, True, 0)
        vbox3.set_size_request(100, 0)

        labelPoliceTitle = gtk.Label('Police Station Fund')
        self.labelPoliceTitle = labelPoliceTitle
        vbox3.pack_start(labelPoliceTitle, False, False, 0)
        labelPoliceTitle.show()

        labelPoliceRequest = gtk.Label('Request: $100,000')
        self.labelPoliceRequest = labelPoliceRequest
        vbox3.pack_start(labelPoliceRequest, False, False, 0)
        labelPoliceRequest.show()

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
        vbox3.pack_start(scalePolicePercent, False, False, 0)
        scalePolicePercent.show()

        labelPolicePercent = gtk.Label('100%')
        self.labelPolicePercent = labelPolicePercent
        vbox3.pack_start(labelPolicePercent, False, False, 0)
        labelPolicePercent.show()

        vbox4 = gtk.VBox(False, 5)
        self.vbox4 = vbox4
        hbox2.pack_start(vbox4, True, True, 0)
        vbox4.set_size_request(100, 0)

        labelRoadTitle = gtk.Label('Road Fund')
        self.labelRoadTitle = labelRoadTitle
        vbox4.pack_start(labelRoadTitle, False, False, 0)
        labelRoadTitle.show()

        labelRoadRequest = gtk.Label('Road Request: $100,000')
        self.labelRoadRequest = labelRoadRequest
        vbox4.pack_start(labelRoadRequest, False, False, 0)
        labelRoadRequest.show()

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
        vbox4.pack_start(scaleRoadPercent, False, False, 0)
        scaleRoadPercent.show()

        labelRoadPercent = gtk.Label('100%')
        self.labelRoadPercent = labelRoadPercent
        vbox4.pack_start(labelRoadPercent, False, False, 0)
        labelRoadPercent.show()

        self.update('taxrate')
        self.update('budget')


    def update(
        self,
        name,
        *args):

        #print "MicropolisBudgetPanel update", name, args

        engine = self.engine

        if name  == 'taxrate':

            taxRate = engine.cityTax
            scaleTaxRate = self.scaleTaxRate
            if scaleTaxRate.get_value() != taxRate:
                scaleTaxRate.set_value(taxRate)
            label = str(taxRate) + '%'
            self.labelTaxRate2.set_text(label)

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
            policeGotString = formatMoney(int(fireFund * policePercent))

            roadWantString = formatMoney(roadFund)
            roadPercentString = formatPercent(roadPercent)
            roadGotString = formatMoney(int(roadFund * roadPercent))

            self.labelBudget1.set_text('Taxes Collected: ' + collectedString)
            self.labelBudget2.set_text('Cash Flow: ' + cashFlowString)
            self.labelBudget3.set_text('Previous Funds: ' + previousString)
            self.labelBudget4.set_text('Current Funds: ' + currentString)

            self.labelFireRequest.set_text('Request: ' + fireWantString)
            self.labelFirePercent.set_text(firePercentString + ' = ' + fireGotString)
            value = int(firePercent * 100.0)
            if self.scaleFirePercent.get_value() != value:
                self.scaleFirePercent.set_value(value)

            self.labelPoliceRequest.set_text('Request: ' + policeWantString)
            self.labelPolicePercent.set_text(policePercentString + ' = ' + policeGotString)
            value = int(policePercent * 100.0)
            if self.scalePolicePercent.get_value() != value:
                self.scalePolicePercent.set_value(value)

            self.labelRoadRequest.set_text('Request: ' + roadWantString)
            self.labelRoadPercent.set_text(roadPercentString + ' = ' + roadGotString)
            value = int(roadPercent * 100.0)
            if self.scaleRoadPercent.get_value() != value:
                self.scaleRoadPercent.set_value(value)


    def taxScaleChanged(self, scale):
        engine = self.engine
        tax = int(scale.get_value())
        if tax != engine.cityTax:
            engine.setCityTax(tax)


    def fireScaleChanged(self, scale):
        engine = self.engine
        fire = scale.get_value() / 100.0
        engine.firePercent = fire
        engine.fireSpend = int(fire * engine.fireFund)
        engine.updateFundEffects()
        engine.mustDrawBudget = True


    def policeScaleChanged(self, scale):
        engine = self.engine
        police = scale.get_value() / 100.0
        engine.policePercent = police
        engine.policeSpend = int(police * engine.policeFund)
        engine.updateFundEffects()
        engine.mustDrawBudget = True


    def roadScaleChanged(self, scale):
        engine = self.engine
        road = scale.get_value() / 100.0
        engine.roadPercent = road
        engine.roadSpend = int(road * engine.roadFund)
        engine.updateFundEffects()
        engine.mustDrawBudget = True


########################################################################
