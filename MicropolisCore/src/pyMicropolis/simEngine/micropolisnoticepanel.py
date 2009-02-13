# micropolisnoticepanel.py
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
import micropolisnoticeview
import micropolisdrawingarea

########################################################################
# MicropolisNoticePanel


class MicropolisNoticePanel(gtk.Frame):


    def __init__(
        self,
        engine=None,
        centerOnTileHandler=None,
        **args):

        gtk.Frame.__init__(
            self,
            **args)

        self.engine = engine
        self.cityViewVisible = False

        # Views

        hpaned1 = gtk.HPaned()
        self.hpaned1 = hpaned1
        self.add(hpaned1)

        self.noticeView = micropolisnoticeview.MicropolisNoticeView(
            engine=engine,
            setCityViewVisible=self.setCityViewVisible)

        hpaned1.pack1(self.noticeView, resize=False, shrink=False)

        cityView = micropolisdrawingarea.NoticeMicropolisDrawingArea(
            engine=engine,
            centerOnTileHandler=centerOnTileHandler)
        self.cityView = cityView
        cityView.set_size_request(150, -1)
        cityView.visible = False
        hpaned1.pack2(cityView, resize=False, shrink=False)

        hpaned1.set_position(1000)

    def setCityViewVisible(self, visible, tileX=-1, tileY=-1, sprite=micropolisengine.SPRITE_NOTUSED):
        #print "setCityViewVisible", visible, tileX, tileY, self.cityViewVisible
        engine = self.engine
        cityView = self.cityView
        if visible and (tileX >= 0) and (tileY >= 0):
            cityView.centerOnTile(tileX, tileY)
            cityView.sprite = sprite
        if self.cityViewVisible == visible:
            return
        if visible:
            self.cityView.set_property("visible", True)
            engine.addView(cityView)
        else:
            self.cityView.set_property("visible", False)
            engine.removeView(cityView)
        self.cityViewVisible = visible


########################################################################
