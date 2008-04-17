# micropolistool.py
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
# Micropolis Tool
# Don Hopkins


########################################################################
# Import Modules


import micropolis
import tiletool


########################################################################


class MicropolisTool(tiletool.TileTool):


    def __init__(
        self,
        toolIndex=-1,
        **args):
        
        tiletool.TileTool.__init__(
            self,
            **args)

        self.toolIndex = toolIndex


    def handleMouseDown(self, view, event):

        x, y = view.getEventXY(event)
        #print "DOWN", x, y, "index", self.toolIndex, view.engine.ToolDown
        view.engine.ToolDown(self.toolIndex, int(x * 16), int(y * 16))


    def handleMouseDrag(self, view, event):

        x, y = view.getEventXY(event)
        #print "DRAG", x, y, "index", self.toolIndex, view.engine.ToolDrag
        view.engine.ToolDrag(self.toolIndex, int(x * 16), int(y * 16))


    def handleMouseUp(self, view, event):

        x, y = view.getEventXY(event)
        #print "UP", x, y
        view.engine.ToolUp(self.toolIndex, int(x * 16), int(y * 16))


    def tick(self, view):

        pass


########################################################################


class MicropolisChalkTool(tiletool.TileTool):


    def getCursorHotSpot(self, view):
        return (
            view.cursorX, 
            view,cursorY,
        )


    def drawCursor(self, view, ctx):

        cursorX = view.cursorX
        cursorY = view.cursorY
        panX = view.panX
        panY = view.panY

        x = cursorX + panX
        y = cursorY + panY

        ctx.save()

        ctx.translate(
            x,
            y)

        ctx.set_line_width(
            4.0)

        ctx.move_to(
            0,
            0)
        ctx.line_to(
            0,
            -5)
        ctx.line_to(
            20,
            -25)
        ctx.line_to(
            25,
            -20)
        ctx.line_to(
            5,
            0)
        ctx.line_to(
            0,
            0)
        ctx.close_path()

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


########################################################################


class MicropolisEraserTool(MicropolisTool):


    def getCursorHotSpot(self, view):
        return (
            view.cursorX, 
            view,cursorY,
        )


    def drawCursor(self, view, ctx):

        cursorX = view.cursorX
        cursorY = view.cursorY
        panX = view.panX
        panY = view.panY

        x = cursorX + panX
        y = cursorY + panY

        ctx.save()

        ctx.translate(
            x,
            y)

        ctx.set_line_width(
            4.0)

        ctx.move_to(
            0,
            -10)
        ctx.line_to(
            10,
            0)
        ctx.line_to(
            0,
            10)
        ctx.line_to(
            -10,
            0)
        ctx.line_to(
            00,
            -10)
        ctx.close_path()

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


########################################################################
# Define all the tools.


tiletool.PanTool(
    name='Pan',
    cursorRows=0,
    cursorCols=0,
    cursorHotCol=0,
    cursorHotRow=0,
    iconNormal='images/icpan.png',
    iconHilite='images/icpanhi.png',
    iconX=0,
    iconY=0)

MicropolisChalkTool(
    name='Chalk',
    cursorRows=0,
    cursorCols=0,
    cursorHotCol=0,
    cursorHotRow=0,
    iconNormal='images/icchlk.png',
    iconHilite='images/icchlkhi.png',
    iconX=28,
    iconY=216)

MicropolisEraserTool(
    name='Eraser',
    cursorRows=0,
    cursorCols=0,
    cursorHotCol=0,
    cursorHotRow=0,
    iconNormal='images/icersr.png',
    iconHilite='images/icersrhi.png',
    iconX=66,
    iconY=216)

MicropolisTool(
    name='Query',
    toolIndex=micropolis.queryState,
    cursorRows=1,
    cursorCols=1,
    cursorHotCol=0,
    cursorHotRow=0,
    iconNormal='images/icqry.png',
    iconHilite='images/icqryhi.png',
    iconX=47,
    iconY=112)

MicropolisTool(
    name='Bulldozer',
    toolIndex=micropolis.dozeState,
    cursorRows=1,
    cursorCols=1,
    cursorHotCol=0,
    cursorHotRow=0,
    iconNormal='images/icdozr.png',
    iconHilite='images/icdozrhi.png',
    iconX=66,
    iconY=150)

