# micropolisgaugeview.py
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
# Micropolis Gauge View
# Don Hopkins


########################################################################
# Import stuff


import gtk
import cairo
import pango
import micropolisengine
import micropolisview


########################################################################
# MicropolisGaugeView


class MicropolisGaugeView(micropolisview.MicropolisView):


    gap = 5

    barHeight = 20

    colWidth = 10
    colHeight = 25
    colGap = 5

    viewWidth = 150
    viewHeight = (2 * gap) + (2 * colHeight) + barHeight


    def __init__(
        self,
        **args):

        micropolisview.MicropolisView.__init__(
            self,
            interests=('demand', 'date', 'funds',),
            **args)

        self.zoomable = False

        self.set_size_request(self.viewWidth, self.viewHeight)


    def update(
        self,
        name,
        *args):

        #print "GAUGE VIEW UPDATE", self, name, args

        self.queue_draw()


    # TODO: Internationalize
    def getMonthName(self, monthIndex):
        return [
            "Jan",
            "Feb",
            "Mar",
            "Apr",
            "May",
            "Jun",
            "Jul",
            "Aug",
            "Sep",
            "Oct",
            "Nov",
            "Dec",
        ][monthIndex];


    def getCityDate(self):
        engine = self.engine
        return (
            self.getMonthName(engine.cityMonth) +
            ' ' +
            str(engine.cityYear))


    def drawContent(
        self,
        ctx,
        playout):

        #print "==== MicropolisGaugeView DRAWCONTENT", self

        engine = self.engine

        # Measure window.

        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        # Draw window.

        ctx.save()

        playout.set_font_description(self.labelFont)

        # Draw background.

        ctx.rectangle(0, 0, winWidth, winHeight)
        ctx.set_source_rgb(0.9, 0.9, 1.0)
        ctx.clip_preserve()
        ctx.fill_preserve()
        ctx.set_source_rgb(0.0, 0.0, 0.0)
        ctx.set_line_width(2)
        ctx.stroke()

        # Measure bar.

        colWidth = self.colWidth
        colHeight = self.colHeight
        colGap = self.colGap

        gap = self.gap
        barHeight = self.barHeight
        barWidth = (4 * colGap) + (3 * colWidth)
        barX = gap
        barY = gap + colHeight

        # Measure columns.

        rColX = barX + colGap
        cColX = rColX + colWidth + colGap
        iColX = cColX + colWidth + colGap

        resDemand, comDemand, indDemand = engine.getDemands()
        #print "RES", resDemand, "COM", comDemand, "IND", indDemand

        res = -resDemand * colHeight
        com = -comDemand * colHeight
        ind = -indDemand * colHeight

        # Draw columns.

        ctx.set_line_width(2)

        if res != 0:
            if res < 0:
                y = barY + res
                h = -res
            else:
                y = barY + barHeight
                h = res

            ctx.rectangle(rColX, y, colWidth, h)
            ctx.set_source_rgb(0.0, 0.0, 0.0) # Black.
            ctx.stroke_preserve()
            ctx.set_source_rgb(0.0, 1.0, 0.0) # Green.
            ctx.fill()

        if com != 0:
            if com < 0:
                y = barY + com
                h = -com
            else:
                y = barY + barHeight
                h = com

            ctx.rectangle(cColX, y, colWidth, h)
            ctx.set_source_rgb(0.0, 0.0, 0.0) # Black.
            ctx.stroke_preserve()
            ctx.set_source_rgb(0.0, 0.0, 1.0) # Blue.
            ctx.fill()

        if ind != 0:
            if ind < 0:
                y = barY + ind
                h = -ind
            else:
                y = barY + barHeight
                h = ind

            ctx.rectangle(iColX, y, colWidth, h)
            ctx.set_source_rgb(0.0, 0.0, 0.0) # Black.
            ctx.stroke_preserve()
            ctx.set_source_rgb(1.0, 1.0, 0.0) # Yellow.
            ctx.fill()

        # Draw bar.

        ctx.save()

        ctx.rectangle(barX, barY, barWidth, barHeight)
        ctx.set_source_rgb(1.0, 1.0, 1.0)
        ctx.clip_preserve()
        ctx.fill_preserve()
        ctx.set_source_rgb(0.0, 0.0, 0.0)
        ctx.set_line_width(2)
        ctx.stroke()
        ctx.restore()

        # Draw text on bar.

        def centerText(text, x, y):
            textX, textY = self.pinMarkupXY(
                text, x, y, 0.5, 0.5, playout)
            ctx.move_to(textX, textY)
            ctx.show_layout(playout)

        ctx.set_source_rgb(0.0, 0.0, 0.0)

        cy = barY + (barHeight / 2) + 1
        centerText('<b>R</b>', rColX + (colWidth / 2), cy)
        centerText('<b>C</b>', cColX + (colWidth / 2), cy)
        centerText('<b>I</b>', iColX + (colWidth / 2), cy)

        # Finish drawing gauge.

        ctx.restore()

        # Draw text.

        ctx.save()

        textX = barX + barWidth + gap
        textY = gap
        textWidth = winWidth - (textX + gap)
        textHeight = winHeight - (textY + gap)

        ctx.translate(textX, textY)

        markup = (
            "<span><b>Date:</b>\n%s\n\n<b>Funds:</b>\n%s\n</span>" % (
            self.getCityDate(),
            '$' + engine.formatNumber(engine.totalFunds),
        ))

        ctx.set_source_rgb(0.0, 0.0, 0.0)
        playout.set_markup(markup)
        ctx.move_to(0, 0)
        ctx.show_layout(playout)

        # Finish drawing window.

        ctx.restore()


    def handleMouseDrag(
        self,
        event):

        pass


    def handleMousePoint(
        self,
        event):

        pass


    def handleButtonRelease(
        self,
        widget,
        event):

        pass


########################################################################
