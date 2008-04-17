# micropoliswindow.py
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
# Micropolis Window
# Don Hopkins


########################################################################
# Import stuff


import sys
import os
import time
import gtk
import gobject
import cairo
import pango
import math
import thread
import random
import array


########################################################################
# Import our modules


#print "CWD", os.getcwd()

cwd = os.getcwd()

for relPath in (
  'ReleaseSymbols',
  'build/lib.macosx-10.3-i386-2.5',
  '../../TileEngine/python/ReleaseSymbols',
  '../../TileEngine/python/build/lib.macosx-10.3-i386-2.5',
  '../../TileEngine/python',
  '.',
):
    sys.path.insert(0, os.path.join(cwd, relPath))

import micropolis
import micropolismodel
import micropolisutils
import micropolispiemenus
import micropolisdrawingarea


########################################################################
# MicropolisWindow

class MicropolisWindow(gtk.Window):


    def __init__(
        self,
        engine=None,
        viewClass=micropolisdrawingarea.EditableMicropolisDrawingArea,
        **args):
        
        gtk.Window.__init__(self, **args)

        self.connect('destroy', gtk.main_quit)

        self.set_title("OLPC Micropolis for Python/Cairo/Pango, by Don Hopkins")

        self.engine = engine

        self.da = \
            viewClass(
                engine=self.engine)

        self.add(self.da)
        engine.addView(self.da)


########################################################################
# MicropolisControlPanel


class MicropolisControlPanel(gtk.DrawingArea):


    def __init__(
        self,
        view=None,
        strokeColor=(0, 0, 0),
        fillColor=(1, 1, 1),
        fontName='Helvetica 10',
        **args):

        gtk.DrawingArea.__init__(self, **args)

        self.view = view
        self.engine = view.engine
        self.strokeColor = strokeColor
        self.fillColor = fillColor

        self.connect('expose_event', self.handleExpose)


    def handleExpose(
        self,
        widget,
        event,
        *args):

        if args:
            pass #print "handleExpose TileDrawingArea", self, "WIDGET", widget, "EVENT", event, "ARGS", args

        self.draw(widget, event)

        return False


    def draw(
        self,
        widget=None,
        event=None):

        ctxWindow = self.window.cairo_create()

        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        ctxWindow.rectangle(
            0,
            0,
            winWidth,
            winHeight)

        ctxWindow.set_line_width(3)

        ctxWindow.set_source_rgb(
            *self.strokeColor)

        ctxWindow.stroke_preserve()

        ctxWindow.set_source_rgb(
            *self.fillColor)

        ctxWindow.fill()

        self.drawContent(ctxWindow)


    def drawContent(
        self,
        ctx):

        pass


    def update(
        self):

        self.queue_draw()


########################################################################
# MicropolisTopControlPanel


class MicropolisTopControlPanel(MicropolisControlPanel):


    def __init__(
        self,
        strokeColor=(0, 0, 0),
        fillColor=(0, 1, 0),
        **args):

        MicropolisControlPanel.__init__(
            self,
            strokeColor=strokeColor,
            fillColor=fillColor,
            **args)


########################################################################
# MicropolisBottomControlPanel


