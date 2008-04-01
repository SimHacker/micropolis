# tiletool.py
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
# Tile Tool
# Don Hopkins


########################################################################
# Import stuff


import gtk
import gobject
import cairo
import math
import array


########################################################################
# TileTool


class TileTool:


    allTools = {}


    def __init__(
        self, 
        name=None,
        cursorVisible=True,
        cursorRows=1,
        cursorCols=1,
        cursorHotCol=0,
        cursorHotRow=0,
        iconNormal=None,
        iconHilite=None,
        iconX=0,
        iconY=0,
        **args):

        self.name = name
        self.cursorVisible = cursorVisible
        self.cursorRows = cursorRows
        self.cursorCols = cursorCols
        self.cursorHotCol = cursorHotCol
        self.cursorHotRow = cursorHotRow
        self.iconNormal = iconNormal
        self.iconHilite = iconHilite
        self.iconX = iconX
        self.iconY = iconY

        if name:
            TileTool.allTools[name] = self


    @classmethod
    def getToolByName(
        self, 
        toolName):

        return self.allTools.get(toolName, None)


    def select(
        self, 
        target):

        pass


    def deselect(
        self, 
        target):

        pass

    
    def getCursorHotSpot(
        self,
        view):

        tileSize = view.tileSize

        return (
            (view.cursorCol + self.cursorHotCol + 0.5) * tileSize,
            (view.cursorRow + self.cursorHotRow + 0.5) * tileSize,
        )


    def drawCursor(
        self, 
        view,
        ctx):

        if not self.cursorVisible:
            return

        cursorX = view.cursorX
        cursorY = view.cursorY
        cursorCol = view.cursorCol
        cursorRow = view.cursorRow
        cursorCols = self.cursorCols
        cursorRows = self.cursorRows
        tileSize = view.tileSize
        panX = view.panX
        panY = view.panY

        x = panX + (tileSize * cursorCol)
        y = panY + (tileSize * cursorRow)

        #print "drawCursor", "cursor", cursorCol, cursorRow, cursorCols, cursorRow, "size", tileSize, "pan", panX, panY, "tile", x, y

        ctx.save()

        ctx.translate(
            x,
            y)

        ctx.rectangle(
            -2,
            -2,
            (cursorCols * tileSize) + 4,
            (cursorRows * tileSize) + 4)

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


    def setCursorPos(
        self,
        view,
        x,
        y,
        forceRedraw=False):

        tileSize = view.tileSize
        col = int(math.floor(x / tileSize)) - self.cursorHotCol
        row = int(math.floor(y / tileSize)) - self.cursorHotRow

        cursorRows = self.cursorRows
        cursorCols = self.cursorCols
        col = max(0, min(view.worldCols - cursorCols, col))
        row = max(0, min(view.worldRows - cursorRows, row))

        if cursorCols:
            cursorXChanged = view.cursorCol != col
        else:
            cursorXChanged = view.cursorX != x
        if cursorRows:
            cursorYChanged = view.cursorRow != row
        else:
            cursorYChanged = view.cursorY != y

        view.cursorX = x
        view.cursorY = y

        if (forceRedraw or
            cursorXChanged or
            cursorYChanged):
            view.cursorCol = col
            view.cursorRow = row
            view.cursorMoved()


    def moveCursor(
        self,
        view,
        dx,
        dy):
        
        col = view.cursorCol + dx
        row = view.cursorRow + dy

        tileSize = view.tileSize
        tileMiddle = int(math.floor((tileSize / 2.0) + 0.5))
        cursorCols = self.cursorCols
        cursorRows = self.cursorRows
        worldCols = view.worldCols
        worldRows = view.worldRows

        if col < 0:
            col = 0
        if col >= (worldCols - cursorCols):
            col =  (worldCols - cursorCols) - 1

        if row < 0:
            row = 0
        if row >= (worldRows - cursorRows):
            row = (worldRows - cursorRows) - 1

        x = ((col + self.cursorHotCol) * tileSize) + tileMiddle
        y = ((row + self.cursorHotRow) * tileSize) + tileMiddle

        self.setCursorPos(
            view,
            x,
            y)

        self.revealCursor(
            view)


    def revealCursor(
        self,
        view):
        
        rect = view.get_allocation()
        winX = rect.x
        winY = rect.y
        winWidth = rect.width
        winHeight = rect.height

        cursorCol = view.cursorCol
        cursorRow = view.cursorRow
        cursorCols = self.cursorCols
        cursorRows = self.cursorRows
        tileSize = view.tileSize
        panX = view.panX
        panY = view.panY

        left = panX + (tileSize * cursorCol)
        right = left + (max(1, cursorCols) * tileSize)
        top = panY + (tileSize * cursorRow)
        bottom = top + (max(1, cursorRows) * tileSize)

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
            view.panBy(dx, dy)


    # Return True if handled.
    def handleKeyDown(self, view, event):

        key = event.keyval

        if key == 65362: # left arrow
            self.moveCursor(view, 0, -1)
            return True
        elif key == 65364: # right arrow
            self.moveCursor(view, 0, 1)
            return True
        elif key == 65361: # up arrow
            self.moveCursor(view, -1, 0)
            return True
        elif key == 65363: # down arrow
            self.moveCursor(view, 1, 0)
            return True

        return False


    def applyTool(
        self, 
        view):

		pass


    # Return True if handled.
    def handleKeyUp(self, view, event):

        return False


    def handleMousePoint(self, view, event):

        pass


    def handleMouseDown(self, view, event):

        pass


    def handleMouseDrag(self, view, event):

        pass


    def handleMouseUp(self, view, event):

        pass


    def tick(self, view):

        pass


