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
import micropolisutils
import micropolispiemenus
from pyMicropolis.tileEngine import tileengine, tiledrawingarea
import micropolistool


########################################################################
# Globals
# @todo This should go through some kind of a resource manager.


Sprites = [
    {
        'id': 1,
        'name': 'train',
        'frames': 5,
    },
    {
        'id': 2,
        'name': 'helicopter',
        'frames': 8,
    },
    {
        'id': 3,
        'name': 'airplane',
        'frames': 11,
    },
    {
        'id': 4,
        'name': 'boat',
        'frames': 8,
    },
    {
        'id': 5,
        'name': 'monster',
        'frames': 16,
    },
    {
        'id': 6,
        'name': 'tornado',
        'frames': 3,
    },
    {
        'id': 7,
        'name': 'explosion',
        'frames': 6,
    },
    {
        'id': 8,
        'name': 'bus',
        'frames': 4,
    },
]
for spriteData in Sprites:
    images = []
    spriteData['images'] = images
    for i in range(0, spriteData['frames']):
        fileName = 'images/simEngine/obj%d-%d.png' % (
            spriteData['id'],
            i,
        )
        fileName = os.path.join(os.path.dirname(__file__), "../.." , fileName)
        fileName = os.path.abspath(fileName)
        image = cairo.ImageSurface.create_from_png(fileName)
        images.append(image)


########################################################################
# Utilities


def PRINT(*args):
    print args


########################################################################


class MicropolisDrawingArea(tiledrawingarea.TileDrawingArea):


    def __init__(
        self,
        engine=None,
        interests=('city'),
        sprite=micropolisengine.SPRITE_NOTUSED,
        showData=True,
        showRobots=True,
        showSprites=True,
        showChalk=True,
        mapStyle='all',
        overlayAlpha=0.5,
        **args):

        args['tileCount'] = micropolisengine.TILE_COUNT
        args['sourceTileSize'] = micropolisengine.BITS_PER_TILE
        args['worldCols'] = micropolisengine.WORLD_W
        args['worldRows'] = micropolisengine.WORLD_H

        self.engine = engine
        self.showData = showData
        self.showRobots = showRobots
        self.showSprites = showSprites
        self.showChalk = showChalk
        self.mapStyle = mapStyle
        self.overlayAlpha = overlayAlpha

        tiledrawingarea.TileDrawingArea.__init__(self, **args)

        self.sprite = sprite
        engine.expressInterest(
            self,
            interests)

        self.blinkFlag = True

        self.reset()


    def reset(self):
        self.selectToolByName('Bulldozer')


    def configTileEngine(self, tengine):

        engine = self.engine
        buffer = engine.getMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = micropolisengine.WORLD_W
        tengine.height = micropolisengine.WORLD_H

        from micropolisengine import ZONEBIT, PWRBIT, ALLBITS, LIGHTNINGBOLT

        def tileFunction(col, row, tile):
            if (tile & ZONEBIT) and not (tile & PWRBIT) and random.random() < 0.5:
                tile = LIGHTNINGBOLT | (tile & ALLBITS)
            return tile

        self.tileFunction = tileFunction

        # Unsigned short tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = micropolisengine.BYTES_PER_TILE * micropolisengine.WORLD_H
        tengine.rowBytes = micropolisengine.BYTES_PER_TILE
        tengine.tileMask = micropolisengine.LOMASK


    def getCell(self, col, row):

        return self.engine.getTile(col, row)


    def beforeDraw(
        self):

        engine = self.engine
        self.blinkFlag = (engine.tickCount() % 60) < 30


    def drawOverlays(
        self,
        ctx):

        if self.showData:
            self.drawData(ctx)

        if self.showRobots:
            self.drawRobots(ctx)

        if self.showSprites:
            self.drawSprites(ctx)

        if self.showChalk:
            self.drawChalk(ctx)

        if self.showCursor:
            tool = self.getActiveTool()
            if tool:
                tool.drawCursor(self, ctx)


    def setMapStyle(self, mapStyle):
        self.mapStyle = mapStyle


    def drawData(self, ctx):
        mapStyle = self.mapStyle
        engine = self.engine
        dataImage, dataAlpha, width, height = \
            engine.getDataImageAlphaSize(mapStyle)
        if not dataImage:
            return

        width = 1.0 / width
        height = 1.0 / height

        ctx.save()

        tileSize = self.tileSize

        ctx.translate(self.panX, self.panY)

        ctx.scale(
            self.worldCols * tileSize,
            self.worldRows * tileSize)

        ctx.rectangle(0, 0, 1, 1)
        ctx.clip()

        imageWidth = dataImage.get_width()
        imageHeight = dataImage.get_height()

        ctx.scale(
            width / imageWidth,
            height / imageHeight)

        ctx.set_source_surface(
            dataImage,
            0,
            0)
        ctx.paint_with_alpha(dataAlpha)

        ctx.restore()


    def drawSprites(self, ctx):
        engine = self.engine
        sprite = engine.spriteList
        while True:
            if not sprite:
                break
            self.drawSprite(ctx, sprite)
            sprite = sprite.next


    def drawSprite(self, ctx, sprite):

        spriteType = sprite.type
        spriteFrame = sprite.frame

        if (spriteFrame == 0 or
            spriteType == micropolisengine.SPRITE_NOTUSED or
            spriteType >= micropolisengine.SPRITE_COUNT):
            return

        ctx.save()

        x = sprite.x
        y = sprite.y
        width = sprite.width
        height = sprite.height
        tileSize = self.tileSize

        ctx.translate(self.panX, self.panY)
        ctx.scale(tileSize / 16.0, tileSize / 16.0)

        ctx.translate(x + sprite.xOffset, y + sprite.yOffset)

        image = Sprites[spriteType - 1]['images'][spriteFrame - 1]

        ctx.set_source_surface(
            image,
            0,
            0)
        #ctx.rectangle(0, 0, 1, 1)
        ctx.paint()

        ctx.restore()


    def drawRobots(self, ctx):
        engine = self.engine
        robots = engine.robots

        if not robots:
            return

        ctx.save()

        tileSize = self.tileSize

        ctx.translate(self.panX, self.panY)
        ctx.scale(tileSize / 16.0, tileSize / 16.0)

        for robot in robots:
            robot.draw(ctx)

        ctx.restore()


    def drawChalk(self, ctx):
        pass # TODO: drawChalk


    def tickEngine(self):

        # Don't do anything! The engine ticks itself.
        return


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


    def handleKey(
        self,
        key):

        if key == 'm':
            self.engine.heatSteps = 1
            self.engine.heatRule = 0
            return True
        elif key == 'n':
            self.engine.heatSteps = 1
            self.engine.heatRule = 1
            return True
        elif key == 'o':
            self.engine.heatSteps = 0
            return True

        return False


    def engage(self):
        self.engine.addView(self)


    def disengage(self):
        self.engine.removeView(self)