class MicropolisBottomControlPanel(MicropolisControlPanel):


    def __init__(
        self,
        strokeColor=(0, 0, 0),
        fillColor=(0.75, 0.75, 0.75),
        stackVertical = False,
        shortScaleFirst = False,
        **args):

        MicropolisControlPanel.__init__(
            self,
            strokeColor=strokeColor,
            fillColor=fillColor,
            **args)

        self.stackVertical = stackVertical
        self.shortScaleFirst = shortScaleFirst


    def drawContent(
        self,
        ctx):

        #print "==== DRAWCONTENT", self

        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        graphColors = (
            (0.50, 0.50, 1.00), # LIGHTGREEN
            (0.00, 0.00, 0.50), # DARKBLUE
            (1.00, 1.00, 0.00), # YELLOW
            (0.00, 0.50, 0.00), # DARKGREEN
            (1.00, 0.00, 0.00), # RED
            (0.33, 0.42, 0.18), # OLIVE
        )

        historyCount = micropolis.HISTORY_COUNT
        historyTypeCount = micropolis.HISTORY_TYPE_COUNT
        historyScaleCount = micropolis.HISTORY_SCALE_COUNT

        scaleWidth = int(float(winWidth) / float(historyScaleCount))
        scaleHeight = int(float(winHeight) / float(historyScaleCount))
        getHistory = self.engine.GetHistory
        getHistoryRange = self.engine.GetHistoryRange

        rightEdgeWidth = 20

        for historyScale in range(0, historyScaleCount):

            #print "historyScale", historyScale

            if self.stackVertical:
                histX = 0
                if not self.shortScaleFirst:
                    histY = ((historyScaleCount - 1) - historyScale) * scaleHeight
                else:
                    histY = historyScale * scaleHeight
                histWidth = winWidth
                histHeight = scaleHeight
            else:
                if not self.shortScaleFirst:
                    histX = ((historyScaleCount - 1) - historyScale) * scaleWidth
                else:
                    histX = historyScale * scaleWidth
                histY = 0
                histWidth = scaleWidth
                histHeight = winHeight

            margin = 3

            histX += margin
            histY += margin
            histWidth -= 2 * margin
            histHeight -= 2 * margin

            histWidth -= rightEdgeWidth

            ctx.save()

            ctx.rectangle(
                histX,
                histY,
                histWidth,
                histHeight)

            ctx.set_source_rgb(
                1.0, 1.0, 1.0)

            #ctx.fill_preserve()
            ctx.fill()

            #ctx.clip()

            for historyType in range(0, historyTypeCount):

                historyMin, historyMax = getHistoryRange(
                    historyType,
                    historyScale)
                historyMin = 0
                historyMax = 500
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
                        histX +
                        histWidth +
                        -(float(historyIndex) * (float(histWidth) / float(historyCount))))

                    y = (
                        histY +
                        (float(val) * (float(histHeight) / float(historyRange))))

                    if historyIndex == 0:
                        ctx.move_to(x, y)
                    else:
                        ctx.line_to(x, y)

                ctx.set_source_rgb(
                    *graphColors[historyType])

                ctx.stroke()

                ctx.move_to(x, y)
                ctx.line_to(x + rightEdgeWidth, y)

                ctx.stroke()

            ctx.restore()

            ctx.rectangle(
                histX,
                histY,
                histWidth,
                histHeight)

            ctx.set_source_rgb(
                *self.strokeColor)

            ctx.set_line_width(2)

            ctx.stroke()


########################################################################
# MicropolisLeftControlPanel


class MicropolisLeftControlPanel(MicropolisControlPanel):


    def __init__(
        self,
        strokeColor=(0, 0, 0),
        fillColor=(1, 1, 0),
        **args):

        MicropolisControlPanel.__init__(
            self,
            strokeColor=strokeColor,
            fillColor=fillColor,
            **args)


########################################################################
# MicropolisRightControlPanel


class MicropolisRightControlPanel(MicropolisControlPanel):

    def __init__(
        self,
        strokeColor=(0, 0, 0),
        fillColor=(0, 0, 1),
        **args):

        MicropolisControlPanel.__init__(
            self,
            strokeColor=strokeColor,
            fillColor=fillColor,
            **args)


########################################################################
# MicropolisPanelWindow

