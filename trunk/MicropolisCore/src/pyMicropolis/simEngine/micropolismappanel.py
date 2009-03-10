# micropolismappanel.py
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
# Micropolis Map Panel
# Don Hopkins


########################################################################
# Import stuff


import gtk
import cairo
import pango
import micropolisengine
import micropolisview


########################################################################
# MicropolisMapPanel


class MicropolisMapPanel(gtk.Frame):


    def __init__(
        self,
        engine=None,
        mapViews=None,
        **args):

        gtk.Frame.__init__(
            self,
            **args)

        self.engine = engine
        self.mapViews = mapViews

        # Views

        vbox0 = gtk.VBox(False, 5)
        self.vbox0 = vbox0
        self.add(vbox0)

        spacer1 = gtk.HBox(False, 5)
        spacer1.set_size_request(5, 5)
        vbox0.pack_start(spacer1, False, False, 0)

        panelLabel = gtk.Label('Zone Filters and Overlays')
        self.panelLabel = panelLabel
        vbox0.pack_start(panelLabel, False, False, 0)

        hbox1 = gtk.HBox(False, 0)
        self.hbox1 = hbox1
        vbox0.pack_start(hbox1, False, False, 0)

        vbox1 = gtk.VBox(False, 0)
        self.vbox1 = vbox1
        hbox1.pack_start(vbox1, False, False, 0)

        radioAll = gtk.RadioButton(None, 'All Zones')
        self.radioAll = radioAll
        radioAll.connect('clicked', self.setMapStyle, 'all')
        vbox1.pack_start(radioAll, False, False, 0)

        radioResidential = gtk.RadioButton(radioAll, 'Residential Zones')
        self.radioResidential = radioResidential
        radioResidential.connect('clicked', self.setMapStyle, 'residential')
        vbox1.pack_start(radioResidential, False, False, 0)

        radioCommercial = gtk.RadioButton(radioAll, 'Commercial Zones')
        self.radioCommercial = radioCommercial
        radioCommercial.connect('clicked', self.setMapStyle, 'commercial')
        vbox1.pack_start(radioCommercial, False, False, 0)

        radioIndustrial = gtk.RadioButton(radioAll, 'Industrial Zones')
        self.radioIndustrial = radioIndustrial
        radioIndustrial.connect('clicked', self.setMapStyle, 'industrial')
        vbox1.pack_start(radioIndustrial, False, False, 0)

        radioTransportation = gtk.RadioButton(radioAll, 'Transportation Zones')
        self.radioTransportation = radioTransportation
        radioTransportation.connect('clicked', self.setMapStyle, 'transportation')
        vbox1.pack_start(radioTransportation, False, False, 0)

        vbox2 = gtk.VBox(False, 0)
        self.vbox2 = vbox2
        hbox1.pack_start(vbox2, False, False, 0)

        radioPopulationDensity = gtk.RadioButton(radioAll, 'Population Density')
        self.radioPopulationDensity = radioPopulationDensity
        radioPopulationDensity.connect('clicked', self.setMapStyle, 'populationdensity')
        vbox2.pack_start(radioPopulationDensity, False, False, 0)

        radioRateOfGrowth = gtk.RadioButton(radioAll, 'Rate of Growth')
        self.radioRateOfGrowth = radioRateOfGrowth
        radioRateOfGrowth.connect('clicked', self.setMapStyle, 'rateofgrowth')
        vbox2.pack_start(radioRateOfGrowth, False, False, 0)

        radioLandValue = gtk.RadioButton(radioAll, 'Land Value')
        self.radioLandValue = radioLandValue
        radioLandValue.connect('clicked', self.setMapStyle, 'landvalue')
        vbox2.pack_start(radioLandValue, False, False, 0)

        radioCrimeRate = gtk.RadioButton(radioAll, 'Crime Rate')
        self.radioCrimeRate = radioCrimeRate
        radioCrimeRate.connect('clicked', self.setMapStyle, 'crimerate')
        vbox2.pack_start(radioCrimeRate, False, False, 0)

        radioPollutionDensity = gtk.RadioButton(radioAll, 'Pollution Density')
        self.radioPollutionDensity = radioPollutionDensity
        radioPollutionDensity.connect('clicked', self.setMapStyle, 'pollutiondensity')
        vbox2.pack_start(radioPollutionDensity, False, False, 0)

        vbox3 = gtk.VBox(False, 0)
        self.vbox3 = vbox3
        hbox1.pack_start(vbox3, False, False, 0)

        radioTrafficDensity = gtk.RadioButton(radioAll, 'Traffic Density')
        self.radioTrafficDensity = radioTrafficDensity
        radioTrafficDensity.connect('clicked', self.setMapStyle, 'trafficdensity')
        vbox3.pack_start(radioTrafficDensity, False, False, 0)

        radioPowerGrid = gtk.RadioButton(radioAll, 'Power Grid')
        self.radioPowerGrid = radioPowerGrid
        radioPowerGrid.connect('clicked', self.setMapStyle, 'powergrid')
        vbox3.pack_start(radioPowerGrid, False, False, 0)

        radioFireCoverage = gtk.RadioButton(radioAll, 'Fire Coverage')
        self.radioFireCoverage = radioFireCoverage
        radioFireCoverage.connect('clicked', self.setMapStyle, 'firecoverage')
        vbox3.pack_start(radioFireCoverage, False, False, 0)

        radioPoliceCoverage = gtk.RadioButton(radioAll, 'Police Coverage')
        self.radioPoliceCoverage = radioPoliceCoverage
        radioPoliceCoverage.connect('clicked', self.setMapStyle, 'policecoverage')
        vbox3.pack_start(radioPoliceCoverage, False, False, 0)


    def setMapStyle(self, widget, mapStyle):
        mapViews = self.mapViews
        if not mapViews:
            return

        for mapView in mapViews:
            mapView.setMapStyle(mapStyle)


########################################################################
