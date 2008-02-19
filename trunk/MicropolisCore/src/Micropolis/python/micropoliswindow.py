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
  '../swig',
  'ReleaseSymbols',
  'build/lib.macosx-10.5-i386-2.5',
  '../../TileEngine/swig',
  '../../TileEngine/python',
  '../../TileEngine/python/ReleaseSymbols',
  '../../TileEngine/python/lib.macosx-10.5-i386-2.5',
):
    sys.path.append(os.path.join(cwd, relPath))

import micropolis
import micropolismodel
import micropolisutils
import micropolispiemenus
import tileengine
import tilewindow


########################################################################
# Utilities


def PRINT(*args):
    print args


########################################################################


class Tool:


    def __init__(self, **args):
        pass


########################################################################


class MicropolisDrawingArea(tilewindow.TileDrawingArea):


    def __init__(
        self,
        engine=None,
        toolPie=None,
        **args):

        args['tileCount'] = 960
        args['sourceTileSize'] = 16
        args['worldCols'] = micropolis.WORLD_X
        args['worldRows'] = micropolis.WORLD_Y

        self.engine = engine
        self.toolPie = toolPie

        tilewindow.TileDrawingArea.__init__(self, **args)


    def createEngine(self):

        # Get our nice scriptable subclass of the SWIG Micropolis wrapper object. 
        engine = micropolismodel.MicropolisModel()
        self.engine = engine

        print "Created Micropolis simulator engine:", engine

        # Hook the engine up so it has a handle on its Python object side. 
        engine.userData = micropolis.GetPythonCallbackData(engine)

        # Hook up the language independent callback mechanism to our low level C++ Python dependent callback handler. 
        engine.callbackHook = micropolis.GetPythonCallbackHook()

        # Hook up the Python side of the callback handler, defined in our scripted subclass of the SWIG wrapper. 
        engine.callbackData = micropolis.GetPythonCallbackData(engine.invokeCallback)

        engine.ResourceDir = 'res'
        engine.InitGame()

        # Load a city file.
        cityFileName = 'cities/haight.cty'
        print "Loading city file:", cityFileName
        engine.loadFile(cityFileName)

        # Initialize the simulator engine.

        engine.Resume()
        engine.setSpeed(2)
        engine.setSkips(10)
        engine.SetFunds(1000000000)
        engine.autoGo = 0
        engine.CityTax = 8

        # Testing...

        self.cursorCols = 3
        self.cursorRows = 3
        self.cursorHotCol = 1
        self.cursorHotRow = 1

        tilewindow.TileDrawingArea.createEngine(self)


    def configTileEngine(self, tengine):

        engine = self.engine
        tengine.setBuffer(engine.getMapBuffer())
        tengine.width = micropolis.WORLD_X
        tengine.height = micropolis.WORLD_Y
        tengine.colBytes = 2 * micropolis.WORLD_Y
        tengine.rowBytes = 2
        tengine.typeCode = 'H'
        tengine.tileMask = micropolis.LOMASK


    def destroyEngine(self):

        # TODO: clean up all user pointers and callbacks. 
        # TODO: Make sure there are no memory leaks.
        
        tilewindow.TileDrawingArea.destroyEngine(self)


    def getCell(self, col, row):

        return self.engine.getTile(col, row)


    def drawOverlays(
        self,
        ctx):

        self.drawSprites(ctx)
        self.drawCursor(ctx)


    def drawSprites(self, ctx):
        pass


    def tickEngine(self):

        engine = self.engine
        engine.sim_tick()
        engine.animateTiles()


    def getToolPie(self):

        toolPie = self.toolPie
        if toolPie:
            return toolPie

        toolPie = micropolispiemenus.MakeToolPie()
        self.toolPie = toolPie
        return toolPie


    def handleButtonPress(
        self,
        widget,
        event):

        #print "handleButtonPress TileDrawingArea", self

        print "EVENT", event
        #print dir(event)

        if event.button == 1:

            self.down = True
            self.downX = event.x
            self.downY = event.y

            self.panning = True
            self.downPanX = self.panX
            self.downPanY = self.panY

        elif event.button == 3:

            toolPie = self.getToolPie()

            win_x, win_y, state = event.window.get_pointer()

            print "POP UP TOOLPIE", toolPie, win_x, win_y, state

            #print "WIN", win_x, win_y

            x, y = event.get_root_coords()

            #print "ROOT", x, y

            toolPie.popup(x, y, False)

        self.handleDrag(widget, event)


########################################################################


class TileWindow(gtk.Window):


    def __init__(self, **args):
        gtk.Window.__init__(self, **args)

        self.connect('destroy', gtk.main_quit)

        self.set_title("OLPC Micropolis for Python/Cairo/Pango, by Don Hopkins")

        self.da = MicropolisDrawingArea()
        self.add(self.da)


########################################################################


if __name__ == '__main__':

    win = TileWindow()
    print "WIN", win
    win.show_all()

    gtk.main()


########################################################################
