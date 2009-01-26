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


from pyMicropolis.tileEngine import piemenu
import gtk


########################################################################


def OldMakeToolPie(setToolAction):

    ########################################################################
    # Make pie menus.


    # top
    top_pie = piemenu.PieMenu(
        header="Micropolis Menu",
        fixed_radius=50,
        neutral_description="Select an option.")

    # top zone
    zone_pie = piemenu.PieMenu(
        header="Zone",
        fixed_radius=50,
        neutral_description="Select a zoning tool.")

    # top zone build
    build_pie = piemenu.PieMenu(
        header="Build",
        fixed_radius=50,
        neutral_description="Select a building tool.")

    # top zone tool
    tool_pie = piemenu.PieMenu(
        header="Micropolis Tools",
        fixed_radius=50,
        neutral_description="Select a Micropolis editing tool,\nor the zone or build submenu.")

    view_pie = piemenu.PieMenu(
        header="View",
        fixed_radius=50,
        neutral_description="Select a view option.")

    control_pie = piemenu.PieMenu(
        header="Control",
        fixed_radius=50,
        neutral_description="Select a control.")

    foo_pie = piemenu.PieMenu(
        header="Foo",
        fixed_radius=50,
        neutral_description="Select a foo.")


    ########################################################################
    # Populate pie menus.


    for params in (

        {
            'pie': top_pie,
            'label': 'Zone...',
            'description': 'Submenu of zoning options.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'sub_pie': zone_pie,
        },

        {
            'pie': tool_pie,
            'label': 'Zone...',
            'description': "Submenu of zoning tools.",
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'sub_pie': zone_pie,
        },

        {
            'pie': zone_pie,
            'description': 'Commercial\nzoning tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/iccom.png',
            'icon_hilite': 'images/simEngine/iccomhi.png',
            'action': lambda item: setToolAction('Commercial'),
        },

        {
            'pie': zone_pie,
            'description': 'Industrial\nzoning tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icind.png',
            'icon_hilite': 'images/simEngine/icindhi.png',
            'action': lambda item: setToolAction('Industrial'),
        },

        {
            'pie': zone_pie,
            'description': 'Police station\nzoning tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icpol.png',
            'icon_hilite': 'images/simEngine/icpolhi.png',
            'action': lambda item: setToolAction('PoliceStation'),
        },

        {
            'pie': zone_pie,
            'description': 'Query zone\ntool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icqry.png',
            'icon_hilite': 'images/simEngine/icqryhi.png',
            'action': lambda item: setToolAction('Query'),
        },

        {
            'pie': zone_pie,
            'description': 'Fire station\nzoning tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icfire.png',
            'icon_hilite': 'images/simEngine/icfirehi.png',
            'action': lambda item: setToolAction('FireStation'),
        },

        {
            'pie': zone_pie,
            'description': 'Residential\nzoning tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icres.png',
            'icon_hilite': 'images/simEngine/icreshi.png',
            'action': lambda item: setToolAction('Residential'),
        },

        {
            'pie': build_pie,
            'description': 'Park building tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icpark.png',
            'icon_hilite': 'images/simEngine/icparkhi.png',
            'action': lambda item: setToolAction('Park'),
        },

        {
            'pie': build_pie,
            'description': 'Seaport building tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icseap.png',
            'icon_hilite': 'images/simEngine/icseaphi.png',
            'action': lambda item: setToolAction('Seaport'),
        },

        {
            'pie': build_pie,
            'description': 'Nuclear power plant\nbuilding tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icnuc.png',
            'icon_hilite': 'images/simEngine/icnuchi.png',
            'action': lambda item: setToolAction('NuclearPowerPlant'),
        },

        {
            'pie': build_pie,
            'description': 'Airport building tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icairp.png',
            'icon_hilite': 'images/simEngine/icairphi.png',
            'action': lambda item: setToolAction('Airport'),
        },

        {
            'pie': build_pie,
            'description': 'Coal power plant\nbuilding tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/iccoal.png',
            'icon_hilite': 'images/simEngine/iccoalhi.png',
            'action': lambda item: setToolAction('CoalPowerPlant'),
        },

        {
            'pie': build_pie,
            'description': 'Stadium building tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icstad.png',
            'icon_hilite': 'images/simEngine/icstadhi.png',
            'action': lambda item: setToolAction('Stadium'),
        },

        {
            'pie': tool_pie,
            'description': 'Bulldozer editing tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icdozr.png',
            'icon_hilite': 'images/simEngine/icdozrhi.png',
            'action': lambda item: setToolAction('Bulldozer'),
        },

        {
            'pie': tool_pie,
            'description': 'Road editing tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icroad.png',
            'icon_hilite': 'images/simEngine/icroadhi.png',
            'action': lambda item: setToolAction('Road'),
        },

        {
            'pie': tool_pie,
            'description': 'Eraser drawing tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icersr.png',
            'icon_hilite': 'images/simEngine/icersrhi.png',
            'action': lambda item: setToolAction('Eraser'),
        },

        {
            'pie': tool_pie,
            'label': 'Build...',
            'description': 'Submenu of building tools.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'sub_pie': build_pie,
        },

        {
            'pie': tool_pie,
            'description': 'Chalk drawing tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icchlk.png',
            'icon_hilite': 'images/simEngine/icchlkhi.png',
            'action': lambda item: setToolAction('Chalk'),
        },

        {
            'pie': tool_pie,
            'description': 'Rail editing tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icrail.png',
            'icon_hilite': 'images/simEngine/icrailhi.png',
            'action': lambda item: setToolAction('Rail'),
        },

        {
            'pie': tool_pie,
            'description': 'Wire editing tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icwire.png',
            'icon_hilite': 'images/simEngine/icwirehi.png',
            'action': lambda item: setToolAction('Wire'),
        },

        {
            'pie': top_pie,
            'label': 'Control...',
            'description': 'Submenu of controls.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'sub_pie': control_pie,
        },

        {
            'pie': top_pie,
            'label': 'Foo...',
            'description': 'Submenu of foo options.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'sub_pie': foo_pie,
        },

        {
            'pie': top_pie,
            'label': 'View...',
            'description': 'Submenu of view options.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'sub_pie': view_pie,
        },

    ):

        apply(
            piemenu.PieItem,
            (),
            params)


    return tool_pie