########################################################################
# PanTool


class PanTool(TileTool):


    def __init__(
        self, 
        **args):

        TileTool.__init__(
            self,
            **args)

        self.startMouseX = 0
        self.startMouseY = 0
        self.startPanX = 0
        self.startPanY = 0
        self.autoScrollBorder = 20
        self.autoScrollStep = 10


    def startPanning(
        self,
        view):

        self.startMouseX = view.mouseX
        self.startMouseY = view.mouseY
        self.startPanX = view.panX
        self.startPanY = view.panY


    def handlePan(
        self,
        view):

        #print "PanTool handlePan", self, view

        x = view.mouseX
        y = view.mouseY

        #print "PANNING"
        dx = x - self.startMouseX
        dy = y - self.startMouseY
        panX = self.startPanX + dx
        panY = self.startPanY + dy
        if ((panX != view.panX) or
            (panY != view.panY)):
            view.panX = panX
            view.panY = panY
            view.queue_draw()


    def handleMousePoint(
        self, 
        view, 
        event):

        self.handlePan(
            view)


    def handleMouseDown(
        self, 
        view, 
        event):

        #print "PanTool handleMouseDown"
        pass


    def handleMouseDrag(
        self, 
        view, 
        event):

        self.handleMousePoint(
            view,
            event)

    def handleMouseUp(self, view, event):

        #print "PanTool handleMouseUp"
        pass


    def handleKeyDown(
        self, 
        view, 
        event):

        pass


    def handleKeyUp(self, view, event):

        key = event.keyval
        #print "PanTool handleKeyRelease", key, view.trackingToolTrigger
        if key == view.trackingToolTrigger:
            view.trackingTool = None


    def tick(
        self, 
        view):

        xDirection, yDirection = \
            self.getAutoScrollDirections(
                view)

        if xDirection or yDirection:
            self.doAutoScroll(
                view,
                xDirection, 
                yDirection)


    def getAutoScrollDirections(
        self,
        view):

        winRect = view.get_allocation()
        winX = winRect.x
        winY = winRect.y
        winWidth = winRect.width
        winHeight = winRect.height

        x = view.mouseX
        y = view.mouseY

        border = self.autoScrollBorder

        #print ("cur", x, y, "pan", view.panX, view.panY, "border", border, "win", winWidth, winHeight)

        if x < border:
            xDirection = -1
        elif x >= (winWidth - border):
            xDirection = 1
        else:
            xDirection = 0

        if y < border:
            yDirection = -1
        elif y >= (winHeight - border):
            yDirection = 1
        else:
            yDirection = 0

        return (
            xDirection,
            yDirection)


    def doAutoScroll(
        self,
        view,
        dx,
        dy):

        # TODO: Make the autoscroll speed be based on the elapsed time

        #print "doAutoScroll", dx, dy

        step = self.autoScrollStep

        self.startMouseX -= dx * step
        self.startMouseY -= dy * step

        self.handlePan(view)
        view.updateCursorPosition(None)


    def drawCursor(
        self, 
        view,
        ctx):

        pass


########################################################################
