# micropolisdrawingarea.py
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
# Micropolis Drawing Area
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


import micropolisengine
import micropolismodel
import micropolisutils
import micropolispiemenus
from tiledrawingarea import TileDrawingArea
import micropolistool
from tiletool import TileTool


########################################################################
# Utilities


def PRINT(*args):
    print args


########################################################################


class MicropolisDrawingArea(TileDrawingArea):


    def __init__(
        self,
        engine=None,
        interests=('city', 'editor'),
        **args):

        args['tileCount'] = 960
        args['sourceTileSize'] = 16
        args['worldCols'] = micropolisengine.WORLD_X
        args['worldRows'] = micropolisengine.WORLD_Y

        self.engine = engine

        TileDrawingArea.__init__(self, **args)

        engine.expressInterest(
            self,
            interests)

        self.reset()


    def reset(self):
        self.selectToolByName('Bulldozer')


    def configTileEngine(self, tengine):

        engine = self.engine
        buffer = engine.getMapBuffer()
        print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = micropolisengine.WORLD_X
        tengine.height = micropolisengine.WORLD_Y

        # Unsigned short tile values, in column major order.
        tengine.typeCode = 'H'
        tengine.colBytes = 2 * micropolisengine.WORLD_Y
        tengine.rowBytes = 2
        tengine.tileMask = micropolisengine.LOMASK


    def getCell(self, col, row):

        return self.engine.getTile(col, row)


    def drawOverlays(
        self,
        ctx):

        self.drawSprites(ctx)

        self.drawChalk(ctx)

        if self.showCursor:
            tool = self.getActiveTool()
            if tool:
                tool.drawCursor(self, ctx)


    def drawSprites(self, ctx):
        pass # TODO: drawSprites


    def drawChalk(self, ctx):
        pass # TODO: drawChalk


    def tickEngine(self):

        engine = self.engine
        engine.sim_tick()
        engine.animateTiles()


    def makeToolPie(self):

        toolPie = micropolispiemenus.MakeToolPie(lambda toolName: self.selectToolByName(toolName))
        self.toolPie = toolPie


    def handleButtonPress(
        self,
        widget,
        event):

        self.handleToolPieButtonPress(
            widget,
            event)


########################################################################


class EditableMicropolisDrawingArea(MicropolisDrawingArea):


    pass


########################################################################


class MiniMicropolisDrawingArea(MicropolisDrawingArea):


    def __init__(
        self,
        **args):

        args['keyable'] = False
        args['clickable'] = False
        args['zoomable'] = False
        args['pannable'] = False
        args['menuable'] = False
        args['showCursor'] = False
        args['scale'] = 0.25

        MicropolisDrawingArea.__init__(self, **args)

        self.currentView = None
        self.panning = False
        self.panningView = None
        self.panningStartCursorX = 0
        self.panningStartCursorY = 0
        self.panningStartPanX = 0
        self.panningStartPanY = 0


    def drawOverlays(self, ctx):

        MicropolisDrawingArea.drawOverlays(self, ctx)

        self.drawOtherViews(ctx)


    def getViewBox(self, view):

        viewRect = view.get_allocation()

        x = self.panX + (((viewRect.x - view.panX) / view.tileSize) * self.tileSize)
        y = self.panY + (((viewRect.y - view.panY) / view.tileSize) * self.tileSize)
        width = (viewRect.width / view.tileSize) * self.tileSize
        height = (viewRect.height / view.tileSize) * self.tileSize

        return x, y, width, height


    def drawOtherViews(self, ctx):

        if self.panning:
            currentView = self.panningView
        else:
            currentView = self.currentView

        views = self.engine.views

        for view in views:

            if view == self:
                continue

            x, y, width, height = self.getViewBox(view)
             
            if view == currentView:

                pad = 4

                ctx.rectangle(
                    x - pad,
                    y - pad,
                    width + (pad * 2),
                    height + (pad * 2))


                ctx.set_line_width(
                    pad * 2)

                ctx.set_source_rgb(
                    0.0,
                    0.0,
                    1.0)

                ctx.stroke_preserve()

                ctx.set_line_width(
                    pad)

                ctx.set_source_rgb(
                    1.0,
                    1.0,
                    0.0)

                ctx.stroke()

            else:

                pad = 2

                ctx.rectangle(
                    x - pad,
                    y - pad,
                    width + (pad * 2),
                    height + (pad * 2))

                ctx.set_line_width(
                    pad * 2)

                ctx.set_source_rgb(
                    1.0,
                    1.0,
                    1.0)

                ctx.stroke_preserve()

                ctx.set_line_width(
                    pad)

                ctx.set_source_rgb(
                    0.0,
                    0.0,
                    0.0)

                ctx.stroke()


    def getCursorPosition(
        self,
        event):

        if not event:
            x, y, state = self.window.get_pointer()
        elif (hasattr(event, 'is_hint') and
              event.is_hint):
            x, y, state = event.window.get_pointer()
        else:
            x = event.x
            y = event.y
            state = event.state

        return x, y


    def handleMousePoint(
        self,
        event):

        x, y = self.getCursorPosition(event)

        views = self.engine.views
        found = []

        for view in views:

            if view == self:
                continue

            viewX, viewY, viewWidth, viewHeight = self.getViewBox(view)

            if ((x >= viewX) and
                (x < (viewX + viewWidth)) and
                (y >= viewY) and
                (y < (viewY + viewHeight))):
                found.append(view)

        if found:
            self.currentView = found[-1]
        else:
            self.currentView = None


    def handleButtonPress(
        self,
        widget,
        event):

        if not self.currentView:
            self.panning = False
            self.down = False
            return

        x, y = self.getCursorPosition(event)
        view = self.currentView

        self.down = True
        self.panning = True
        self.panningView = view
        self.panningStartCursorX = x
        self.panningStartCursorY = y
        self.panningStartPanX = view.panX
        self.panningStartPanY = view.panY


    def handleMouseDrag(
        self,
        event):

        if not self.panning:
            return

        x, y = self.getCursorPosition(event)
        view = self.panningView

        dx = self.panningStartCursorX - x
        dy = self.panningStartCursorY - y
        scale = view.tileSize / self.tileSize
        dx *= scale
        dy *= scale

        view.panX = self.panningStartPanX + dx
        view.panY = self.panningStartPanY + dy


    def handleButtonRelease(
        self,
        widget,
        event):

        if not self.panning:
            return

        self.handleMouseDrag(
            event)

        self.down = False
        self.panning = False
        self.panningView = None


    def handleMouseScroll(
        self,
        widget,
        event):

        view = self.currentView
        if not view:
            pass

        direction = event.direction

        if direction == gtk.gdk.SCROLL_UP:
            view.changeScale(view.scale * view.scrollWheelZoomScale)
        elif direction == gtk.gdk.SCROLL_DOWN:
            view.changeScale(view.scale / view.scrollWheelZoomScale)


    def handleKey(
        self,
        key):
        
        print "*" * 72
        print "HANDLEKEY", key

        if key == 'm':
            self.engine.heat_steps = 1
            self.engine.heat_rule = 0
            return True
        elif key == 'n':
            self.engine.heat_steps = 1
            self.engine.heat_rule = 1
            return True

        return False


########################################################################
