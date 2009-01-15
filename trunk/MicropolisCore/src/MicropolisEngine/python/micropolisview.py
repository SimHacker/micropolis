# micropolisview.py
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
# Micropolis View
# Don Hopkins


########################################################################
# Import stuff


import gtk
import cairo
import pango


########################################################################
# MicropolisView


class MicropolisView(gtk.DrawingArea):


    def __init__(
        self,
        engine=None,
        aspect=None,
        strokeColor=(0, 0, 0),
        fillColor=(0.75, 0.75, 0.75),
        interests=(),
        titleFont=pango.FontDescription('Helvetiva 18'),
        labelFont=pango.FontDescription('Helvetiva 12'),
        **args):

        gtk.DrawingArea.__init__(
            self,
            **args)

        self.engine = engine
        self.aspect = aspect
        self.strokeColor = strokeColor
        self.fillColor = fillColor
        self.interests = interests
        self.titleFont = titleFont
        self.labelFont = labelFont

        self.downX = -1
        self.downY = -1
        self.upX = -1
        self.upY = -1
        self.mouseX = -1
        self.mouseY = -1
        self.down = False
        self.clickable = True

        self.engine.expressInterest(
            self,
            self.interests)

        self.set_events(
            gtk.gdk.EXPOSURE_MASK |
            gtk.gdk.POINTER_MOTION_MASK |
            gtk.gdk.POINTER_MOTION_HINT_MASK |
            gtk.gdk.BUTTON_MOTION_MASK |
            gtk.gdk.BUTTON_PRESS_MASK |
            gtk.gdk.BUTTON_RELEASE_MASK)

        self.connect('expose_event', self.handleExpose)
        self.connect('motion_notify_event', self.handleMotionNotify)
        self.connect('button_press_event', self.handleButtonPress)
        self.connect('button_release_event', self.handleButtonRelease)
        self.connect('scroll_event', self.handleMouseScroll)


    def handleExpose(
        self,
        widget,
        event,
        *args):

        if args:
            pass # print "handleExpose MicropolisView", self, "WIDGET", widget, "EVENT", event, "ARGS", args

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

        ctxWindow.set_line_width(2)

        ctxWindow.set_source_rgb(
            *self.fillColor)

        ctxWindow.fill_preserve()

        ctxWindow.set_source_rgb(
            *self.strokeColor)

        ctxWindow.stroke()

        pcontext = self.create_pango_context()
        playout = pango.Layout(pcontext)

        self.drawContent(ctxWindow, playout)


    def update(
        self,
        name,
        *args):

        #print "MicropolisView update", self, name, args

        self.queue_draw()


    def drawContent(
        self,
        ctx,
        playout):

        pass

    def drawShadowText(
        self,
        text,
        x,
        y,
        ctx,
        playout):

        playout.set_text(text)

        ctx.set_source_rgb(1.0, 1.0, 1.0)
        for dx, dy in (
            (-1, -1), ( 1, -1),
            (-1,  1), ( 1,  1),
        ):
            ctx.move_to(x + dx, y +  dy)
            ctx.show_layout(playout)

        ctx.set_source_rgb(0.0, 0.0, 0.0)
        ctx.move_to(x, y)
        ctx.show_layout(playout)

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

        if self.down:
            self.handleMouseDrag(event)
        else:
            self.handleMousePoint(event)


    def handleMouseDrag(
        self,
        event):

        if not self.clickable:
            return

        print "handleMouseDrag", self, event, event.x, event.y


    def handleMousePoint(
        self,
        event):

        if not self.clickable:
            return

        print "handleMousePoint", self, event, event.x, event.y


    def handleButtonPress(
        self,
        widget,
        event):

        print "handleButtonPress", self, event, event.x, event.y

        if not self.clickable:
            return

        self.down = True
        self.downX = event.x
        self.downY = event.y


    def handleButtonRelease(
        self,
        widget,
        event):

        print "handleMouseRelease", self, event, event.x, event.y

        if not self.clickable:
            return

        self.handleMouseDrag(event)

        self.down = False


    def handleMouseScroll(
        self,
        widget,
        event):

        if not self.zoomable:
            return

        direction = event.direction

        print "handleMouseScroll", direction


    def pointInRect(self, x, y, rect):
        xx, yy, ww, hh = rect
        return ((x >= xx) and (x < (xx + ww)) and
                (y >= yy) and (y < (yy + hh)))



########################################################################

