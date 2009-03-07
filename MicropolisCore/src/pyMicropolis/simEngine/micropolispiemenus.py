# micropolispiemenus.py
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
# Micropolis Pie Menus.
# By Don Hopkins.


########################################################################


from pyMicropolis.piemenu import piemenu
import gtk


########################################################################


def MakePie(setToolAction):

    ########################################################################
    # Make pie menus.


    # top
    topPie = piemenu.PieMenu(
        header="Micropolis Menu",
        fixedRadius=50,
        neutralDescription="Select an option.")

    # top/tool
    toolPie = piemenu.PieMenu(
        header="Micropolis Tools",
        fixedRadius=50,
        neutralDescription="Select a Micropolis editing tool,\nor the zone or build submenu.")

    # top/tool/zone
    zonePie = piemenu.PieMenu(
        header="Zone",
        fixedRadius=50,
        neutralDescription="Select a zoning tool.")

    # top/zone/build
    buildPie = piemenu.PieMenu(
        header="Build",
        fixedRadius=50,
        neutralDescription="Select a building tool.")

    # top/control
    controlPie = piemenu.PieMenu(
        header="Control",
        fixedRadius=50,
        neutralDescription="Select a control.")

    # top/foo
    fooPie = piemenu.PieMenu(
        header="Foo",
        fixedRadius=50,
        neutralDescription="Select a foo.")

    # top/view
    viewPie = piemenu.PieMenu(
        header="View",
        fixedRadius=50,
        neutralDescription="Select a view option.")


    ########################################################################
    # Populate pie menus.


    for params in (

        {
            'pie': toolPie,
            'label': 'Zone...',
            'description': "Submenu of zoning tools.",
            'subPie': zonePie,
        },

        {
            'pie': zonePie,
            'description': 'Commercial\nzoning tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/iccom.png',
            'iconHilite': 'images/simEngine/iccomhi.png',
            'action': lambda item: setToolAction('Commercial'),
        },

        {
            'pie': zonePie,
            'description': 'Industrial\nzoning tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icind.png',
            'iconHilite': 'images/simEngine/icindhi.png',
            'action': lambda item: setToolAction('Industrial'),
        },

        {
            'pie': zonePie,
            'description': 'Police station\nzoning tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icpol.png',
            'iconHilite': 'images/simEngine/icpolhi.png',
            'action': lambda item: setToolAction('PoliceStation'),
        },

        {
            'pie': zonePie,
            'description': 'Query zone\ntool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icqry.png',
            'iconHilite': 'images/simEngine/icqryhi.png',
            'action': lambda item: setToolAction('Query'),
        },

        {
            'pie': zonePie,
            'description': 'Fire station\nzoning tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icfire.png',
            'iconHilite': 'images/simEngine/icfirehi.png',
            'action': lambda item: setToolAction('FireStation'),
        },

        {
            'pie': zonePie,
            'description': 'Residential\nzoning tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icres.png',
            'iconHilite': 'images/simEngine/icreshi.png',
            'action': lambda item: setToolAction('Residential'),
        },

        {
            'pie': toolPie,
            'description': 'Bulldozer editing tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icdozr.png',
            'iconHilite': 'images/simEngine/icdozrhi.png',
            'action': lambda item: setToolAction('Bulldozer'),
        },

        {
            'pie': toolPie,
            'description': 'Road editing tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icroad.png',
            'iconHilite': 'images/simEngine/icroadhi.png',
            'action': lambda item: setToolAction('Road'),
        },

        {
            'pie': toolPie,
            'description': 'Eraser drawing tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icersr.png',
            'iconHilite': 'images/simEngine/icersrhi.png',
            'action': lambda item: setToolAction('Eraser'),
        },

        {
            'pie': toolPie,
            'label': 'Build...',
            'description': 'Submenu of building tools.',
            'subPie': buildPie,
        },

        {
            'pie': buildPie,
            'description': 'Park building tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icpark.png',
            'iconHilite': 'images/simEngine/icparkhi.png',
            'action': lambda item: setToolAction('Park'),
        },

        {
            'pie': buildPie,
            'description': 'Forest building tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'linear': True,
            'linearDirection': 'n',
            'icon': 'images/simEngine/icpark.png',
            'iconHilite': 'images/simEngine/icparkhi.png',
            'action': lambda item: setToolAction('Forest'),
        },

        {
            'pie': buildPie,
            'description': 'Land building tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'linear': True,
            'linearDirection': 'n',
            'icon': 'images/simEngine/icpark.png',
            'iconHilite': 'images/simEngine/icparkhi.png',
            'action': lambda item: setToolAction('Land'),
        },

        {
            'pie': buildPie,
            'description': 'Water building tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'linear': True,
            'linearDirection': 'n',
            'icon': 'images/simEngine/icpark.png',
            'iconHilite': 'images/simEngine/icparkhi.png',
            'action': lambda item: setToolAction('Water'),
        },

        {
            'pie': buildPie,
            'description': 'Seaport building tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icseap.png',
            'iconHilite': 'images/simEngine/icseaphi.png',
            'action': lambda item: setToolAction('Seaport'),
        },

        {
            'pie': buildPie,
            'description': 'Nuclear power plant\nbuilding tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icnuc.png',
            'iconHilite': 'images/simEngine/icnuchi.png',
            'action': lambda item: setToolAction('NuclearPowerPlant'),
        },

        {
            'pie': buildPie,
            'description': 'Airport building tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icairp.png',
            'iconHilite': 'images/simEngine/icairphi.png',
            'action': lambda item: setToolAction('Airport'),
        },

        {
            'pie': buildPie,
            'description': 'Coal power plant\nbuilding tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/iccoal.png',
            'iconHilite': 'images/simEngine/iccoalhi.png',
            'action': lambda item: setToolAction('CoalPowerPlant'),
        },

        {
            'pie': buildPie,
            'description': 'Stadium building tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icstad.png',
            'iconHilite': 'images/simEngine/icstadhi.png',
            'action': lambda item: setToolAction('Stadium'),
        },

        {
            'pie': toolPie,
            'description': 'Chalk drawing tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icchlk.png',
            'iconHilite': 'images/simEngine/icchlkhi.png',
            'action': lambda item: setToolAction('Chalk'),
        },

        {
            'pie': toolPie,
            'description': 'Rail editing tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icrail.png',
            'iconHilite': 'images/simEngine/icrailhi.png',
            'action': lambda item: setToolAction('Rail'),
        },

        {
            'pie': toolPie,
            'description': 'Wire editing tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icwire.png',
            'iconHilite': 'images/simEngine/icwirehi.png',
            'action': lambda item: setToolAction('Wire'),
        },

    ):

        apply(
            piemenu.PieItem,
            (),
            params)


    #return topPie
    return toolPie


