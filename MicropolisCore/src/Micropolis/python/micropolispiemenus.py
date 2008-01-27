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


import piemenu
import gtk


########################################################################


def MakeToolPie():


    ########################################################################
    # Make zone_pie.


    zone_pie = piemenu.PieMenu(
        header="Zone",
        fixed_radius=50,
        neutral_description="Select a zoning tool.")

    zone_pie.add_item(
        piemenu.PieItem(
            description='Commercial\nzoning tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/iccom.png',
            icon_hilite='images/iccomhi.png'))

    zone_pie.add_item(
        piemenu.PieItem(
            description='Industrial\nzoning tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icind.png',
            icon_hilite='images/icindhi.png'))

    zone_pie.add_item(
        piemenu.PieItem(
            description='Police station\nzoning tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icpol.png',
            icon_hilite='images/icpolhi.png'))

    zone_pie.add_item(
        piemenu.PieItem(
            description='Query zone\ntool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icqry.png',
            icon_hilite='images/icqryhi.png'))

    zone_pie.add_item(
        piemenu.PieItem(
            description='Fire station\nzoning tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icfire.png',
            icon_hilite='images/icfirehi.png'))

    zone_pie.add_item(
        piemenu.PieItem(
            description='Residential\nzoning tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icres.png',
            icon_hilite='images/icreshi.png'))

    ########################################################################
    # Make build_pie.


    build_pie = piemenu.PieMenu(
        header="Build",
        fixed_radius=50,
        neutral_description="Select a building tool.")

    build_pie.add_item(
        piemenu.PieItem(
            description='Park building tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icpark.png',
            icon_hilite='images/icparkhi.png'))

    build_pie.add_item(
        piemenu.PieItem(
            description='Seaport building tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icseap.png',
            icon_hilite='images/icseaphi.png'))

    build_pie.add_item(
        piemenu.PieItem(
            description='Nuclear power plant\nbuilding tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icnuc.png',
            icon_hilite='images/icnuchi.png'))

    build_pie.add_item(
        piemenu.PieItem(
            description='Airport building tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icairp.png',
            icon_hilite='images/icairphi.png'))

    build_pie.add_item(
        piemenu.PieItem(
            description='Coal power plant\nbuilding tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/iccoal.png',
            icon_hilite='images/iccoalhi.png'))

    build_pie.add_item(
        piemenu.PieItem(
            description='Stadium building tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icstad.png',
            icon_hilite='images/icstadhi.png'))


    ########################################################################
    # Make tool_pie.


    tool_pie = piemenu.PieMenu(
        header="Micropolis Tools",
        fixed_radius=50,
        neutral_description="Select a Micropolis editing tool,\nor the zone or build submenu.")

    tool_pie.add_item(
        piemenu.PieItem(
            label='Zone...',
            description="Submenu of zoning tools.",
            lolite_fill_color=None,
            lolite_stroke_color=None,
            sub_pie=zone_pie))

    tool_pie.add_item(
        piemenu.PieItem(
            description='Bulldozer editing tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icdozr.png',
            icon_hilite='images/icdozrhi.png'))

    tool_pie.add_item(
        piemenu.PieItem(
            description='Road editing tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icroad.png',
            icon_hilite='images/icroadhi.png'))

    tool_pie.add_item(
        piemenu.PieItem(
            description='Eraser drawing tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icersr.png',
            icon_hilite='images/icersrhi.png'))

    tool_pie.add_item(
        piemenu.PieItem(
            label='Build...',
            description='Submenu of building tools.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            sub_pie=build_pie))

    tool_pie.add_item(
        piemenu.PieItem(
            description='Chalk drawing tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icchlk.png',
            icon_hilite='images/icchlkhi.png'))

    tool_pie.add_item(
        piemenu.PieItem(
            description='Rail editing tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icrail.png',
            icon_hilite='images/icrailhi.png'))

    tool_pie.add_item(
        piemenu.PieItem(
            description='Wire editing tool.',
            lolite_fill_color=None,
            lolite_stroke_color=None,
            icon='images/icwire.png',
            icon_hilite='images/icwirehi.png'))


    ########################################################################


    return tool_pie


def main():

    ########################################################################
    # Make Window and PieMenuTarget.


    win = gtk.Window()
    win.set_title("Micropolis Pie Menus, by Don Hopkins")

    target = piemenu.PieMenuTarget(label="Micropolis Pie Menus")

    win.add(target)

    pie = MakeToolPie()
    target.setPie(pie)

    win.resize(300, 300)

    win.show_all()

    gtk.main()


########################################################################


if __name__ == '__main__':
    main()


########################################################################
