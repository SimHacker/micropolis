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


class MicropolisTool:


    allTools = {}


    def __init__(
        self, 
        name=None,
        cursorRows=1,
        cursorCols=1,
        cursorHotCol=0,
        cursorHotRow=0,
        cursorDrawer=None,
        toolIndex=-1,
        **args):

        self.name = name
        self.cursorRows = cursorRows
        self.cursorCols = cursorCols
        self.cursorHotCol = cursorHotCol
        self.cursorHotRow = cursorHotRow
        self.cursorDrawer = cursorDrawer
        self.toolIndex = toolIndex

        if name:
            MicropolisTool.allTools[name] = self


    def select(self, target):
        target.tool = self
        target.cursorHotCol = self.cursorHotCol
        target.cursorHotRow = self.cursorHotRow
        target.cursorDrawer = self.cursorDrawer
        target.toolIndex = self.toolIndex

        target.setCursorSize(
            self.cursorCols, 
            self.cursorRows)


    def deselect(self, target):
        pass


    @classmethod
    def getToolName(self, toolName):
        return self.allTools.get(toolName, None)


########################################################################


def DrawChalkCursor(self, ctx):

    cursorX = self.cursorX
    cursorY = self.cursorY
    panX = self.panX
    panY = self.panY

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

    return True # Do not draw regular tile cursor.


def DrawEraserCursor(self, ctx):

    cursorX = self.cursorX
    cursorY = self.cursorY
    panX = self.panX
    panY = self.panY

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

    return True # Do not draw regular tile cursor.


########################################################################
# Define all the tools.


MicropolisTool(
    name='Chalk',
    cursorRows=0,
    cursorCols=0,
    cursorHotCol=0,
    cursorHotRow=0,
    cursorDrawer=DrawChalkCursor,
    toolIndex=10)

MicropolisTool(
    name='Eraser',
    cursorRows=0,
    cursorCols=0,
    cursorHotCol=0,
    cursorHotRow=0,
    cursorDrawer=DrawEraserCursor,
    toolIndex=11)

MicropolisTool(
    name='Query',
    cursorRows=1,
    cursorCols=1,
    cursorHotCol=0,
    cursorHotRow=0,
    cursorDrawer=None,
    toolIndex=0)

MicropolisTool(
    name='Bulldozer',
    cursorRows=1,
    cursorCols=1,
    cursorHotCol=0,
    cursorHotRow=0,
    cursorDrawer=None,
    toolIndex=7)

MicropolisTool(
    name='Wire',
    cursorRows=1,
    cursorCols=1,
    cursorHotCol=0,
    cursorHotRow=0,
    cursorDrawer=None,
    toolIndex=6)

MicropolisTool(
    name='Road',
    cursorRows=1,
    cursorCols=1,
    cursorHotCol=0,
    cursorHotRow=0,
    cursorDrawer=None,
    toolIndex=9)

MicropolisTool(
    name='Rail',
    cursorRows=1,
    cursorCols=1,
    cursorHotCol=0,
    cursorHotRow=0,
    cursorDrawer=None,
    toolIndex=8)

MicropolisTool(
    name='Park',
    cursorRows=1,
    cursorCols=1,
    cursorHotCol=0,
    cursorHotRow=0,
    cursorDrawer=None,
    toolIndex=13)

MicropolisTool(
    name='Residential',
    cursorRows=3,
    cursorCols=3,
    cursorHotCol=1,
    cursorHotRow=1,
    cursorDrawer=None,
    toolIndex=0)

MicropolisTool(
    name='Commercial',
    cursorRows=3,
    cursorCols=3,
    cursorHotCol=1,
    cursorHotRow=1,
    cursorDrawer=None,
    toolIndex=1)

MicropolisTool(
    name='Industrial',
    cursorRows=3,
    cursorCols=3,
    cursorHotCol=1,
    cursorHotRow=1,
    cursorDrawer=None,
    toolIndex=2)

MicropolisTool(
    name='FireStation',
    cursorRows=3,
    cursorCols=3,
    cursorHotCol=1,
    cursorHotRow=1,
    cursorDrawer=None,
    toolIndex=3)

MicropolisTool(
    name='PoliceStation',
    cursorRows=3,
    cursorCols=3,
    cursorHotCol=1,
    cursorHotRow=1,
    cursorDrawer=None,
    toolIndex=5)

MicropolisTool(
    name='Stadium',
    cursorRows=4,
    cursorCols=4,
    cursorHotCol=1,
    cursorHotRow=1,
    cursorDrawer=None,
    toolIndex=12)

MicropolisTool(
    name='Seaport',
    cursorRows=4,
    cursorCols=4,
    cursorHotCol=1,
    cursorHotRow=1,
    cursorDrawer=None,
    toolIndex=4)

MicropolisTool(
    name='CoalPowerPlant',
    cursorRows=4,
    cursorCols=4,
    cursorHotCol=1,
    cursorHotRow=1,
    cursorDrawer=None,
    toolIndex=15)

MicropolisTool(
    name='NuclearPowerPlant',
    cursorRows=4,
    cursorCols=4,
    cursorHotCol=1,
    cursorHotRow=1,
    cursorDrawer=None,
    toolIndex=16)

MicropolisTool(
    name='Airport',
    cursorRows=5,
    cursorCols=5,
    cursorHotCol=2,
    cursorHotRow=2,
    cursorDrawer=None,
    toolIndex=17)


########################################################################