########################################################################


def NewMakePie(setToolAction):

    ########################################################################
    # Make pie menus.


    toolPie = piemenu.PieMenu(
        header="Micropolis Tools",
        fixedRadius=25,
        ringRadius=70,
        maxPieItems=(8, 8, 16,),
        neutralDescription="Select a Micropolis editing tool,\nor the zone or build submenu.")


    ########################################################################
    # Populate pie menus.


    for params in (

        # Inner Ring

        { # North
            'pie': toolPie,
            'description': 'Commercial\nzoning tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/iccom.png',
            'iconHilite': 'images/simEngine/iccomhi.png',
            'action': lambda item: setToolAction('Commercial'),
        },

        { # NorthEast
            'pie': toolPie,
            'description': 'Industrial\nzoning tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icind.png',
            'iconHilite': 'images/simEngine/icindhi.png',
            'action': lambda item: setToolAction('Industrial'),
        },

        { # East
            'pie': toolPie,
            'description': 'Road editing tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icroad.png',
            'iconHilite': 'images/simEngine/icroadhi.png',
            'action': lambda item: setToolAction('Road'),
        },

        { # SouthEast
            'pie':pie,
            'description': 'Park building tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icpark.png',
            'iconHilite': 'images/simEngine/icparkhi.png',
            'action': lambda item: setToolAction('Park'),
        },

        { # South
            'pie': toolPie,
            'description': 'Bulldozer editing tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icdozr.png',
            'iconHilite': 'images/simEngine/icdozrhi.png',
            'action': lambda item: setToolAction('Bulldozer'),
        },

        { # SoutWest
            'pie': toolPie,
            'description': 'Wire editing tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icwire.png',
            'iconHilite': 'images/simEngine/icwirehi.png',
            'action': lambda item: setToolAction('Wire'),
        },

        { # West
            'pie': toolPie,
            'description': 'Rail editing tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icrail.png',
            'iconHilite': 'images/simEngine/icrailhi.png',
            'action': lambda item: setToolAction('Rail'),
        },

        { # NorthWest
            'pie': toolPie,
            'description': 'Residential\nzoning tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icres.png',
            'iconHilite': 'images/simEngine/icreshi.png',
            'action': lambda item: setToolAction('Residential'),
        },

        # Outer Ring

        { # North
            'pie': toolPie,
            'description': 'Query zone\ntool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icqry.png',
            'iconHilite': 'images/simEngine/icqryhi.png',
            'action': lambda item: setToolAction('Query'),
        },

        { # NorthWest
            'pie': toolPie,
            'description': 'Police station\nzoning tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icpol.png',
            'iconHilite': 'images/simEngine/icpolhi.png',
            'action': lambda item: setToolAction('PoliceStation'),
        },

        { # West
            'pie': toolPie,
            'description': 'Seaport building tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icseap.png',
            'iconHilite': 'images/simEngine/icseaphi.png',
            'action': lambda item: setToolAction('Seaport'),
        },

        { # SouthWest
            'pie': toolPie,
            'description': 'Nuclear power plant\nbuilding tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icnuc.png',
            'iconHilite': 'images/simEngine/icnuchi.png',
            'action': lambda item: setToolAction('NuclearPowerPlant'),
        },

        { # South
            'pie': toolPie,
            'description': 'Airport building tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icairp.png',
            'iconHilite': 'images/simEngine/icairphi.png',
            'action': lambda item: setToolAction('Airport'),
        },

        { # SouthEast
            'pie': toolPie,
            'description': 'Coal power plant\nbuilding tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/iccoal.png',
            'iconHilite': 'images/simEngine/iccoalhi.png',
            'action': lambda item: setToolAction('CoalPowerPlant'),
        },

        { # East
            'pie': toolPie,
            'description': 'Stadium building tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icstad.png',
            'iconHilite': 'images/simEngine/icstadhi.png',
            'action': lambda item: setToolAction('Stadium'),
        },

        { # NorthEast
            'pie': toolPie,
            'description': 'Fire station\nzoning tool.',
            'loliteFillColor': None,
            'loliteStrokeColor': None,
            'icon': 'images/simEngine/icfire.png',
            'iconHilite': 'images/simEngine/icfirehi.png',
            'action': lambda item: setToolAction('FireStation'),
        },

    ):

        apply(
            piemenu.PieItem,
            (),
            params)


    return toolPie


########################################################################


def main():

    ########################################################################
    # Make Window and PieMenuTarget.


    win = gtk.Window()
    win.set_title("Micropolis Pie Menus, by Don Hopkins")

    target = piemenu.PieMenuTarget(label="Micropolis Pie Menus")

    win.add(target)

    def setToolAction(toolName):
        print "setToolAction", toolName

    pie = MakePie(setToolAction)
    target.setPie(pie)

    win.resize(300, 300)

    win.show_all()

    gtk.main()


########################################################################


if __name__ == '__main__':
    main()


########################################################################
