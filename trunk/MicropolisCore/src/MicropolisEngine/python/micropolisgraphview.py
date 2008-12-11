# micropolisgraphview.py
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
# Micropolis Graph View
# Don Hopkins


########################################################################
# Import stuff


import gtk
import cairo
import pango
import micropolisengine
import micropolisview


########################################################################
# MicropolisGraphView


class MicropolisGraphView(micropolisview.MicropolisView):


    graphColors = (
        (0.00, 1.00, 0.00), # LIGHTGREEN
        (0.00, 0.00, 0.50), # DARKBLUE
        (1.00, 1.00, 0.00), # YELLOW
        (0.00, 0.60, 0.00), # DARKGREEN
        (1.00, 0.00, 0.00), # RED
        (0.33, 0.42, 0.18), # OLIVE
    )

    graphLegends = (
        "Residential",
        "Commercial",
        "Industrial",
        "Cash Flow",
        "Crime",
        "Pollution",
    )

    graphLegendWidth = 10
    graphLegendHeight = 60

    topEdgeHeight = 0
    bottomEdgeHeight = 25
    leftEdgeWidth = 50
    rightEdgeWidth = 20
    margin = 3
    legendX = 5
    legendY = 5
    legendRightGap = 2
    legendBottomGap = 5
    scaleTopGap = 0


    def __init__(
        self,
        historyScale=micropolisengine.HISTORY_SCALE_LONG,
        historyTypes=[0, 1, 2, 4, 5],
        **args):

        micropolisview.MicropolisView.__init__(
            self,
            aspect='graph',
            interests=('city', 'graph',),
            **args)

        self.historyScale = historyScale
        self.historyTypes = historyTypes


    def update(
        self,
        name,
        *args):

        print "GRAPH UPDATE", self, name, args

        self.queue_draw()


    def drawContent(
        self,
        ctx):

        #print "==== MicropolisGraphView DRAWCONTENT", self

        engine = self.engine

        getHistory = engine.GetHistory
        getHistoryRange = engine.GetHistoryRange

        historyCount = micropolisengine.HISTORY_COUNT
        historyTypeCount = micropolisengine.HISTORY_TYPE_COUNT
        historyScaleCount = micropolisengine.HISTORY_SCALE_COUNT

        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        historyScale = self.historyScale
        historyTypes = self.historyTypes

        histX = 0
        histY = 0
        histWidth = winWidth
        histHeight = winHeight

        histX += self.margin
        histY += self.margin
        histWidth -= 2 * self.margin
        histHeight -= 2 * self.margin

        histY += self.topEdgeHeight
        histHeight -= self.topEdgeHeight + self.bottomEdgeHeight
        histHeight = max(1, histHeight)

        histX += self.leftEdgeWidth
        histWidth -= self.leftEdgeWidth + self.rightEdgeWidth
        histWidth = max(1, histWidth)

        ctx.save()

        ctx.rectangle(
            histX,
            histY,
            histWidth,
            histHeight)

        ctx.set_source_rgb(
            1.0, 1.0, 1.0)

        ctx.fill()

        # Draw legend.

        ctx.save()

        graphLegends = self.graphLegends

        legendWidth = self.leftEdgeWidth - (self.legendX + self.legendRightGap)
        legendHeight = winHeight - (self.legendY + self.bottomEdgeHeight + self.legendBottomGap)

        ctx.translate(
            self.legendX,
            self.legendY)

        h = float(legendHeight) / len(graphLegends)

        for i in range(0, len(graphLegends)):

            ctx.save()

            rgb = self.graphColors[i]

            ctx.rectangle(
                1,
                (i * h) + 1,
                legendWidth - 2,
                h - 2)

            ctx.set_source_rgb(
                *rgb)

            ctx.fill_preserve()

            if i in historyTypes:
                rgb = (0, 0, 0)
                lineWidth = 6
            else:
                rgb = (0.5, 0.5, 0.5)
                lineWidth = 6
                
            ctx.set_line_width(
                lineWidth)

            ctx.set_source_rgb(
                *rgb)

            ctx.clip_preserve()

            ctx.stroke()

            ctx.restore()

        ctx.restore()

        # Draw scale.

        ctx.save()

        scaleX = self.legendX
        scaleY = winHeight - self.bottomEdgeHeight
        scaleWidth = legendWidth
        scaleHeight = winHeight - (scaleY + self.legendBottomGap)

        ctx.translate(
            scaleX,
            scaleY)

        ctx.rectangle(
            0,
            0,
            scaleWidth,
            scaleHeight)

        if historyScale == micropolisengine.HISTORY_SCALE_SHORT:
            rgb = (0.5, 1, 0.5)
        elif historyScale == micropolisengine.HISTORY_SCALE_LONG:
            rgb = (0.5, 0.5, 1)
        else:
            print "Invalid historyScale:", historyScale
            rgb = (1, 0, 0)

        ctx.set_source_rgb(
            *rgb)

        ctx.fill_preserve()

        ctx.set_source_rgb(
            *self.strokeColor)

        ctx.set_line_width(2)

        ctx.stroke()

        ctx.restore()

        # Draw histories.

        ctx.save()

        ctx.translate(
            histX,
            histY)

        for historyType in historyTypes:

            historyMin, historyMax = getHistoryRange(
                historyType,
                historyScale)

            historyRange = historyMax - historyMin
            if historyRange == 0:
                historyRange = 1

            for historyIndex in range(historyCount - 1, -1, -1):

                val = getHistory(
                    historyType,
                    historyScale,
                    historyIndex)

                val -= historyMin

                x = (
                    histWidth -
                    (float(historyIndex) * (float(histWidth) / float(historyCount - 1))))
                #print historyIndex, x, histWidth

                y = (
                    (float(val) * (float(histHeight) / float(historyRange))))

                if historyIndex == 0:
                    ctx.move_to(x, y)
                else:
                    ctx.line_to(x, y)

            ctx.line_to(x + self.rightEdgeWidth, y)

            ctx.set_source_rgb(
                *self.graphColors[historyType])

            ctx.stroke()

        ctx.rectangle(
            0,
            0,
            histWidth,
            histHeight)

        ctx.set_source_rgb(
            *self.strokeColor)

        ctx.set_line_width(2)

        ctx.stroke()

        ctx.restore()

        # Draw years.

        cityYear = engine.CityYear
        cityMonth = engine.CityMonth

        ctx.save()

        ctx.translate(
            histX,
            histY)

        ctx.set_line_width(1)

        ctx.set_source_rgb(
            *self.strokeColor)

        sx = float(histWidth) / 120.0

        if historyScale == micropolisengine.HISTORY_SCALE_SHORT:

            r = range(120 - cityMonth, -1, -12)

        elif historyScale == micropolisengine.HISTORY_SCALE_LONG:

            sx /= 10
            past = 10 * (cityYear % 10)
            year = int(cityYear / 10)

            r = range(1200 - past, -1, -120)

        else:

            print "Invalid historyScale:", historyScale
            r = ()

        for i in r:

            x = i * sx

            ctx.move_to(
                x, 0)

            ctx.line_to(
                x, histHeight + self.bottomEdgeHeight)

            ctx.stroke()

        ctx.restore()
    

########################################################################