########################################################################


def MakeToolPie(setToolAction):

    ########################################################################
    # Make pie menus.


    tool_pie = piemenu.PieMenu(
        header="Micropolis Tools",
        fixed_radius=25,
        ring_radius=70,
        max_pie_items=(8, 8, 16),
        neutral_description="Select a Micropolis editing tool,\nor the zone or build submenu.")


    ########################################################################
    # Populate pie menus.


    for params in (

        # Inner Ring

        { # North
            'pie': tool_pie,
            'description': 'Commercial\nzoning tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/iccom.png',
            'icon_hilite': 'images/simEngine/iccomhi.png',
            'action': lambda item: setToolAction('Commercial'),
        },

        { # NorthEast
            'pie': tool_pie,
            'description': 'Industrial\nzoning tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icind.png',
            'icon_hilite': 'images/simEngine/icindhi.png',
            'action': lambda item: setToolAction('Industrial'),
        },

        { # East
            'pie': tool_pie,
            'description': 'Road editing tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icroad.png',
            'icon_hilite': 'images/simEngine/icroadhi.png',
            'action': lambda item: setToolAction('Road'),
        },

        { # SouthEast
            'pie':tool_pie,
            'description': 'Park building tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icpark.png',
            'icon_hilite': 'images/simEngine/icparkhi.png',
            'action': lambda item: setToolAction('Park'),
        },

        { # South
            'pie': tool_pie,
            'description': 'Bulldozer editing tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icdozr.png',
            'icon_hilite': 'images/simEngine/icdozrhi.png',
            'action': lambda item: setToolAction('Bulldozer'),
        },

        { # SoutWest
            'pie': tool_pie,
            'description': 'Wire editing tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icwire.png',
            'icon_hilite': 'images/simEngine/icwirehi.png',
            'action': lambda item: setToolAction('Wire'),
        },

        { # West
            'pie': tool_pie,
            'description': 'Rail editing tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icrail.png',
            'icon_hilite': 'images/simEngine/icrailhi.png',
            'action': lambda item: setToolAction('Rail'),
        },

        { # NorthWest
            'pie': tool_pie,
            'description': 'Residential\nzoning tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icres.png',
            'icon_hilite': 'images/simEngine/icreshi.png',
            'action': lambda item: setToolAction('Residential'),
        },

        # Outer Ring

        { # North
            'pie': tool_pie,
            'description': 'Query zone\ntool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icqry.png',
            'icon_hilite': 'images/simEngine/icqryhi.png',
            'action': lambda item: setToolAction('Query'),
        },

        { # NorthWest
            'pie': tool_pie,
            'description': 'Police station\nzoning tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icpol.png',
            'icon_hilite': 'images/simEngine/icpolhi.png',
            'action': lambda item: setToolAction('PoliceStation'),
        },

        { # West
            'pie':tool_pie,
            'description': 'Seaport building tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icseap.png',
            'icon_hilite': 'images/simEngine/icseaphi.png',
            'action': lambda item: setToolAction('Seaport'),
        },

        { # SouthWest
            'pie':tool_pie,
            'description': 'Nuclear power plant\nbuilding tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icnuc.png',
            'icon_hilite': 'images/simEngine/icnuchi.png',
            'action': lambda item: setToolAction('NuclearPowerPlant'),
        },

        { # South
            'pie':tool_pie,
            'description': 'Airport building tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icairp.png',
            'icon_hilite': 'images/simEngine/icairphi.png',
            'action': lambda item: setToolAction('Airport'),
        },

        { # SouthEast
            'pie':tool_pie,
            'description': 'Coal power plant\nbuilding tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/iccoal.png',
            'icon_hilite': 'images/simEngine/iccoalhi.png',
            'action': lambda item: setToolAction('CoalPowerPlant'),
        },

        { # East
            'pie':tool_pie,
            'description': 'Stadium building tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icstad.png',
            'icon_hilite': 'images/simEngine/icstadhi.png',
            'action': lambda item: setToolAction('Stadium'),
        },

        { # NorthEast
            'pie': tool_pie,
            'description': 'Fire station\nzoning tool.',
            'lolite_fill_color': None,
            'lolite_stroke_color': None,
            'icon': 'images/simEngine/icfire.png',
            'icon_hilite': 'images/simEngine/icfirehi.png',
            'action': lambda item: setToolAction('FireStation'),
        },

    ):

        apply(
            piemenu.PieItem,
            (),
            params)


    return tool_pie


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

    pie = MakeToolPie(setToolAction)
    target.setPie(pie)

    win.resize(300, 300)

    win.show_all()

    gtk.main()


########################################################################


if __name__ == '__main__':
    main()


########################################################################