########################################################################


class EditableMicropolisDrawingArea(MicropolisDrawingArea):


    pass


########################################################################


class NoticeMicropolisDrawingArea(MicropolisDrawingArea):


    def __init__(
        self,
        follow=None,
        centerOnTileHandler=None,
        **args):

        args['keyable'] = False
        args['clickable'] = False
        args['zoomable'] = False
        args['pannable'] = False
        args['menuable'] = False
        args['showCursor'] = False
        args['scale'] = 2

        MicropolisDrawingArea.__init__(self, **args)

        self.follow = follow
        self.centerOnTileHandler = centerOnTileHandler


    def handleMousePoint(
        self,
        event):

        pass


    def handleButtonPress(
        self,
        widget,
        event):

        centerOnTileHandler = self.centerOnTileHandler
        if centerOnTileHandler:
            centerX, centerY = self.getCenterTile()
            centerOnTileHandler(centerX, centerY)


    def handleMouseDrag(
        self,
        event):

        pass


    def handleButtonRelease(
        self,
        widget,
        event):

        pass


    def handleMouseScroll(
        self,
        widget,
        event):

        pass


    def beforeDraw(
        self):

        MicropolisDrawingArea.beforeDraw(self)

        engine = self.engine
        self.blinkFlag = (engine.tickCount() % 30) < 15

        sprite = self.sprite
        if sprite != micropolisengine.SPRITE_NOTUSED:
            s = engine.getSprite(sprite)
            if s:
                fudge = 8
                x = ((s.x + s.xHot + fudge) / 16.0)
                y = ((s.y + s.yHot + fudge) / 16.0)
                self.centerOnTile(x, y)


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
        args['showRobots'] = False
        args['showSprites'] = False
        args['scale'] = 1.0 / micropolisengine.EDITOR_TILE_SIZE
        args['overlayAlpha'] = 0.8

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
        viewWidth = viewRect.width
        viewHeight = viewRect.height

        x = self.panX - ((view.panX / view.tileSize) * self.tileSize)
        y = self.panY - ((view.panY / view.tileSize) * self.tileSize)

        width = (viewWidth / view.tileSize) * self.tileSize
        height = (viewHeight / view.tileSize) * self.tileSize

        #print "GETVIEWBOX", "view", view, "pan", view.panX, view.panY, "tileSize", view.tileSize, "pos", x, y, "size", width, height

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
        view.updateView()


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
        if ((not view) and
            (not view.zoomable)):
            pass

        direction = event.direction

        if direction == gtk.gdk.SCROLL_UP:
            view.changeScale(view.scale * view.scrollWheelZoomScale)
        elif direction == gtk.gdk.SCROLL_DOWN:
            view.changeScale(view.scale / view.scrollWheelZoomScale)


########################################################################


class PreviewMicropolisDrawingArea(MicropolisDrawingArea):


    def __init__(
        self,
        **args):

        args['keyable'] = False
        args['clickable'] = True
        args['zoomable'] = False
        args['pannable'] = False
        args['menuable'] = False
        args['showCursor'] = False
        args['showRobots'] = False
        args['showSprites'] = False
        args['scale'] = 3.0 / micropolisengine.EDITOR_TILE_SIZE
        args['overlayAlpha'] = 0.8

        MicropolisDrawingArea.__init__(self, **args)


    def handleMousePoint(
        self,
        event):

        pass


    def handleButtonPress(
        self,
        widget,
        event):

        pass


    def handleMouseDrag(
        self,
        event):

        pass


    def handleButtonRelease(
        self,
        widget,
        event):

        pass


    def handleMouseScroll(
        self,
        widget,
        event):

        pass


########################################################################
