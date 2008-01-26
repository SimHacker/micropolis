# micropolisutils.py
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
# Micropolis Utilities
# Don Hopkins


########################################################################
# Import stuff


import sys
import os
import time
import micropolis


########################################################################
# Globals


__version__ = "0.9"


########################################################################
# MicropolisView Class


class MicropolisView:

    def __init__(
            self,
            m=None,
            x=0,
            y=0,
            width=micropolis.WORLD_X,
            height=micropolis.WORLD_Y,
            **args):
        self.m = m
        self.x = x
        self.y = y
        self.width = width
        self.height = height


    def applyToClippedTiles(self, onTilesBegin, onRowBegin, onTile, onRowEnd, onTilesEnd):

        left = max(self.x, 0)
        top = max(self.y, 0)
        right = min(self.x + self.width, micropolis.WORLD_X)
        bottom = min(self.y + self.height, micropolis.WORLD_Y)

        w = right - left
        h = bottom - top

        onTilesBegin(left, top, right, bottom, w, h)

        if (w != 0) and (h != 0):

            getTile = self.m.GetTile
            lowMask = micropolis.LOMASK
            flagsMask = ~lowMask

            for y in range(top, bottom):
                onRowBegin(y, w, h)
                for x in range(left, right):
                    rawTile = getTile(x, y)
                    onTile(x, y, rawTile & lowMask, rawTile & flagsMask)
                onRowEnd(y, w, h)

        onTilesEnd(left, top, right, bottom, w, h)


    def renderSummaryAsHtml(self):

        m = self.m

        out = [
            "<p>\n",
            """<b>Micropolis City Evaluation, %s</b><br/>\n""" % (
                m.CityDate,
            ),
            "Score: %s, Changed: %s, Class: %s, Level: %s, Tax Rate: %s%%<br/>\n" % (
                m.evalScore, m.evalChanged, m.evalCityClass, m.evalCityLevel, m.CityTax,
            ),
            "Demand: Residential: %s, Commercial: %s, Industrial: %s<br/>\n" % (
              m.LastR, m.LastC, m.LastI,
            ),
            "Funds: $%s, Assessed: %s, Population: %s, Delta: %s<br/>\n" % (
              m.TotalFunds, m.evalAssessedDollars, m.evalPop, m.evalDelta,
            ),
            "Survey: Is the Mayor doing a good job? Yes: %s, No: %s<br/>" % (
              m.evalGoodYes, m.evalGoodNo,
            ),
            """Problems: %s: %s; %s: %s; %s: %s; %s: %s<br/>\n""" % (
                m.evalPs0, m.evalPv0, m.evalPs1, m.evalPv1, m.evalPs2, m.evalPv2, m.evalPs3, m.evalPv3, 
            ),
            "</p>\n",
            "<hr/>\n",
        ]

        return "".join(out)
    

    def renderClippedTilesAsHtml(self):

        out = []

        def onTilesBegin(left, top, right, bottom, w, h):

            out.append(
                """<style><!--
.tile {
  width: 16px;
  height: 16px;
  background-image: url(http://localhost/micropolis/tiles/tiles.png)
}
</style>""")

            out.append(
                """<div style="width: %dpx; height: %dpx">\n""" % (
                    w * 16,
                    h * 16,
                ))

            out.append(
                """<table gap="0" cellpadding="0" cellspacing="0"> <!-- Tiles: left %d top %d right %d bottom %d w %d h %d -->\n""" % (
                    left, top, right, bottom, w, h,
                ))

        def onRowBegin(y, w, h):
            out.append(
                """<tr height="16"> <!-- Row %d: --> """ % (
                    y,
                ))

        def onTile(x, y, tile, flags):
            tileUrl = \
                "http://localhost/micropolis/tiles/tile%04d.png" % (
                    tile,
                )
            out.append(
                """<td width="16" class="tile" style="background-position: 0px %dpx">%s</td>""" % (
                    -16 * tile,
                    "", # str(tile),
                ))

        def onRowEnd(y, w, h):
            out.append(
                """<tr/>\n""")

        def onTilesEnd(left, top, right, bottom, w, h):

            out.append(
                """</table>\n""")
        
            out.append(
                """</div>\n""")

        self.applyToClippedTiles(
            onTilesBegin,
            onRowBegin,
            onTile,
            onRowEnd,
            onTilesEnd)
            
        return "".join(out)


########################################################################