class MicropolisPanedWindow(gtk.Window):


    def __init__(
        self,
        engine=None,
        viewClass=micropolisdrawingarea.EditableMicropolisDrawingArea,
        topControlPanelClass=MicropolisTopControlPanel,
        bottomControlPanelClass=MicropolisBottomControlPanel,
        leftControlPanelClass=MicropolisLeftControlPanel,
        rightControlPanelClass=MicropolisRightControlPanel,
        **args):
        
        gtk.Window.__init__(self, **args)

        self.connect('destroy', gtk.main_quit)

        self.connect('realize', self.handleRealize)
        self.connect('size-allocate', self.handleResize)

        self.set_title("OLPC Micropolis for Python/Cairo/Pango, by Don Hopkins")

        self.firstResize = True

        self.engine = engine

        self.tileView = \
            viewClass(
                engine=self.engine)
        self.frameCenter = gtk.Frame()
        self.frameCenter.set_shadow_type(gtk.SHADOW_IN)
        self.frameCenter.add(self.tileView)
        engine.addView(self.tileView)

        self.topControlPanel = \
            topControlPanelClass(
                view=self.tileView)
        self.frameTop = gtk.Frame()
        self.frameTop.set_shadow_type(gtk.SHADOW_IN)
        self.frameTop.add(self.topControlPanel)
        engine.addEvaluation(self.topControlPanel)

        self.bottomControlPanel = \
            bottomControlPanelClass(
                view=self.tileView)
        self.frameBottom = gtk.Frame()
        self.frameBottom.set_shadow_type(gtk.SHADOW_IN)
        self.frameBottom.add(self.bottomControlPanel)
        engine.addGraph(self.bottomControlPanel)

        self.leftControlPanel = \
            leftControlPanelClass(
                view=self.tileView)
        self.frameLeft = gtk.Frame()
        self.frameLeft.set_shadow_type(gtk.SHADOW_IN)
        self.frameLeft.add(self.leftControlPanel)

        self.rightControlPanel = \
            rightControlPanelClass(
                view=self.tileView)
        self.frameRight = gtk.Frame()
        self.frameRight.set_shadow_type(gtk.SHADOW_IN)
        self.frameRight.add(self.rightControlPanel)

        self.vpaned1 = gtk.VPaned()
        self.vpaned2 = gtk.VPaned()
        self.hpaned1 = gtk.HPaned()
        self.hpaned2 = gtk.HPaned()

        # Put the top level pane in this window.

        self.add(
            self.vpaned1)

        # Nest the vertical and horizontal panes into a tree.

        self.vpaned1.pack2(
            self.vpaned2,
            resize=False,
            shrink=False)

        self.vpaned2.pack1(
            self.hpaned1,
            resize=False,
            shrink=False)

        self.hpaned1.pack2(
            self.hpaned2,
            resize=False,
            shrink=False)

        # Populate the leaves of the tree with the view and control panels.

        self.hpaned2.pack1(
            self.frameCenter,
            resize=False,
            shrink=False)

        self.vpaned1.pack1(
            self.frameTop,
            resize=False,
            shrink=False)

        self.vpaned2.pack2(
            self.frameBottom,
            resize=False,
            shrink=False)

        self.hpaned1.pack1(
            self.frameLeft,
            resize=False,
            shrink=False)

        self.hpaned2.pack2(
            self.frameRight,
            resize=False,
            shrink=False)


    def resizeEdges(
        self):

        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        print "WINDOW SIZE", winWidth, winHeight

        extra = 4
        padding = 14

        leftEdge = 16
        rightEdge = 16
        topEdge = 16
        bottomEdge = 100

        self.hpaned1.set_position(leftEdge + extra)
        self.hpaned2.set_position(winWidth - (extra + leftEdge + padding + rightEdge))

        self.vpaned1.set_position(topEdge + extra)
        self.vpaned2.set_position(winHeight - (extra + topEdge + padding + bottomEdge))

        self.tileView.panTo(-100, -100)


    def handleRealize(
        self,
        *args):

        #print "handleRealize MicropolisPanedWindow", self, "ARGS", args

        self.firstResize = True


    def handleResize(
        self,
        widget,
        event,
        *args):

        #print "handleResize MicropolisPanedWindow", self, "WIDGET", widget, "EVENT", event, "ARGS", args

        if self.firstResize:
            self.firstResize = False
            self.resizeEdges()


########################################################################


if __name__ == '__main__':

    engine = micropolismodel.CreateTestEngine()

    fudge = 0
    width = int((120 * 4) + fudge)
    height = int((100 * 4) + fudge)

    w = width
    h = height

    x1 = 0
    y1 = 0
    x2 = w + 20
    y2 = h + 40

    if True:
        win2 = MicropolisPanedWindow(engine=engine)
        win2.set_default_size(500, 500)
        win2.move(x1, y1)
        win2.show_all()
        win2.tileView.setScale(1.0)
        win2.tileView.panTo(-200, -200)

    if False:
        win2 = MicropolisWindow(engine=engine)
        win2.set_default_size(w, h)
        win2.move(x1, y1)
        win2.show_all()
        win2.da.setScale(1.0)

    if False:
        win1 = MicropolisWindow(
            engine=engine, 
            viewClass=micropolisdrawingarea.MiniMicropolisDrawingArea)
        win1.set_default_size(width, height)
        win1.move(x2, y1)
        win1.show_all()

    if False:
        win3 = MicropolisWindow(engine=engine)
        win3.set_default_size(w, h)
        win3.move(x1, y2)
        win3.show_all()
        win3.da.setScale(2.0)

    if False:
        win4 = MicropolisWindow(engine=engine)
        win4.set_default_size(w, h)
        win4.move(x2, y2)
        win4.show_all()
        win4.da.setScale(4.0)

    gtk.main()


########################################################################
