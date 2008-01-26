# setup.py
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
# Old test file. Probably doesn't work, since APIs have changed. 
# Don Hopkins



import gtk
import gobject
import cairo
import pango
import math
import time
import thread
import random
import array
import cam
from cam import RenderCellTiles, RenderCellPixels
from cam.utils import Engine
from configurations import *


########################################################################


DefaultConfiguration = Configurations[2]


########################################################################


class CamDrawingArea(gtk.DrawingArea):


    def __init__(
        self,
        drawTiles=True,
        drawTilesAlpha=1.0,
        drawPixels=False,
        drawPixelsAlpha=0.5,
        pixelTileScaleCrossfade=True,
        pixelTileScaleThresholdMin=8.0,
        pixelTileScaleThresholdMax=32.0,
        cmapFileName='cmap.png',
        cmapCount=256,
        tilesFileName='vonNeumann29Tiles.png',
        sourceTileSize=32,
        cellCols=256,
        cellRows=256,
        running=True,
        renderCol=0,
        renderRow=0,
        renderCols=256,
        renderRows=256,
        timeDelay=50,
        panX=0,
        panY=0,
        scale=32,
        cursorCol=0,
        cursorRow=0,
        config=DefaultConfiguration,
        outsideBackgroundColor=(0.5, 0.5, 0.5),
        insideBackgroundColor=(0.0, 0.0, 0.0),
        debugInset=0,
        **args):

        gtk.DrawingArea.__init__(self, **args)

        self.drawTiles = drawTiles
        self.drawTilesAlpha = drawTilesAlpha
        self.drawPixels = drawPixels
        self.drawPixelsAlpha = drawPixelsAlpha
        self.pixelTileScaleCrossfade = pixelTileScaleCrossfade
        self.pixelTileScaleThresholdMin = pixelTileScaleThresholdMin
        self.pixelTileScaleThresholdMax = pixelTileScaleThresholdMax
        self.cmapFileName = cmapFileName
        self.cmapCount = cmapCount
        self.tilesFileName = tilesFileName
        self.sourceTileSize = sourceTileSize
        self.cellCols = cellCols
        self.cellRows = cellRows
        self.running = running
        self.renderCol = renderCol
        self.renderRow = renderRow
        self.renderCols = renderCols
        self.renderRows = renderRows
        self.timeDelay = timeDelay
        self.panX = panX
        self.panY = panY
        self.scale = scale
        self.cursorCol = cursorCol
        self.cursorRow = cursorRow
        self.config = config
        self.outsideBackgroundColor = outsideBackgroundColor
        self.insideBackgroundColor = insideBackgroundColor
        self.debugInset = debugInset

        self.cmapSurface = None
        self.cmapDest = None
        self.cmapDestWidth = 0
        self.cmapDestHeight = 0
        self.tilesSurface = None
        self.tilesWidth = 0
        self.tilesHeight = 0
        self.tileMap = None
        self.down = False
        self.downX = 0
        self.downY = 0
        self.downPanX = 0
        self.downPanY = 0
        self.panning = False
        self.tileSize = 1
        self.buffer = None
        self.bufferWidth = 0
        self.bufferHeight = 0
        self.windowBuffer = None
        self.windowBufferWidth = 0
        self.windowBufferHeight = 0
        self.mouseX = 0
        self.mouseY = 0

        self.e = None
        self.c = None

        self.timerActive = False
        self.timerId = None

        self.tilesLoaded = False
        self.cmapLoaded = False

        self.createEngine()
        
        self.set_flags(
            gtk.CAN_FOCUS)

        self.set_events(
            gtk.gdk.EXPOSURE_MASK |
            gtk.gdk.POINTER_MOTION_MASK |
            gtk.gdk.POINTER_MOTION_HINT_MASK |
            gtk.gdk.BUTTON_MOTION_MASK |
            gtk.gdk.BUTTON_PRESS_MASK |
            gtk.gdk.BUTTON_RELEASE_MASK |
            gtk.gdk.FOCUS_CHANGE_MASK |
            gtk.gdk.KEY_PRESS_MASK |
            gtk.gdk.KEY_RELEASE_MASK |
            gtk.gdk.PROXIMITY_IN_MASK |
            gtk.gdk.PROXIMITY_OUT_MASK)

        self.connect('expose_event', self.handle_expose)
        self.connect('focus_in_event', self.handle_focus_in)
        self.connect('focus_out_event', self.handle_focus_out)
        self.connect('key_press_event', self.handle_key_press)
        self.connect('motion_notify_event', self.handle_motion_notify)
        self.connect('button_press_event', self.handle_button_press)
        self.connect('button_release_event', self.handle_button_release)

        if self.running:
            self.startTimer()


    def __del__(self):

        self.stopTimer()

        # TODO: destroy engine and cam

        gtk.DrawingArea.__del__(self)


    def startTimer(self):
        if self.timerActive:
            return

        self.timerId = gobject.timeout_add(self.timeDelay, self.tickTimer)
        self.timerActive = True


    def stopTimer(self):

        # FIXME: Is there some way to immediately cancel self.timerId? 

        self.timerActive = False


    def tickTimer(self):
        if not self.timerActive:
            return False

        #print "tick", self

        if self.running:
            self.c.DoRule()

        self.queue_draw()

        self.timerActive = False

        return False


    def loadGraphics(self, ctx):
        if self.drawTiles:
            if not self.tilesLoaded:
                self.loadTiles(ctx)
                self.tilesLoaded = True
        if self.drawPixels:
            if not self.cmapLoaded:
                self.loadCmap(ctx)
                self.cmapLoaded = True


    def loadCmap(self, ctx):

        self.cmapSurface = cairo.ImageSurface.create_from_png(self.cmapFileName)


    def setScale(self, scale):
       if self.scale == scale:
           return
       
       self.scale = scale
       self.tilesLoaded = False

       self.queue_draw()


    def loadTiles(self, ctx):

        scale = self.scale
        sourceTileSize = int(self.sourceTileSize)

        pngSurface = cairo.ImageSurface.create_from_png(self.tilesFileName)
        pngWidth = int(pngSurface.get_width())
        pngHeight = int(pngSurface.get_height())

        nativeTarget = ctx.get_target()

        if scale == sourceTileSize:

            tileSize = sourceTileSize
            tilesWidth = pngWidth
            tilesHeight = pngHeight
            tilesSurface = nativeTarget.create_similar(cairo.CONTENT_COLOR, tilesWidth, tilesHeight)

            tilesCtx = cairo.Context(tilesSurface)

            tilesCtx.set_source_surface(pngSurface, 0, 0)

            tilesCtx.paint()

        else:

            tileSize = max(1, int(math.floor(scale)))

            pngCols = int(math.floor(pngWidth / sourceTileSize))
            pngRows = int(math.floor(pngHeight / sourceTileSize))

            tilesWidth = tileSize * pngCols
            tilesHeight = tileSize * pngRows

            #print "tileSize", tileSize, "pngCols", pngCols, "pngRows", pngRows, "tilesWidth", tilesWidth, "tilesHeight", tilesHeight

            tilesSurface = nativeTarget.create_similar(cairo.CONTENT_COLOR, tilesWidth, tilesHeight)

            tilesCtx = cairo.Context(tilesSurface)

            # Clip and convert each tile through this server side 565 surface.
            #tileSurface = nativeTarget.create_similar(cairo.CONTENT_COLOR, sourceTileSize, sourceTileSize)

            # Clip each tile through this client side 888 surface.
            tileSurface = pngSurface.create_similar(cairo.CONTENT_COLOR, sourceTileSize, sourceTileSize)

            tileCtx = cairo.Context(tileSurface)

            for row in range(0, pngRows):
                for col in range(0, pngCols):
                    # Make a temporary tile size of a source tile.
                    tileCtx.set_source_surface(
                        pngSurface,
                        -col * sourceTileSize,
                        -row * sourceTileSize)
                    tileCtx.paint()

                    # Scale it down into the tilesSurface.
                    tilesCtx.save()

                    x = col * tileSize
                    y = row * tileSize

                    tilesCtx.rectangle(
                        x,
                        y,
                        tileSize,
                        tileSize)
                    tilesCtx.clip()

                    # Try to keep the tiles centered.
                    fudge = (0.5 * (scale - tileSize))
                    
                    tilesCtx.translate(
                        x + fudge,
                        y + fudge)

                    tilesCtx.scale(
                        float(tileSize) / float(sourceTileSize),
                        float(tileSize) / float(sourceTileSize))
                    tilesCtx.set_source_surface(
                        tileSurface,
                        0,
                        0)
                    tilesCtx.paint()
                    tilesCtx.restore()

        self.tilesSurface = tilesSurface
        self.tilesWidth = tilesWidth
        self.tilesHeight = tilesHeight
        self.tileSize = tileSize

        # This is vonNeumann29 specific.

        tileMap = array.array('I')
        self.tileMap = tileMap
        for i in range(0, 256):
            tileMap.append(CodeToFrame.get(i, 1))

        self.revealCursor()


    def createEngine(self):

        # This is vonNeumann29 specific.

        config = self.config

        name = config['name']
        description = config['description']
        rows = config['rows']
        cols = config['cols']
        neighborhood = config['neighborhood']
        state = config['state']

        self.cellCols = cols
        self.cellRows = rows

        print "COLS", cols, "ROWS", rows, "NEIGHBORHOOD", neighborhood, "NAME", name
        
        self.e = Engine(
            cols,
            rows)
        c = self.e.cam
        self.c = c

        c.neighborhood = neighborhood

        rowLines = [
            rowLine.strip()
            for rowLine in state.split('\n')
            if rowLine.strip()
        ]

        #c.Garble()

        #print "WIDTH", c.width, "HEIGHT", c.height, "FRONTMEM", c.frontMem

        if True:
            for row in range(0, len(rowLines)):
                rowLine = rowLines[row]
                for col in range(0, len(rowLine)):
                    ch = rowLine[col : col + 1]
                    if ch not in CharToCode:
                        print "Unknown char: ", ch
                    else:
                        code = CharToCode[ch]
                        #print row, col, code
                        c.SetCell(col, row, code)

    def handle_expose(self, widget, event):

        #print "handle_expose CamDrawingArea", self, widget, event
        self.draw(widget, event)

        return False


    def draw(self, widget=None, event=None):

        ctxWindow = self.window.cairo_create()

        self.loadGraphics(ctxWindow)

        winRect = self.get_allocation()
        winX = winRect.x
        winY = winRect.y
        winWidth = winRect.width
        winHeight = winRect.height

        windowBuffer = self.windowBuffer
        windowBufferWidth = self.windowBufferWidth
        windowBufferHeight = self.windowBufferHeight
        
        slop = 4

        if ((not windowBuffer) or
            (windowBufferWidth < winWidth) or
            (windowBufferHeight < winHeight)):

            nativeTarget = ctxWindow.get_target()
            windowBufferWidth = max(windowBufferWidth, winWidth + slop)
            windowBufferHeight = max(windowBufferHeight, winHeight + slop)

            #print "!!!! Window Buffer growing from", self.windowBufferWidth, self.windowBufferHeight, "to", windowBufferWidth, windowBufferHeight

            windowBuffer = nativeTarget.create_similar(cairo.CONTENT_COLOR, windowBufferWidth, windowBufferHeight)

            self.windowBuffer = windowBuffer
            self.windowBufferWidth = windowBufferWidth
            self.windowBufferHeight = windowBufferHeight

        ctxWindowBuffer = cairo.Context(windowBuffer)

        debugInset = self.debugInset
        if debugInset:
            # Enable debugInset to help debug the tile drawing optimization,
            # by insetting the view and drawing a rectangle over where it
            # would be clipped. 

            winX += debugInset
            winY += debugInset
            winWidth -= debugInset * 2
            winHeight -= debugInset * 2

            ctxWindowBuffer.save()
            ctxWindowBuffer.set_fill_rule(cairo.FILL_RULE_EVEN_ODD)
            ctxWindowBuffer.rectangle(winRect.x, winRect.y, winRect.width, winRect.height)
            ctxWindowBuffer.rectangle(winX, winY, winWidth, winHeight)
            ctxWindowBuffer.set_source_rgb(1.0, 1.0, 0.0)
            ctxWindowBuffer.fill()

        panX = self.panX
        panY = self.panY
        cellCols = self.cellCols
        cellRows = self.cellRows
        scale = self.scale

        if self.drawTiles:
            # If drawing tiles, then snap to integral tileSize.
            tileSize = self.tileSize
        else:
            # If not drawing tiles, then set tileSize to real scale.
            tileSize = scale
            self.tileSize = scale

        gridWidth = cellCols * tileSize
        gridHeight = cellRows * tileSize
        gridRight = panX + gridWidth
        gridBottom = panY + gridHeight

        gridLeftClipped = max(winX, winX + panX)
        gridTopClipped = max(winY, winY + panY)
        gridRightClipped = min(gridWidth + winX + panX, winX + winWidth)
        gridBottomClipped = min(gridHeight + winY + panY, winY + winHeight)

        colLeft = \
            int(max(0,
                    math.floor(float(0 - panX) / float(tileSize))))

        colRight = \
            int(min(cellCols,
                    math.ceil(float(winWidth - panX) / float(tileSize))))

        rowTop = \
            int(max(0,
                    math.floor(float(0 - panY) / float(tileSize))))

        rowBottom = \
            int(min(cellRows,
                    math.ceil(float(winHeight - panY) / float(tileSize))))

        renderCols = colRight - colLeft
        renderRows = rowBottom - rowTop

        renderX = panX + (colLeft * tileSize)
        renderY = panY + (rowTop * tileSize)
        renderWidth = renderCols * tileSize
        renderHeight = renderRows * tileSize

        self.renderCol = colLeft
        self.renderRow = rowTop
        self.renderCols = renderCols
        self.renderRows = renderRows

        # Make the off-screen buffer to draw the tiles into.

        buffer = self.buffer
        bufferWidth = self.bufferWidth
        bufferHeight = self.bufferHeight
        if ((not buffer) or
            (bufferWidth < renderWidth) or
            (bufferHeight < renderHeight)):

            nativeTarget = ctxWindow.get_target()
            bufferWidth = max(bufferWidth, renderWidth + slop)
            bufferHeight = max(bufferHeight, renderHeight + slop)

            #print "!!!! Buffer growing from", self.bufferWidth, self.bufferHeight, "to", bufferWidth, bufferHeight

            buffer = nativeTarget.create_similar(cairo.CONTENT_COLOR, bufferWidth, bufferHeight)

            self.buffer = buffer
            self.bufferWidth = bufferWidth
            self.bufferHeight = bufferHeight

        ctx = cairo.Context(buffer)

        if ((renderCols > 0) and
            (renderRows > 0)):

            if self.pixelTileScaleCrossfade:
                pixelTileScaleThresholdMin = self.pixelTileScaleThresholdMin
                pixelTileScaleThresholdMax = self.pixelTileScaleThresholdMax
                if scale <= pixelTileScaleThresholdMin:
                    self.drawPixels = True
                    self.drawPixelsAlpha = 1.0
                    self.drawTiles = False
                    self.drawTilesAlpha = 1.0
                elif scale <= pixelTileScaleThresholdMax:
                    alpha = (
                        (scale - pixelTileScaleThresholdMin) /
                        (pixelTileScaleThresholdMax - pixelTileScaleThresholdMin));
                    self.drawPixels = True
                    self.drawPixelsAlpha = (1.0 - alpha)
                    self.drawTiles = True
                    self.drawTilesAlpha = alpha
                else:
                    self.drawPixels = False
                    self.drawPixelsAlpha = 1.0
                    self.drawTiles = True
                    self.drawTilesAlpha = 1.0
                    
                self.loadGraphics(ctxWindow)

            drawTilesAlpha = self.drawTilesAlpha
            drawTiles = self.drawTiles and (drawTilesAlpha > 0.0)
            drawPixelsAlpha = self.drawPixelsAlpha
            drawPixels = self.drawPixels and (drawPixelsAlpha > 0.0)

            # If (not drawing tiles, but drawing pixels, and pixels are not opaque), or
            #    (not drawing tiles, and not drawing pixels), 
            # then fill the inside background.
            if (((not drawTiles) and
                 (drawPixels) and
                 (drawPixelsAlpha < 1.0)) or
                ((not drawTiles) and
                 (not drawPixels))):

                ctx.rectangle(
                    0,
                    0,
                    renderWidth,
                    renderHeight)
                ctx.set_source_rgb(*self.insideBackgroundColor)
                ctx.fill()

            if drawTiles:

                ctx.save()

                tileSize = self.tileSize

                RenderCellTiles(
                    ctx,
                    self.tilesSurface,
                    self.tilesWidth,
                    self.tilesHeight,
                    self.tileMap,
                    max(1, int(tileSize)),
                    self.e.buf,
                    self.e.width,
                    self.e.height,
                    self.e.colBytes,
                    self.e.rowBytes,
                    self.e.typeCode,
                    self.renderCol,
                    self.renderRow,
                    renderCols,
                    renderRows,
                    drawTilesAlpha)

                ctx.restore()

            if drawPixels:

                if ((not self.cmapDest) or
                    (renderCols != self.cmapDestWidth) or
                    (renderRows != self.cmapDestHeight)):
                    self.cmapDest = self.cmapSurface.create_similar(cairo.CONTENT_COLOR_ALPHA, renderCols, renderRows)
                    self.cmapDestWidth = renderCols
                    self.cmapDestHeight = renderRows

                RenderCellPixels(
                    self.cmapDest,
                    self.cmapSurface,
                    self.tileMap,
                    self.e.buf,
                    self.e.width,
                    self.e.height,
                    self.e.colBytes,
                    self.e.rowBytes,
                    self.e.typeCode,
                    self.renderCol,
                    self.renderRow,
                    renderCols,
                    renderRows)

                ctx.save()

                ctx.scale(
                    tileSize,
                    tileSize)

                ctx.set_source_surface(
                    self.cmapDest,
                    0,
                    0)

                # How to I set the filter to cairo.FILTER_FAST?

                if drawPixelsAlpha == 1.0:
                    ctx.paint()
                else:
                    ctx.paint_with_alpha(drawPixelsAlpha)

                ctx.restore()

        ctxWindowBuffer.save()

        ctxWindowBuffer.set_source_surface(
            buffer,
            winX + renderX,
            winY + renderY)

        ctxWindowBuffer.rectangle(
            winX + renderX,
            winY + renderY,
            renderWidth,
            renderHeight)
        ctxWindowBuffer.clip()

        ctxWindowBuffer.paint()

        ctxWindowBuffer.restore()

        # Draw the background outside of the tile grid.
        
        backgroundVisible = False

        # Left Background
        if panX > 0:
            backgroundVisible = True
            ctxWindowBuffer.rectangle(
                winX,
                gridTopClipped,
                panX,
                gridBottomClipped - gridTopClipped)

        # Right Background
        if gridRight < winWidth:
            backgroundVisible = True
            ctxWindowBuffer.rectangle(
                winX + gridRight,
                gridTopClipped,
                winWidth - gridRight,
                gridBottomClipped - gridTopClipped)

        # Top Background
        if panY > 0:
            backgroundVisible = True
            ctxWindowBuffer.rectangle(
                winX,
                winY,
                winWidth,
                panY)

        # Bottom Background
        if gridBottom < winHeight:
            backgroundVisible = True
            ctxWindowBuffer.rectangle(
                winX,
                gridBottom + winY,
                winWidth,
                winHeight - gridBottom)

        if backgroundVisible:
            ctxWindowBuffer.set_source_rgb(*self.outsideBackgroundColor)
            ctxWindowBuffer.fill()

        self.drawCursor(ctxWindowBuffer)

        if debugInset:
            ctxWindowBuffer.rectangle(winX, winY, winWidth, winHeight)
            ctxWindowBuffer.set_source_rgb(1.0, 0.0, 0.0)
            ctxWindowBuffer.stroke()

        ctxWindow.set_source_surface(
            windowBuffer,
            0,
            0)

        ctxWindow.paint()

        if self.running:
            self.startTimer()


    def drawCursor(self, ctx):

        cursorCol = self.cursorCol
        cursorRow = self.cursorRow
        tileSize = self.tileSize
        panX = self.panX
        panY = self.panY

        x = panX + (tileSize * cursorCol)
        y = panY + (tileSize * cursorRow)

        #print "drawCursor", "cursor", cursorCol, cursorRow, "size", tileSize, "pan", panX, panY, "tile", x, y

        ctx.save()

        debugInset = self.debugInset
        if debugInset:
            # Enable debugInset to help debug the tile drawing optimization,
            # by insetting the view and drawing a rectangle over where it
            # would be clipped. 

            x += debugInset
            y += debugInset

        ctx.translate(
            x,
            y)

        ctx.rectangle(
            -2,
            -2,
            tileSize + 4,
            tileSize + 4)

        ctx.set_line_width(
            4.0)

        ctx.set_source_rgb(
            1.0,
            1.0,
            1.0)

        ctx.stroke_preserve()

        ctx.set_line_width(
            2.0)

        ctx.set_source_rgb(
            0.0,
            0.0,
            0.0)

        ctx.stroke()

        ctx.restore()

        cellCode = self.c.GetCell(cursorCol, cursorRow)

        cellName = CodeToName.get(cellCode, None)
        if not cellName:
            print "Unknown cell code:", cellCode
            return

        cellDescription = NameToDescription.get(cellName, None)
        if not cellDescription:
            print "Unknown cell name:", cellName, "code", cellCode
            return

        #print hex(cellCode), cellName, cellDescription


    def setCursorPos(self, col, row):

        col = max(0, min(self.cellCols - 1, col))
        row = max(0, min(self.cellRows - 1, row))

        if ((self.cursorCol != col) or
            (self.cursorRow != row)):
            self.cursorCol = col
            self.cursorRow = row
            self.queue_draw()


    def moveCursorToMouse(self, x, y):

        debugInset = self.debugInset
        if debugInset:
            # Enable debugInset to help debug the tile drawing optimization,
            # by insetting the view and drawing a rectangle over where it
            # would be clipped. 

            x -= debugInset
            y -= debugInset

        tileSize = self.tileSize
        col = int(math.floor((x - self.panX) / tileSize))
        row = int(math.floor((y - self.panY) / tileSize))

        self.setCursorPos(col, row)


    def moveCursor(self, dx, dy):
        col = self.cursorCol + dx
        row = self.cursorRow + dy

        cellCols = self.cellCols
        cellRows = self.cellRows

        if col < 0:
            col = 0
        if col >= cellCols:
            col =  cellCols - 1

        if row < 0:
            row = 0
        if row >= cellRows:
            row = cellRows - 1

        self.setCursorPos(
            col,
            row)

        self.revealCursor()


    def revealCursor(self):
        
        rect = self.get_allocation()
        winX = rect.x
        winY = rect.y
        winWidth = rect.width
        winHeight = rect.height

        debugInset = self.debugInset
        if debugInset:
            # Enable debugInset to help debug the tile drawing optimization,
            # by insetting the view and drawing a rectangle over where it
            # would be clipped. 

            winX += debugInset
            winY += debugInset
            winWidth -= debugInset * 2
            winHeight -= debugInset * 2

        cursorCol = self.cursorCol
        cursorRow = self.cursorRow
        tileSize = self.tileSize
        panX = self.panX
        panY = self.panY

        left = panX + (tileSize * cursorCol)
        right = left + tileSize
        top = panY + (tileSize * cursorRow)
        bottom = top + tileSize

        dx = 0
        dy = 0

        if right >= (winX + winWidth):
            dx = (winX + winWidth) - right
        elif left < winX:
            dx = winX - left

        if bottom >= (winY + winHeight):
            dy = (winY + winHeight) - bottom
        elif top < winY:
            dy = winY - top

        if (dx or dy):
            #print "Panning", dx, dy
            self.panX += dx
            self.panY += dy
            self.queue_draw()


    def handle_focus_in(self, widget, event):

        #print "handle_focus_in CamDrawingArea", self, event
        pass


    def handle_focus_out(self, widget, event):

        #print "handle_focus_out CamDrawingArea", self, event
        pass


    def handle_key_press(self, widget, event):

        # gtk.gdk.Return

        key = event.keyval

        if key == ord('i'):
            self.setScale(self.scale * 1.1)
        elif key == ord('o'):
            self.setScale(self.scale * 0.9)
        elif key == ord('r'):
            self.setScale(self.sourceTileSize)
        elif key == ord('p'):
            self.drawPixels = not self.drawPixels
        elif key == ord('t'):
            self.drawTiles = not self.drawTiles
            # Force re-loading tiles to re-measure tileSize
            self.tilesLoaded = False
        elif key == ord('d'):
            debugInset = self.debugInset
            if debugInset:
                debugInset = 0
            else:
                debugInset = 50
            self.debugInset = debugInset
        elif key == 65362:
            self.moveCursor(0, -1)
        elif key == 65364:
            self.moveCursor(0, 1)
        elif key == 65361:
            self.moveCursor(-1, 0)
        elif key == 65363:
            self.moveCursor(1, 0)
        else:
            print "KEY", event.keyval


    def handle_motion_notify(self, widget, event):

        #print "handle_motion_notify CamDrawingArea", self, widget, event

        if (hasattr(event, 'is_hint') and
            event.is_hint):
            x, y, state = event.window.get_pointer()
        else:
            x = event.x
            y = event.y
            state = event.state

        self.mouseX = x
        self.mouseY = y

        self.moveCursorToMouse(x, y)

        if self.down:
            self.handle_drag(widget, event)


    def handle_drag(self, widget, event):

        #print "handle_drag CamDrawingArea", self

        x = self.mouseX
        y = self.mouseY

        if self.panning:
            #print "PANNING"
            dx = x - self.downX
            dy = y - self.downY
            panX = self.downPanX + dx
            panY = self.downPanY + dy
            if ((panX != self.panX) or
                (panY != self.panY)):
                self.panX = panX
                self.panY = panY
                self.queue_draw()


    def handle_button_press(self, widget, event):
        #print "handle_button_press CamDrawingArea", self

        self.down = True
        self.downX = event.x
        self.downY = event.y

        self.panning = True
        self.downPanX = self.panX
        self.downPanY = self.panY

        self.handle_drag(widget, event)


    def handle_button_release(self, widget, event):

        #print "handle_button_release CamDrawingArea", self

        self.handle_drag(widget, event)

        self.down = False

        self.panning = False


########################################################################


class CamWindow(gtk.Window):


    def __init__(self, **args):
        gtk.Window.__init__(self, **args)

        self.connect('destroy', gtk.main_quit)

        self.set_title("Cellular Automata Machine, by Don Hopkins")

        self.da = CamDrawingArea()
        self.add(self.da)


########################################################################


win = CamWindow()
win.show_all()


########################################################################


gtk.main()


########################################################################
