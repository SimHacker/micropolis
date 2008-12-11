# tiledrawingarea.py
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
# Tile Drawing Area
# Don Hopkins


########################################################################
# Import stuff


import gtk
import gobject
import cairo
import math
import array


########################################################################
# Import our modules


import tileengine
import tiletool


########################################################################


class TileDrawingArea(gtk.DrawingArea):


    def __init__(
        self,
        tengine=None,
        tilesFileName='images/tiles.png',
        sourceTileSize=32,
        tileCount=256,
        maxSurfaceSize=512,
        worldCols=256,
        worldRows=256,
        renderCol=0,
        renderRow=0,
        renderCols=256,
        renderRows=256,
        timeDelay=50,
        panX=0,
        panY=0,
        scale=1.0,
        outsideBackgroundColor=(0.5, 0.5, 0.5),
        insideBackgroundColor=(0.0, 0.0, 0.0),
        scrollWheelZoomScale=1.1,
        selectedTool=None,
        keyable=True,
        pannable=True,
        panKeys=(65505, 65506,), # left shift, right shift
        zoomable=True,
        clickable=True,
        clickButton=1, # left button
        menuable=True,
        menuButton=3, # right button
        toolPie=None,
        showCursor=True,
        **args):

        gtk.DrawingArea.__init__(self, **args)

        self.tengine = tengine
        self.tilesFileName = tilesFileName
        self.sourceTileSize = sourceTileSize
        self.tileCount = tileCount
        self.maxSurfaceSize = maxSurfaceSize
        self.worldRows = worldRows
        self.worldCols = worldCols
        self.renderCol = renderCol
        self.renderRow = renderRow
        self.renderCols = renderCols
        self.renderRows = renderRows
        self.timeDelay = timeDelay
        self.panX = panX
        self.panY = panY
        self.scale = scale
        self.outsideBackgroundColor = outsideBackgroundColor
        self.insideBackgroundColor = insideBackgroundColor
        self.scrollWheelZoomScale = scrollWheelZoomScale
        self.selectedTool = selectedTool
        self.keyable = keyable
        self.pannable = pannable
        self.panKeys = panKeys
        self.zoomable = zoomable
        self.clickable = clickable
        self.clickButton = clickButton
        self.menuable = menuable
        self.menuButton = menuButton
        self.toolPie = toolPie
        self.showCursor = showCursor

        self.tilesSourceSurface = None
        self.tilesSourceWidth = None
        self.tilesSourceHeight = None
        self.tileSurface = None
        self.tileCtx = None
        self.tilesWidth = 0
        self.tilesHeight = 0
        self.tileFunction = None
        self.tileMap = None
        self.tileCache = None
        self.tileCacheSurfaces = None
        self.tileState = None
        self.tileCacheCount = 0
        self.down = False
        self.downX = 0
        self.downY = 0
        self.tileSize = 1
        self.buffer = None
        self.bufferWidth = 0
        self.bufferHeight = 0
        self.windowBuffer = None
        self.windowBufferWidth = 0
        self.windowBufferHeight = 0
        self.cursorX = 0
        self.cursorY = 0
        self.cursorRow = 0
        self.cursorCol = 0
        self.mouseX = 0
        self.mouseY = 0
        self.trackingTool = None
        self.trackingToolTrigger = None

        self.tilesLoaded = False

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

        self.connect('expose_event', self.handleExpose)
        self.connect('focus_in_event', self.handleFocusIn)
        self.connect('focus_out_event', self.handleFocusOut)
        self.connect('key_press_event', self.handleKeyPress)
        self.connect('key_release_event', self.handleKeyRelease)
        self.connect('motion_notify_event', self.handleMotionNotify)
        self.connect('button_press_event', self.handleButtonPress)
        self.connect('button_release_event', self.handleButtonRelease)
        self.connect('scroll_event', self.handleMouseScroll)


    def __del__(
        self):

        self.destroyEngine()

        gtk.DrawingArea.__del__(self)


    def tickTimer(
        self):
        
        self.tickEngine()
        self.queue_draw()


    def tickActiveTool(
        self):

        tool = self.getActiveTool()
        if tool:
            tool.tick(self)


    def tickEngine(
        self):

        pass


    def getActiveTool(
        self):

        return self.trackingTool or self.selectedTool


    def changeScale(
        self,
        scale):

        # Keep the cursor centered on the same place on the screen.

        #print "==== CHANGESCALE", scale

        tileSize = self.tileSize
        #print "TILESIZE", tileSize

        panX = self.panX
        panY = self.panY
        #print "PAN", panX, panY

        # TODO: take into account cursor hot spot, or precise cursor position?

        tool = self.getActiveTool()
        if tool:
            cursorCol = self.cursorCol
            cursorRow = self.cursorRow
            cursorCols = tool.cursorCols
            cursorRows = tool.cursorRows
        else:
            cursorCol = int(math.floor(self.cursorX / tileSize)) # TODO: get rid of cursorX and cursorY and track it in the tool instead.
            cursorRow = int(math.floor(self.cursorY / tileSize)) # TODO: get rid of cursorX and cursorY and track it in the tool instead.
            cursorRows = 1
            cursorCols = 1
        #print "CURSOR", cursorCol, cursorRow, cursorCols, cursorRows

        cursorScreenX = panX + (cursorCol * tileSize)
        cursorScreenY = panY + (cursorRow * tileSize)
        #print "CURSORSCREEN", cursorScreenX, cursorScreenY

        cursorCenterScreenX = cursorScreenX + (0.5 * cursorCols * tileSize)
        cursorCenterScreenY = cursorScreenY + (0.5 * cursorRows * tileSize)
        #print "CURSORCENTERSCREEN", cursorCenterScreenX, cursorCenterScreenY

        self.setScale(scale)

        tileSize = self.tileSize
        #print "TILESIZE", tileSize
        
        cursorScreenX = cursorCenterScreenX - (0.5 * cursorCols * tileSize)
        cursorScreenY = cursorCenterScreenY - (0.5 * cursorRows * tileSize)
        #print "CURSORSCREEN", cursorScreenX, cursorScreenY
        
        panX = cursorScreenX - (cursorCol * tileSize)
        panY = cursorScreenY - (cursorRow * tileSize)
        #print "PAN", panX, panY
        
        self.panX = panX
        self.panY = panY


    def setScale(
        self,
        scale):

        if self.scale == scale:
            return

        self.scale = scale

        ctxWindow = self.window.cairo_create()
        self.loadGraphics(ctxWindow, True)

        self.queue_draw()


    def loadGraphics(
        self,
        ctx,
        force=False):
        
        if force or (not self.tilesLoaded):
            self.loadTiles(ctx)
            self.tilesLoaded = True


    def loadSurfaceFromPNG(self, fileName):
        surface = cairo.ImageSurface.create_from_png(fileName)
        return (
            surface,
            surface.get_width(),
            surface.get_height())


    def loadTiles(
        self,
        ctx):

        scale = self.scale
        sourceTileSize = int(self.sourceTileSize)

        (
            tilesSourceSurface,
            tilesSourceWidth,
            tilesSourceHeight,
        ) = self.loadSurfaceFromPNG(self.tilesFileName)

        tilesSourceCols = int(math.floor(tilesSourceWidth / sourceTileSize))
        tilesSourceRows = int(math.floor(tilesSourceHeight / sourceTileSize))

        nativeTarget = ctx.get_target()

        if scale == 1.0:

            # We don't need any scaling, so copy directly to a surface of the same size.

            # All tiles are pre-cached.
            surfaceIndex = 0
            tileCache = array.array('i')
            self.tileCache = tileCache
            for tile in range(0, self.tileCount):
                col = tile % tilesSourceCols
                row = int(math.floor(tile / tilesSourceCols))
                tileCache.append(1)
                tileCache.append(surfaceIndex)
                tileCache.append(col * sourceTileSize)
                tileCache.append(row * sourceTileSize)

            #print "ALL TILES ARE PRE-CACHED", self.tileCount, len(self.tileCache)

            tileSize = sourceTileSize
            tilesWidth = tilesSourceWidth
            tilesHeight = tilesSourceHeight

            #print "MAKING TILESSURFACE", tilesWidth, tilesHeight
            tilesSurface = nativeTarget.create_similar(cairo.CONTENT_COLOR, tilesWidth, tilesHeight)
            #print "DONE"

            tilesCtx = cairo.Context(tilesSurface)
            tilesCtx.set_source_surface(tilesSourceSurface, 0, 0)
            tilesCtx.set_antialias(cairo.ANTIALIAS_NONE)
            tilesCtx.paint()

            self.tileCacheSurfaces = [tilesSurface]
            self.tileCacheCount = self.tileCount

            tileSurface = None
            tileCtx = None

        else:

            # No tiles are pre-cached.
            self.tileCache = array.array('i', (0, 0, 0, 0,) * (self.tileCount))

            #print "NO TILES ARE PRE-CACHED", self.tileCount, len(self.tileCache)

            tileSize = max(1, int(math.floor(0.5 + (scale * sourceTileSize))))

            tilesWidth = tileSize * tilesSourceCols
            tilesHeight = tileSize * tilesSourceRows

            #print "tileSize", tileSize, "tilesSourceCols", tilesSourceCols, "tilesSourceRows", tilesSourceRows, "tilesWidth", tilesWidth, "tilesHeight", tilesHeight

            # Clip each tile through this client side 888 surface.
            tileSurface = tilesSourceSurface.create_similar(cairo.CONTENT_COLOR, sourceTileSize, sourceTileSize)
            tileCtx = cairo.Context(tileSurface)

            tileCtx.set_antialias(cairo.ANTIALIAS_NONE)
        
            self.tileCacheSurfaces = []
            self.tileCacheCount = 0

        self.tilesSourceWidth = tilesSourceWidth
        self.tilesSourceHeight = tilesSourceHeight
        self.tilesSourceRows = tilesSourceRows
        self.tilesSourceCols = tilesSourceCols
        self.tilesSourceSurface = tilesSourceSurface
        self.tileSize = tileSize
        self.tilesWidth = tilesWidth
        self.tilesHeight = tilesHeight
        self.tileSurface = tileSurface
        self.tileCtx = tileCtx

        self.makeTileMap()

        tool = self.getActiveTool()
        if tool:
            tool.revealCursor(self)


    def makeTileMap(
        self):
        
        tileMap = array.array('i')
        self.tileMap = tileMap
        for i in range(0, self.tileCount):
            tileMap.append(i)


    def getTile(
        self,
        col,
        row):

        return 0


    def createEngine(
        self):

        if not self.tengine:

            tengine = tileengine.TileEngine()
            self.tengine = tengine

            self.configTileEngine(tengine)


    def getToolPie(self):

        toolPie = self.toolPie
        if toolPie:
            return toolPie

        self.makeToolPie()

        return self.toolPie


    def makeToolPie(self):

        pass


    def configTileEngine(
        self,
        tengine):

        tengine.setBuffer(None)
        tengine.width = 1
        tengine.height = 1
        tengine.colBytes = 0
        tengine.rowBytes = 0
        tengine.typeCode = 'H'
        tengine.tileMask = 0xffffffff


    def destroyEngine(
        self):

        pass


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

        self.loadGraphics(ctxWindow)

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

            #print "Window Buffer growing from", self.windowBufferWidth, self.windowBufferHeight, "to", windowBufferWidth, windowBufferHeight

            windowBuffer = nativeTarget.create_similar(cairo.CONTENT_COLOR, windowBufferWidth, windowBufferHeight)

            self.windowBuffer = windowBuffer
            self.windowBufferWidth = windowBufferWidth
            self.windowBufferHeight = windowBufferHeight

        ctxWindowBuffer = cairo.Context(windowBuffer)
        ctxWindowBuffer.set_antialias(cairo.ANTIALIAS_NONE)

        panX = self.panX
        panY = self.panY
        worldCols = self.worldCols
        worldRows = self.worldRows
        scale = self.scale
        tileSize = self.tileSize

        worldWidth = worldCols * tileSize
        worldHeight = worldRows * tileSize
        worldRight = panX + worldWidth
        worldBottom = panY + worldHeight

        #print "WORLDRIGHT", worldRight, "WORLDBOTTOM", worldBottom

        worldLeftClipped = max(0, panX)
        worldTopClipped = max(0, panY)
        worldRightClipped = min(worldWidth + panX, winWidth)
        worldBottomClipped = min(worldHeight + panY, winHeight)

        colLeft = \
            int(max(0,
                    math.floor(float(0 - panX) /
                               float(tileSize))))

        colRight = \
            int(min(worldCols,
                    math.ceil(float(winWidth - panX) /
                              float(tileSize))))

        #print "COLLEFT", colLeft, "COLRIGHT", colRight, "winWidth - panX", winWidth - panX

        rowTop = \
            int(max(0,
                    math.floor(float(0 - panY) /
                               float(tileSize))))

        rowBottom = \
            int(min(worldRows,
                    math.ceil(float(winHeight - panY) /
                              float(tileSize))))

        #print "ROWTOP", rowTop, "ROWBOTTOM", rowBottom, "winHeight - panY", winHeight - panY

        renderCols = colRight - colLeft
        renderRows = rowBottom - rowTop

        #print "PANX", panX, "PANY", panY, "TILESIZE", tileSize, "COLS", renderCols, "ROWS", renderRows

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

            #print "Buffer growing from", self.bufferWidth, self.bufferHeight, "to", bufferWidth, bufferHeight

            buffer = nativeTarget.create_similar(cairo.CONTENT_COLOR, bufferWidth, bufferHeight)

            self.buffer = buffer
            self.bufferWidth = bufferWidth
            self.bufferHeight = bufferHeight

        ctx = cairo.Context(buffer)

        if ((renderCols > 0) and
            (renderRows > 0)):

            #print "renderTilesLazy BEGIN", self.generateTile
            self.tengine.renderTilesLazy(
                ctx,
                self.tileFunction,
                self.tileMap,
                self.tileSize,
                self.renderCol,
                self.renderRow,
                renderCols,
                renderRows,
                1.0,
                self.generateTile,
                self.tileCache,
                self.tileCacheSurfaces,
                self.tileState)
            #print "renderTilesLazy END"

        ctxWindowBuffer.save()

        ctxWindowBuffer.set_source_surface(
            buffer,
            renderX,
            renderY)

        ctxWindowBuffer.rectangle(
            renderX,
            renderY,
            renderWidth,
            renderHeight)

        ctxWindowBuffer.clip()

        ctxWindowBuffer.paint()

        ctxWindowBuffer.restore()

        # Draw the background outside of the tile world.
        
        backgroundVisible = False

        # Left Background
        if panX > 0:
            backgroundVisible = True
            ctxWindowBuffer.rectangle(
                0,
                worldTopClipped,
                panX,
                worldBottomClipped - worldTopClipped)

        # Right Background
        if worldRight < winWidth:
            backgroundVisible = True
            ctxWindowBuffer.rectangle(
                worldRight,
                worldTopClipped,
                winWidth - worldRight,
                worldBottomClipped - worldTopClipped)

        # Top Background
        if panY > 0:
            backgroundVisible = True
            ctxWindowBuffer.rectangle(
                0,
                0,
                winWidth,
                panY)

        # Bottom Background
        if worldBottom < winHeight:
            backgroundVisible = True
            ctxWindowBuffer.rectangle(
                0,
                worldBottom,
                winWidth,
                winHeight - worldBottom)

        if backgroundVisible:
            ctxWindowBuffer.set_source_rgb(*self.outsideBackgroundColor)
            ctxWindowBuffer.fill()

        self.drawOverlays(ctxWindowBuffer)

        ctxWindow.set_source_surface(
            windowBuffer,
            0,
            0)

        ctxWindow.paint()


    # This function is called from the C++ code in self.tengine.renderTilesLazy.
    # It renders a tile, and returns a tuple with a surface index, tile x and tile y position. 
    # This function is totally in charge of the scaled tile cache, and can implement a variety 
    # variety of different policies. 
    def generateTile(
        self,
        tile):

        #print "======== GENERATETILE", tile

        # Get the various tile measurements.
        sourceTileSize = self.sourceTileSize
        tilesSourceSurface = self.tilesSourceSurface
        tilesSourceCols = self.tilesSourceCols
        tileSize = self.tileSize
        tileCtx = self.tileCtx
        tileSurface = self.tileSurface
        tileCacheSurfaces = self.tileCacheSurfaces
        scale = self.scale
        maxSurfaceSize = self.maxSurfaceSize

        # Calculate the measurements per surface, each of which contains one or more tiles, 
        # depending on the tile size.
        # If the tiles are small, we will put a lot of them per surface, but as they get
        # bigger, we limit the size of the surface by reducing the number of tiles, so the 
        # surfaces to not get too big. 

        tileColsPerSurface = max(1, int(math.floor(maxSurfaceSize / tileSize)))
        #print "tileColsPerSurface", tileColsPerSurface

        tilesPerSurface = tileColsPerSurface * tileColsPerSurface
        #print "tilesPerSurface", tilesPerSurface

        surfaceSize = tileColsPerSurface * tileSize
        #print "surfaceSize", surfaceSize

        cacheTile = self.tileCacheCount
        self.tileCacheCount += 1

        surfaceIndex = int(math.floor(cacheTile / tilesPerSurface))
        #print "surfaceIndex", surfaceIndex

        ctxWindow = None
        nativeTarget = None
        while len(tileCacheSurfaces) <= surfaceIndex:
            #print "MAKING TILESSURFACE", len(tileCacheSurfaces), tilesPerSurface, surfaceSize
            if nativeTarget == None:
                ctxWindow = self.window.cairo_create()
                nativeTarget = ctxWindow.get_target()
            tilesSurface = nativeTarget.create_similar(cairo.CONTENT_COLOR, surfaceSize, surfaceSize)
            tileCacheSurfaces.append(tilesSurface)
            #print "DONE"

        tilesSurface = tileCacheSurfaces[surfaceIndex]
        tileOnSurface = cacheTile % tilesPerSurface
        #print "tileOnSurface", tileOnSurface
        tileCol = tileOnSurface % tileColsPerSurface
        tileRow = int(math.floor(tileOnSurface / tileColsPerSurface))
        #print "tileCol", tileCol, "tileRow", tileRow
        tileX = tileCol * tileSize
        tileY = tileRow * tileSize
        #print "tileX", tileX, "tileY", tileY
        sourceTileCol = tile % tilesSourceCols
        sourceTileRow = int(math.floor(tile / self.tilesSourceCols))
        #print "sourceTileCol", sourceTileCol, "sourceTileRow", sourceTileRow

        # Make a temporary tile the size of a source tile.
        tileCtx.set_source_surface(
            tilesSourceSurface,
            -sourceTileCol * sourceTileSize,
            -sourceTileRow * sourceTileSize)
        tileCtx.paint()

        tilesCtx = cairo.Context(tilesSurface)
        tilesCtx.set_source_surface(tilesSourceSurface, 0, 0)

        # Scale it down into the tilesSurface.
        tilesCtx.save()

        x = tileCol * tileSize
        y = tileRow * tileSize

        tilesCtx.rectangle(
            x,
            y,
            tileSize,
            tileSize)
        tilesCtx.clip()

        # Try to keep the tiles centered.
        fudge = 0 # (0.5 * (scale - tileSize))

        x += fudge
        y += fudge

        tilesCtx.translate(
            x,
            y)

        #print "X", x, "Y", y, "FUDGE", fudge, "SCALE", scale, "TILESIZE", tileSize

        # Make it a pixel bigger to eliminate the fuzzy black edges.
        #zoomScale = float(tileSize) / float(sourceTileSize)
        zoomScale = float(tileSize) / float(sourceTileSize - 1.0)

        #print "ZOOMSCALE", zoomScale, "TILESIZE", tileSize, "SOURCETILESIZE", sourceTileSize

        tilesCtx.scale(
            zoomScale,
            zoomScale)

        #smear = True
        smear = False

        if smear:

            fudge = 0.5
            for dx, dy in (
                (-fudge, -fudge),
                ( fudge, -fudge),
                (-fudge,  fudge),
                ( fudge,  fudge),
                (-fudge,  0.0),
                ( fudge,  0.0),
                ( 0.0, -fudge),
                ( 0.0,  fudge),
                ( 0.0,  0.0),
            ):
                tilesCtx.save()

                tilesCtx.translate(dx, dy)

                tilesCtx.set_source_surface(
                    tileSurface,
                    0,
                    0)
                tilesCtx.paint()

                tilesCtx.restore()

        else:

            tilesCtx.set_source_surface(
                tileSurface,
                -0.5,
                -0.5)
            tilesCtx.paint()

        tilesCtx.restore()

        #print "GENERATETILE", tile, "surfaceIndex", surfaceIndex, "tileX", tileX, "tileY", tileY

        result = (surfaceIndex, tileX, tileY)
        #print "GENERATETILE", tile, "RESULT", result
        return result


    def drawOverlays(
        self,
        ctx):

        if self.showCursor:
            tool = self.getActiveTool()
            if tool:
                tool.drawCursor(self, ctx)


    def cursorMoved(self):
        self.queue_draw()


    def moveCursorToMouse(
        self,
        x,
        y):

        tool = self.getActiveTool()
        if tool:
            tool.setCursorPos(
                self,
                x - self.panX, 
                y - self.panY)


    def getEventXY(
        self,
        event):
        if (hasattr(event, 'is_hint') and
            event.is_hint):
            x, y, state = event.window.get_pointer()
        else:
            x = event.x
            y = event.y

        tileSize = self.tileSize

        return (
            (x - self.panX) / tileSize,
            (y - self.panY) / tileSize,
        )


    def getEventColRow(
        self,
        event):
        if (hasattr(event, 'is_hint') and
            event.is_hint):
            x, y, state = event.window.get_pointer()
        else:
            x = event.x
            y = event.y

        tileSize = self.tileSize
        col = int((x - self.panX) / tileSize)
        row = int((y - self.panY) / tileSize)

        return (col, row)


    def panTo(self, x, y):
        self.panX = x
        self.panY = y
        self.queue_draw()


    def panBy(self, dx, dy):
        self.panTo(
            self.panX + dx,
            self.panY + dy)


    def selectToolByName(self, toolName):
        print "selectToolByName", toolName

        tool = tiletool.TileTool.getToolByName(toolName)

        lastTool = self.selectedTool
        if lastTool:
            tool.deselect(self)

        if tool:
            tool.select(self)

        self.selectedTool = tool


    def handleFocusIn(
        self,
        widget,
        event):

        #print "handleFocusIn TileDrawingArea", self, event
        pass


    def handleFocusOut(
        self,
        widget,
        event):

        #print "handleFocusOut TileDrawingArea", self, event
        pass


    def handleKeyPress(
        self,
        widget,
        event):

        if not self.keyable:
            return

        key = event.keyval

        #print "KEYPRESS", key

        if (self.pannable and
            (not self.trackingTool) and
            (key in self.panKeys)):
            panTool = tiletool.TileTool.getToolByName('Pan')
            #print "panTool", panTool
            if panTool:
                self.trackingToolTrigger = key
                self.trackingTool = panTool
                panTool.startPanning(self)
                #print "Activated panTool", panTool
                return

        tool = self.getActiveTool()
        if tool:
            if tool.handleKeyDown(self, event):
                return

        # TODO: This might be handled by the pan tool. 
        if self.zoomable:
            if key == ord('i'):
                self.changeScale(self.scale * 1.1)
            elif key == ord('I'):
                self.changeScale(self.scale * 2.0)
            elif key == ord('o'):
                self.changeScale(self.scale * 0.9)
            elif key == ord('O'):
                self.changeScale(self.scale * 0.5)
            elif key == ord('r'):
                self.changeScale(1.0)


    def handleKeyRelease(
        self,
        widget,
        event):

        if not self.keyable:
            return

        #print "KEYRELEASE", event.keyval

        tool = self.getActiveTool()
        if tool:
            if tool.handleKeyUp(self, event):
                return

        key = event.keyval


    def handleMotionNotify(
        self,
        widget,
        event):

        #print "handleMotionNotify TileDrawingArea", self, widget, event

        self.updateCursorPosition(event)


    def updateCursorPosition(
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

        self.mouseX = x
        self.mouseY = y

        self.moveCursorToMouse(x, y)

        if self.down:
            self.handleMouseDrag(event)
        else:
            self.handleMousePoint(event)


    def handleMouseDrag(
        self,
        event):

        if not self.clickable:
            return

        tool = self.getActiveTool()
        if tool:
            tool.handleMouseDrag(self, event)


    def handleMousePoint(
        self,
        event):

        if not self.clickable:
            return

        tool = self.getActiveTool()
        #print "handleMousePoint", tool
        if tool:
            tool.handleMousePoint(self, event)


    def handleToolPieButtonPress(
        self,
        widget,
        event):

        #print "handleButtonPress TileDrawingArea", self

        #print "EVENT", event
        #print dir(event)

        if (self.clickable and
            (event.button == self.clickButton)):

            self.down = True
            self.downX = event.x
            self.downY = event.y

            tool = self.getActiveTool()
            #print "Active tool:", tool
            if tool:
                tool.handleMouseDown(self, event)

        elif (self.menuable and 
              (event.button == self.menuButton)):

            toolPie = self.getToolPie()

            if toolPie:

                win_x, win_y, state = event.window.get_pointer()

                #print "POP UP TOOLPIE", toolPie, win_x, win_y, state
                #print "WIN", win_x, win_y

                x, y = event.get_root_coords()

                #print "ROOT", x, y

                toolPie.popup(x, y, False)


    def handleButtonPress(
        self,
        widget,
        event):

        if not self.clickable:
            return

        self.down = True
        self.downX = event.x
        self.downY = event.y

        tool = self.getActiveTool()
        #print "Active tool:", tool
        if tool:
            tool.handleMouseDown(self, event)


    def handleButtonRelease(
        self,
        widget,
        event):

        #print "handleButtonRelease TileDrawingArea", self

        if not self.clickable:
            return

        self.handleMouseDrag(event)

        self.down = False

        tool = self.getActiveTool()
        if tool:
            tool.handleMouseUp(self, event)


    def handleMouseScroll(
        self,
        widget,
        event):

        if not self.zoomable:
            return

        direction = event.direction

        if direction == gtk.gdk.SCROLL_UP:
            self.changeScale(self.scale * self.scrollWheelZoomScale)
        elif direction == gtk.gdk.SCROLL_DOWN:
            self.changeScale(self.scale / self.scrollWheelZoomScale)


########################################################################
