# micropolishistoryview.py
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
# Micropolis History View
# Don Hopkins


########################################################################
# Import stuff


import gtk
import cairo
import pango
import micropolisengine
import micropolisview


########################################################################
# MicropolisHistoryView


class MicropolisHistoryView(micropolisview.MicropolisView):


    historyColors = (
        (0.00, 1.00, 0.00), # Res: LIGHTGREEN
        (0.00, 0.00, 0.50), # Com: DARKBLUE
        (1.00, 1.00, 0.00), # Ind: YELLOW
        (0.00, 0.60, 0.00), # Money: DARKGREEN
        (1.00, 0.00, 0.00), # Crime: RED
        (0.33, 0.42, 0.18), # Pollution: OLIVE
    )

    historyRanges = (
        (0, 768), # Res
        (0, 768), # Com
        (0, 768), # Ind
        (0, 256), # Money
        (0, 128), # Crime
        (0, 128), # Pollution
    )

    historyLegends = (
        "Residential",
        "Commercial",
        "Industrial",
        "Cash Flow",
        "Crime",
        "Pollution",
    )

    topEdgeHeight = 5
    bottomEdgeHeight = 30
    leftEdgeWidth = 100
    rightEdgeWidth = 30
    margin = 5
    gap = 2
    boxGap = 1


    def __init__(
        self,
        historyScale=micropolisengine.HISTORY_SCALE_LONG,
        historyTypes=[0, 1, 2, 3, 4, 5],
        **args):

        micropolisview.MicropolisView.__init__(
            self,
            aspect='graph',
            interests=('graph',),
            **args)

        self.zoomable = False
        self.historyScale = historyScale
        self.historyTypes = historyTypes
        self.hiliteTarget = None


    def update(
        self,
        name,
        *args):

        #print "HISTORY UPDATE", self, name, args

        self.queue_draw()


    def getScaleRect(self):

        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        margin = self.margin
        gap = self.gap

        scaleX = margin
        scaleY = winHeight - self.bottomEdgeHeight + gap
        scaleWidth = self.leftEdgeWidth - (margin + gap)
        scaleHeight = self.bottomEdgeHeight - (margin + gap)

        return scaleX, scaleY, scaleWidth, scaleHeight


    def getHistoryRect(self):

        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        margin = self.margin
        gap = self.gap

        histX = self.leftEdgeWidth + gap
        histY = self.topEdgeHeight + gap
        histWidth = winWidth - (self.leftEdgeWidth + self.rightEdgeWidth + gap + gap)
        histHeight = winHeight - (self.topEdgeHeight + self.bottomEdgeHeight + gap + gap)

        histWidth = max(1, histWidth)
        histHeight = max(1, histHeight)

        return histX, histY, histWidth, histHeight


    def getLegendRect(self):
        
        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        margin = self.margin
        gap = self.gap

        legendX = margin
        legendY = self.topEdgeHeight + gap
        legendWidth = self.leftEdgeWidth - (margin + gap)
        legendHeight = winHeight - (self.topEdgeHeight + self.bottomEdgeHeight + gap + gap)
        legendHeight = max(1, legendHeight)

        return legendX, legendY, legendWidth, legendHeight


    def getLegendBoxRect(self, i):
        
        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        margin = self.margin
        gap = self.gap

        legendX = margin
        legendY = self.topEdgeHeight + gap
        legendWidth = self.leftEdgeWidth - (margin + gap)
        legendHeight = winHeight - (self.topEdgeHeight + self.bottomEdgeHeight + gap + gap)
        legendHeight = max(1, legendHeight)

        h = float(legendHeight) / len(self.historyLegends)

        boxGap = self.boxGap

        boxX = legendX
        boxY = legendY + (i * h)
        boxWidth = legendWidth
        boxHeight = h
        return boxX, boxY, boxWidth, boxHeight


    def findTarget(
        self,
        x,
        y):

        if self.pointInRect(x, y, self.getHistoryRect()):
            return 'history', 0

        if self.pointInRect(x, y, self.getScaleRect()):
            return 'scale', 0

        if self.pointInRect(x, y, self.getLegendRect()):
            for i in range(0, len(self.historyLegends)):
                if self.pointInRect(x, y, self.getLegendBoxRect(i)):
                    return 'legend', i

        return None, None


    def drawContent(
        self,
        ctx,
        playout):

        #print "==== MicropolisHistoryView DRAWCONTENT", self

        engine = self.engine

        getHistory = engine.getHistory
        getHistoryRange = engine.getHistoryRange

        historyCount = micropolisengine.HISTORY_COUNT
        historyTypeCount = micropolisengine.HISTORY_TYPE_COUNT
        historyScaleCount = micropolisengine.HISTORY_SCALE_COUNT

        historyScale = self.historyScale
        historyTypes = self.historyTypes

        histX, histY, histWidth, histHeight = self.getHistoryRect()

        ctx.save()

        ctx.rectangle(
            histX,
            histY,
            histWidth,
            histHeight)

        histY += 2
        histHeight -= 4

        hiliteTarget = self.hiliteTarget

        if hiliteTarget == ('history', 0):
            ctx.set_source_rgb(
                1.0, 1.0, 1.0)
        else:
            ctx.set_source_rgb(
                0.75, 0.75, 0.75)

        ctx.fill()

        ctx.set_source_rgb(
            *self.strokeColor)

        ctx.set_line_width(2)

        ctx.stroke()

        # Draw legend.

        ctx.save()

        playout.set_font_description(self.labelFont)

        historyLegends = self.historyLegends

        for i in range(0, len(historyLegends)):

            boxX, boxY, boxWidth, boxHeight = self.getLegendBoxRect(i)

            ctx.save()

            rgb = self.historyColors[i]

            ctx.rectangle(
                boxX,
                boxY,
                boxWidth,
                boxHeight)

            ctx.set_source_rgb(
                *rgb)

            ctx.clip_preserve()
            ctx.fill_preserve()

            if hiliteTarget == ('legend', i):
                rgb = (0.0, 0.0, 1.0)
                lineWidth = 10
                ctx.set_line_width(
                    lineWidth)
                ctx.set_source_rgb(
                    *rgb)
                ctx.stroke_preserve()

            if i in historyTypes:
                rgb = (1.0, 1.0, 1.0)
            else:
                rgb = (0.0, 0.0, 0.0)

            lineWidth = 4
            ctx.set_line_width(
                lineWidth)

            ctx.set_source_rgb(
                *rgb)

            ctx.stroke()

            label = historyLegends[i]
            playout.set_text(label)
            labelWidth, labelHeight = playout.get_pixel_size()

            xx = boxX + (boxWidth / 2) - (labelWidth / 2)
            yy = boxY + (boxHeight / 2) - (labelHeight / 2)

            yy += 1 # Fudge the position.

            self.drawShadowText(label, xx, yy, ctx, playout)

            ctx.restore()

        ctx.restore()

        # Draw scale.

        ctx.save()

        playout.set_font_description(self.labelFont)

        scaleX, scaleY, scaleWidth, scaleHeight = self.getScaleRect()

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
            label = '10 Years'
        elif historyScale == micropolisengine.HISTORY_SCALE_LONG:
            rgb = (0.5, 0.5, 1)
            label = '120 Years'
        else:
            print "Invalid historyScale:", historyScale
            rgb = (1, 0, 0)
            label = '???'

        ctx.set_source_rgb(
            *rgb)

        ctx.clip_preserve()
        ctx.fill_preserve()

        if hiliteTarget == ('scale', 0):
            lineWidth = 10
            ctx.set_line_width(lineWidth)
            ctx.set_source_rgb(
                0.0, 0.0, 1.0)

            ctx.stroke_preserve()

        lineWidth = 4
        ctx.set_line_width(lineWidth)
        ctx.set_source_rgb(
            *self.strokeColor)

        ctx.stroke()

        playout.set_text(label)
        labelWidth, labelHeight = playout.get_pixel_size()

        xx = 0 + (scaleWidth / 2) - (labelWidth / 2)
        yy = 0 + (scaleHeight / 2) - (labelHeight / 2)

        yy += 1 # Fudge the position.
        
        self.drawShadowText(label, xx, yy, ctx, playout)

        ctx.restore()

        # Draw histories.

        ctx.save()

        ctx.translate(
            histX,
            histY)

        # Scale the residential, commercial and industrial histories
        # together relative to the max of all three.  Up to 128 they
        # are not scaled. Starting at 128 they are scaled down so the
        # maximum is always at the top of the history.

        def calcScale(maxVal):
            if maxVal < 128:
                maxVal = 0
            if maxVal > 0:
                return 128.0 / float(maxVal)
            else:
                return 1.0

        resHistoryMin, resHistoryMax = getHistoryRange(
            micropolisengine.HISTORY_TYPE_RES,
            historyScale)
        comHistoryMin, comHistoryMax = getHistoryRange(
            micropolisengine.HISTORY_TYPE_COM,
            historyScale)
        indHistoryMin, indHistoryMax = getHistoryRange(
            micropolisengine.HISTORY_TYPE_IND,
            historyScale)
        allMax = max(resHistoryMax, 
                     max(comHistoryMax,
                         indHistoryMax))
        rciScale = calcScale(allMax)

        # Scale the money, crime and pollution histories 
        # independently of each other.

        moneyHistoryMin, moneyHistoryMax = getHistoryRange(
            micropolisengine.HISTORY_TYPE_MONEY,
            historyScale)
        crimeHistoryMin, crimeHistoryMax = getHistoryRange(
            micropolisengine.HISTORY_TYPE_CRIME,
            historyScale)
        pollutionHistoryMin, pollutionHistoryMax = getHistoryRange(
            micropolisengine.HISTORY_TYPE_POLLUTION,
            historyScale)

        moneyScale = calcScale(moneyHistoryMax)
        crimeScale = calcScale(crimeHistoryMax)
        pollutionScale = calcScale(pollutionHistoryMax)

        historyRange = 128.0
        historyScales = (
            rciScale, rciScale, rciScale, # res, com, ind
            moneyScale, crimeScale, pollutionScale, # money, crime, pollution
        )

        for historyType in historyTypes:

            scaleValue = historyScales[historyType]

            first = True
            for historyIndex in range(historyCount - 1, -1, -1):

                rawVal = getHistory(
                    historyType,
                    historyScale,
                    historyIndex)

                val = rawVal * scaleValue

                x = (
                    histWidth -
                    (float(historyIndex) * (float(histWidth) / float(historyCount - 1))))
                #print historyIndex, x, histWidth

                y = (
                    histHeight -
                    (float(val) * (float(histHeight) / float(historyRange))))

                if first:
                    #print "rawVal", rawVal, "scale", scale, "val", val, "y", y
                    ctx.move_to(x, y)
                    first = False
                else:
                    ctx.line_to(x, y)

            ctx.line_to(x + self.rightEdgeWidth, y)

            if (hiliteTarget and
                (hiliteTarget[0] == 'legend') and
                (hiliteTarget[1] == historyType)):
                lineWidth = 8
            else:
                lineWidth = 2

            ctx.set_line_width(lineWidth)

            ctx.set_source_rgb(
                *self.historyColors[historyType])

            ctx.stroke()

        ctx.restore()

        # Draw years.

        cityYear = engine.cityYear
        cityMonth = engine.cityMonth

        ctx.save()

        ctx.translate(
            histX,
            histY)

        ctx.set_line_width(1)

        ctx.set_source_rgb(
            *self.strokeColor)

        sx = float(histWidth) / 120.0

        cityTime = engine.cityTime
        year = (cityTime / 48) + engine.startingYear
        month = (cityTime / 4) % 12

        if historyScale == micropolisengine.HISTORY_SCALE_SHORT:

            dur = 120
            r = range(dur - cityMonth, -1, -dur / 10)

        elif historyScale == micropolisengine.HISTORY_SCALE_LONG:

            sx /= 10
            past = 10 * (year % 10)
            year = int(year / 10) * 10
            dur = 1200
            r = range(dur - past, -1, -dur / 10)

        else:

            print "Invalid historyScale:", historyScale
            dur = 120
            r = ()

        y = histHeight + self.bottomEdgeHeight

        playout.set_font_description(self.labelFont)

        for i in r:

            x = i * sx

            ctx.move_to(
                x, 0)

            ctx.line_to(
                x, y)

            ctx.stroke()

            if historyScale == micropolisengine.HISTORY_SCALE_SHORT:

                label = str(year)
                year -= 1

            elif historyScale == micropolisengine.HISTORY_SCALE_LONG:

                label = str(year)
                year -= 10

            playout.set_text(label)
            labelWidth, labelHeight = playout.get_pixel_size()

            xx = x + 4
            yy = histHeight + (self.bottomEdgeHeight / 2) - (labelHeight / 2)

            yy += 1 # Fudge the position.

            self.drawShadowText(label, xx, yy, ctx, playout)

        ctx.restore()
    

    def handleMouseDrag(
        self,
        event):
        target = self.findTarget(event.x, event.y)
        if target != self.hiliteTarget:
            self.hiliteTarget = target
            self.queue_draw()


    def handleMousePoint(
        self,
        event):

        target = self.findTarget(event.x, event.y)
        if target != self.hiliteTarget:
            self.hiliteTarget = target
            self.queue_draw()


    def handleButtonRelease(
        self,
        widget,
        event):

        print "handleMouseRelease", self, event, event.x, event.y

        if not self.clickable:
            return

        self.handleMouseDrag(event)

        self.down = False

        if self.hiliteTarget:
            targetType, targetIndex = self.hiliteTarget
            if  targetType == 'scale':

                if self.historyScale == micropolisengine.HISTORY_SCALE_SHORT:
                    self.historyScale = micropolisengine.HISTORY_SCALE_LONG
                elif self.historyScale == micropolisengine.HISTORY_SCALE_LONG:
                    self.historyScale = micropolisengine.HISTORY_SCALE_SHORT
                else:
                    print "Invalid history scale", self.historyScale

            elif targetType == 'legend':

                historyTypes = self.historyTypes
                if targetIndex in historyTypes:
                    historyTypes.remove(targetIndex)
                else:
                    historyTypes.append(targetIndex)

            elif targetType == 'history':
                pass


########################################################################