MicropolisTool(
    name='Wire',
    toolIndex=micropolis.wireState,
    cursorRows=1,
    cursorCols=1,
    cursorHotCol=0,
    cursorHotRow=0,
    iconNormal='images/icwire.png',
    iconHilite='images/icwirehi.png',
    iconX=28,
    iconY=150)

MicropolisTool(
    name='Road',
    toolIndex=micropolis.roadState,
    cursorRows=1,
    cursorCols=1,
    cursorHotCol=0,
    cursorHotRow=0,
    iconNormal='images/icroad.png',
    iconHilite='images/icroadhi.png',
    iconX=66,
    iconY=188)

MicropolisTool(
    name='Rail',
    toolIndex=micropolis.rrState,
    cursorRows=1,
    cursorCols=1,
    cursorHotCol=0,
    cursorHotRow=0,
    iconNormal='images/icrail.png',
    iconHilite='images/icrailhi.png',
    iconX=6,
    iconY=188)

MicropolisTool(
    name='Park',
    toolIndex=micropolis.parkState,
    cursorRows=1,
    cursorCols=1,
    cursorHotCol=0,
    cursorHotRow=0,
    iconNormal='images/icpark.png',
    iconHilite='images/icparkhi.png',
    iconX=47,
    iconY=254)

MicropolisTool(
    name='Residential',
    toolIndex=micropolis.residentialState,
    cursorRows=3,
    cursorCols=3,
    cursorHotCol=1,
    cursorHotRow=1,
    iconNormal='images/icres.png',
    iconHilite='images/icreshi.png',
    iconX=9,
    iconY=58)

MicropolisTool(
    name='Commercial',
    toolIndex=micropolis.commercialState,
    cursorRows=3,
    cursorCols=3,
    cursorHotCol=1,
    cursorHotRow=1,
    iconNormal='images/iccom.png',
    iconHilite='images/iccomhi.png',
    iconX=47,
    iconY=58)

MicropolisTool(
    name='Industrial',
    toolIndex=micropolis.industrialState,
    cursorRows=3,
    cursorCols=3,
    cursorHotCol=1,
    cursorHotRow=1,
    iconNormal='images/icind.png',
    iconHilite='images/icindhi.png',
    iconX=85,
    iconY=58)

MicropolisTool(
    name='FireStation',
    toolIndex=micropolis.fireState,
    cursorRows=3,
    cursorCols=3,
    cursorHotCol=1,
    cursorHotRow=1,
    iconNormal='images/icfire.png',
    iconHilite='images/icfirehi.png',
    iconX=9,
    iconY=112)

MicropolisTool(
    name='PoliceStation',
    toolIndex=micropolis.policeState,
    cursorRows=3,
    cursorCols=3,
    cursorHotCol=1,
    cursorHotRow=1,
    iconNormal='images/icpol.png',
    iconHilite='images/icpolhi.png',
    iconX=85,
    iconY=112)

MicropolisTool(
    name='Stadium',
    toolIndex=micropolis.stadiumState,
    cursorRows=4,
    cursorCols=4,
    cursorHotCol=1,
    cursorHotRow=1,
    iconNormal='images/icstad.png',
    iconHilite='images/icstadhi.png',
    iconX=1,
    iconY=254)

MicropolisTool(
    name='Seaport',
    toolIndex=micropolis.seaportState,
    cursorRows=4,
    cursorCols=4,
    cursorHotCol=1,
    cursorHotRow=1,
    iconNormal='images/icseap.png',
    iconHilite='images/icseaphi.png',
    iconX=85,
    iconY=254)

MicropolisTool(
    name='CoalPowerPlant',
    toolIndex=micropolis.powerState,
    cursorRows=4,
    cursorCols=4,
    cursorHotCol=1,
    cursorHotRow=1,
    iconNormal='images/iccoal.png',
    iconHilite='images/iccoalhi.png',
    iconX=1,
    iconY=300)

MicropolisTool(
    name='NuclearPowerPlant',
    toolIndex=micropolis.nuclearState,
    cursorRows=4,
    cursorCols=4,
    cursorHotCol=1,
    cursorHotRow=1,
    iconNormal='images/icnuc.png',
    iconHilite='images/icnuchi.png',
    iconX=85,
    iconY=300)

MicropolisTool(
    name='Airport',
    toolIndex=micropolis.airportState,
    cursorRows=6,
    cursorCols=6,
    cursorHotCol=1,
    cursorHotRow=1,
    iconNormal='images/icairp.png',
    iconHilite='images/icairphi.png',
    iconX=35,
    iconY=346)


########################################################################
