#!/usr/bin/env python

########################################################################
# Pie Menu for GTK/Cairo/Pango/Sugar.
# Copyright (C) 1986-2007 by Don Hopkins. All rights reserved.
#
# Designed and implemented by Don Hopkins (dhopkins@DonHopkins.com).
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA

'''Pie Menus

Pie menu component for Python, GTK, Cairo, Pango and Sugar.
By Don Hopkins.
http://www.DonHopkins.com
http://www.PieMenu.com
'''


########################################################################


import gtk
import cairo
import pango
import math
import time


########################################################################
# Math utilities.


PI = math.pi
TwoPI = 2.0 * PI

Cos = math.cos
Sin = math.sin
Atan2 = math.atan2
Sqrt = math.sqrt


def Floor(val):
    return int(math.floor(val))

def Ceil(val):
    return int(math.ceil(val))

def Round(val):
    return Floor(val + 0.5)


def DegToRad(deg):
    return (TwoPI * deg) / 360.0


def RadToDeg(rad):
    return (360.0 * rad) / TwoPI


def NormalizeAngleRad(ang):

    while ang < 0:
        ang += TwoPI

    while ang >= TwoPI:
        ang -= TwoPI

    return ang


########################################################################
# Font cache.


FontCache = {}

def GetFont(s):

    if FontCache.has_key(s):
        return FontCache[s]

    descr = pango.FontDescription(s)

    FontCache[s] = descr

    return descr


########################################################################
# Image cache.


ImageCache = {}

def GetImage(s):
   
    if ImageCache.has_key(s):
        return ImageCache[s]
   
    image = None
   
    if s[-4:].lower() == '.png':
        try:
            image = cairo.ImageSurface.create_from_png(s)
        except: pass
    elif s[-4:].lower() == '.svg':
        try:
            # FIXME: read SVG into image
            print "SVG images not supported yet:", s
        except: pass
    else:
        print "Don't know how to load image file type:", s
   
    if image:
        ImageCache[s] = image
   
    return image


########################################################################


class PieItem:


    def __init__(
            self,
            label=None,
            description=None,
            action=None,
            x=0,
            y=0,
            width=0,
            height=0,
            labelFont='Sans 14',
            labelPadding=2,
            labelXAdjust=0,
            labelYAdjust=2,
            icon=None,
            iconHilite=None,
            iconPadding=2,
            iconSide='top',
            visible=True,
            pie=None,
            subPie=None,
            fixedRadius=0,
            linear=False,
            linearDirection='down',
            linearOrderReversed=False,
            linearGap=0,
            hiliteFillColor=(1, 1, 0),
            hiliteStrokeColor=(0, 0, 1),
            hiliteTextColor=(0, 0, 1),
            loliteFillColor=(1, 1, 1),
            loliteStrokeColor=(0, 0, 0),
            loliteTextColor=(0, 0, 0),
            **args):

        #print "PIEITEM INIT", self, label, args
        self.label = label
        self.description = description
        self.action = action
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.labelFont = labelFont
        self.labelPadding = labelPadding
        self.labelXAdjust = labelXAdjust
        self.labelYAdjust = labelYAdjust
        self.icon = icon
        self.iconHilite = iconHilite
        self.iconPadding = iconPadding
        self.iconSide = iconSide
        self.visible = visible
        self.pie = None # Added later.
        self.subPie = subPie
        self.fixedRadius = fixedRadius
        self.linear = linear
        self.linearDirection = linearDirection
        self.linearOrderReversed = linearOrderReversed
        self.linearGap = linearGap
        self.hiliteFillColor = hiliteFillColor
        self.hiliteStrokeColor = hiliteStrokeColor
        self.hiliteTextColor = hiliteTextColor
        self.loliteFillColor = loliteFillColor
        self.loliteStrokeColor = loliteStrokeColor
        self.loliteTextColor = loliteTextColor

        self.valid = False
        self.index = -1
        self.dx = 0
        self.dy = 0
        self.enterTime = 0
        self.exitTime = 0
        self.totalTime = 0
        self.finalX = 0
        self.finalY = 0
        self.ringIndex = None
        self.angle = 0
        self.edge1Dx = 0
        self.edge1Dy = 0
        self.edge1Angle = 0
        self.edge2Dx = 0
        self.edge2Dy = 0
        self.edge2Angle = 0
        self.labelX = 0
        self.labelY = 0
        self.labelWidth = 0
        self.labelHeight = 0
        self.iconX = 0
        self.iconY = 0
        self.iconWidth = 0
        self.iconHeight = 0

        if pie != None:
            pie.addItem(self)


    def measure(self, context, pcontext, playout):

        label = self.label

        width = 0
        height = 0

        labelX = 0
        labelY = 0
        labelWidth = 0
        labelHeight = 0

        iconX = 0
        iconY = 0
        iconWidth = 0
        iconHeight = 0

        labelPadding = self.labelPadding
        iconPadding = self.iconPadding

        if label != None:

            font = GetFont(self.labelFont)

            playout.set_font_description(font)
            playout.set_markup(label)
            labelWidth, labelHeight = playout.get_pixel_size()

            labelWidth += 2 * labelPadding
            labelHeight += 2 * labelPadding

        icon = self.icon

        if icon != None:

            image = GetImage(icon)
            if image:

                iconWidth = image.get_width()
                iconHeight = image.get_height()

                iconWidth += 2 * iconPadding
                iconHeight += 2 * iconPadding

        if not label:
            if not icon:
                # No icon, no label.
                pass
            else:
                # Just an icon.
                width = iconWidth
                height = iconHeight
                iconX = iconPadding
                iconY = iconPadding
        else:
            if not icon:
                # Just a label.
                width = labelWidth
                height = labelHeight
                labelX = labelPadding
                labelY = labelPadding
            else:
                # Icon and label.
                iconSide = self.iconSide
                if iconSide in ('n', 'up', 'top'):
                    width = max(labelWidth, iconWidth)
                    height = labelHeight + iconHeight
                    iconX = Floor(((width - iconWidth) / 2) + iconPadding)
                    iconY = iconPadding
                    labelX = Floor(((width - labelWidth) / 2) + labelPadding)
                    labelY = iconHeight + labelPadding
                elif iconSide in ('nw', 'topleft'):
                    width = max(labelWidth, iconWidth)
                    height = labelHeight + iconHeight
                    iconX = iconPadding
                    iconY = iconPadding
                    labelX = labelPadding
                    labelY = iconHeight + labelPadding
                elif iconSide in ('ne', 'topright'):
                    width = max(labelWidth, iconWidth)
                    height = labelHeight + iconHeight
                    iconX = width - iconWidth + iconPadding
                    iconY = iconPadding
                    labelX = width - labelWidth + labelPadding
                    labelY = iconHeight + labelPadding
                elif iconSide in ('s', 'down', 'bottom'):
                    width = max(labelWidth, iconWidth)
                    height = labelHeight + iconHeight
                    labelX = Floor(((width - labelWidth) / 2) + labelPadding)
                    labelY = labelPadding
                    iconX = Floor(((width - iconWidth) / 2) + iconPadding)
                    iconY = labelHeight + iconPadding
                elif iconSide in ('sw', 'bottomleft'):
                    width = max(labelWidth, iconWidth)
                    height = labelHeight + iconHeight
                    labelX = labelPadding
                    labelY = labelPadding
                    iconX = iconPadding
                    iconY = labelHeight + iconPadding
                elif iconSide in ('se', 'bottomright'):
                    width = max(labelWidth, iconWidth)
                    height = labelHeight + iconHeight
                    labelX = width - labelWidth + labelPadding
                    labelY = labelPadding
                    iconX = width - iconWidth + iconPadding
                    iconY = labelHeight + iconPadding
                elif iconSide in ('w', 'left'):
                    width = labelWidth + iconWidth
                    height = max(labelHeight, iconHeight)
                    iconX = iconPadding
                    iconY = Floor(((height - iconHeight) / 2) + iconPadding)
                    labelX = iconWidth + labelPadding
                    labelY = Floor(((height - labelHeight) / 2) + labelPadding)
                elif iconSide in ('e', 'right'):
                    width = labelWidth + iconWidth
                    height = max(labelHeight, iconHeight)
                    labelX = labelPadding
                    labelY = Floor(((height - labelHeight) / 2) + labelPadding)
                    iconX = labelWidth + iconPadding
                    iconY = Floor(((height - iconHeight) / 2) + iconPadding)
                else:
                    print "Invalid iconSide: " + repr(iconSide)

        self.width = width
        self.height = height

        self.labelX = labelX
        self.labelY = labelY
        self.labelWidth = labelWidth
        self.labelHeight = labelHeight

        self.iconX = iconX
        self.iconY = iconY
        self.iconWidth = iconWidth
        self.iconHeight = iconHeight


    def layoutPie(self, radius):

        if self.fixedRadius > 0:
            radius = self.fixedRadius;

        gap = 1
        rdx = radius * self.dx
        rdy = radius * self.dy

        # Special cases for top, bottom, left, and right
        if Round(rdx) == 0: # top or bottom
            rdx -= (self.width / 2.0) + 1
            if rdy > 0:
                rdy += gap - 3; # top
            else:
                rdy -= self.height + gap - 2; # bottom
        else:
            if Round(rdy) == 0: # left or right
                rdy -= (self.height / 2.0) + 0.5
                if rdx < 0:
                    rdx -= self.width + gap - 2
                else:
                    rdx += gap - 3
            else: # everwhere else
                # Justify the label according to its quadrant.
                fudge = 2

                if rdx < 0:
                    rdx -= self.width - fudge
                else:
                    rdx -= fudge

                if rdy < 0:
                    rdy -= self.height - fudge
                else:
                    rdy -= fudge

        self.x = Round(rdx)
        self.y = Round(rdy)


    def layoutLinear(self, minX, minY, maxX, maxY):
        pie = self.pie

        linearDirection = self.linearDirection
        if ((linearDirection == None) or
            (linearDirection == "")):
            linearDirection = self.pie.linearDirection

        pie = self.pie
        gap = self.linearGap
        xCenterOffset = 0
        yCenterOffset = 0

        if linearDirection in ('c', 'center'):
            otherItem = pie.addItemDirection(self, 'center')
            xCenterOffset = Floor((self.width / -2.0) - 1) # TODO: Explain what this -1 fudge is for!
            yCenterOffset = Floor(self.height / -2.0)
        elif linearDirection in ('e', 'right'):
            otherItem = pie.addItemDirection(self, 0)
            if otherItem:
                xCenterOffset = Floor(otherItem.x + otherItem.width + gap)
                yCenterOffset = Floor(self.height / -2.0)
            else:
                self.layoutPie(pie.radius)
                return
        elif linearDirection in ('ne', 'topright'):
            otherItem = pie.addItemDirection(self, 45)
            if otherItem:
                xCenterOffset = Floor(otherItem.x + otherItem.width + gap)
                yCenterOffset = Floor(otherItem.y + otherItem.height - self.height)
            else:
                self.layoutPie(pie.radius)
                return
        elif linearDirection in ('se', 'bottomright'):
            otherItem = pie.addItemDirection(self, 315)
            if otherItem:
                xCenterOffset = Floor(otherItem.x + otherItem.width + gap)
                yCenterOffset = Floor(otherItem.y)
            else:
                self.layoutPie(pie.radius)
                return
        elif linearDirection in ('w', 'left'):
            otherItem = pie.addItemDirection(self, 180)
            if otherItem:
                xCenterOffset = Floor(otherItem.x - self.width - gap)
                yCenterOffset = Floor(self.height / -2.0)
            else:
                self.layoutPie(pie.radius)
                return
        elif linearDirection in ('nw', 'topleft'):
            otherItem = pie.addItemDirection(self, 135)
            if otherItem:
                xCenterOffset = Floor(otherItem.x - otherItem.width - gap)
                yCenterOffset = Floor((otherItem.y + otherItem.height - self.height))
            else:
                self.layoutPie(pie.radius)
                return
        elif linearDirection in ('sw', 'bottomleft'):
            otherItem = pie.addItemDirection(self, 225)
            if otherItem:
                xCenterOffset = Floor(otherItem.x - self.width - gap)
                yCenterOffset = Floor(otherItem.y)
            else:
                self.layoutPie(pie.radius)
                return
        elif linearDirection in ('n', 'up', 'top'):
            otherItem = pie.addItemDirection(self, 90)
            if otherItem:
                xCenterOffset = Floor((self.width / -2.0) - 1)
                yCenterOffset = Floor(otherItem.y - self.height - gap)
            else:
                self.layoutPie(pie.radius)
                return
        elif linearDirection in ('s', 'down', 'bottom'):
            otherItem = pie.addItemDirection(self, 270)
            if otherItem:
                xCenterOffset = Floor((self.width / -2.0) - 1)
                yCenterOffset = Floor(otherItem.y + otherItem.height + gap)
            else:
                self.layoutPie(pie.radius)
                return

        self.x = Round(xCenterOffset)
        self.y = Round(yCenterOffset)

        self.ringIndex = None


    def itemsOverlap(self, item, fringe=2):

        myLeft = self.x - fringe
        myRight = self.x + self.width + fringe
        yourLeft = item.x - fringe
        yourRight = item.x + item.width + fringe
        myTop = self.y - fringe
        myBottom = self.y + self.height + fringe
        yourTop = item.y - fringe
        yourBottom = item.y + item.height + fringe

        return ((myBottom > yourTop) and
                (myTop < yourBottom) and
                (myRight > yourLeft) and
                (myLeft < yourRight))


    def draw(self, rect, context, pcontext, playout):

        x = self.x
        y = self.y
        width = self.width
        height = self.height

        hilited = self.index == self.pie.curItem

        if hilited:
            fillColor = self.hiliteFillColor
            strokeColor = self.hiliteStrokeColor
            textColor = self.hiliteTextColor
        else:
            fillColor = self.loliteFillColor
            strokeColor = self.loliteStrokeColor
            textColor = self.loliteTextColor

        if fillColor or strokeColor:

            context.rectangle(x, y, width, height)

            if fillColor:
                context.set_source_rgb(*fillColor)
                if strokeColor:
                    context.fill_preserve()
                else:
                    context.fill()

            if strokeColor:
                context.set_source_rgb(*strokeColor)
                context.stroke()

        label = self.label
        if textColor and (label != None):

            context.set_source_rgb(*textColor)

            font = GetFont(self.labelFont)

            playout.set_font_description(font)
            playout.set_markup(label)

            context.move_to(
                x + self.labelX,
                y + self.labelY)

            context.show_layout(playout)

        hilited = self.index == self.pie.curItem
        if hilited:
            icon = self.iconHilite or self.icon
        else:
            icon = self.icon

        if icon:

            image = GetImage(icon)
            if image:

                context.set_source_surface(
                    image,
                    x + self.iconX,
                    y + self.iconY)

                context.paint()


    def handleHilite(self):

        # TODO: notify menu item about hilite
        #print "handleHilite", self, repr(self.label), repr(self.description)
        pass


    def handleLolite(self):

        # TODO: notify menu item about lolite
        #print "handleLolite", self, repr(self.label), repr(self.description)
        pass


    def handleMotion(self):

        # TODO: notify menu item about mouse motion
        #print "handleMotion", self, repr(self.label), repr(self.description)
        pass


    def handleAction(self):

        # TODO: notify menu item about mouse motion
        #print "handleAction", self, repr(self.label), repr(self.description)

        if self.action:
            self.action(self)


########################################################################


class PieMenu(gtk.Window):


    def __init__(
            self,
            parent=None,
            action=None,
            outsideFillColor=(.9, .9, .9),
            outsideStrokeColor=(0, 0, 0),
            backgroundFillColor=(1, 1, 1),
            backgroundStrokeColor=(.5, .5, .5),
            edgeStrokeColor=(.5, .5, .5),
            sliceHiliteFillColor=(0, 1, 0),
            sliceHiliteStrokeColor=(0, 0, 1),
            neutralRadius=12,
            neutralHiliteFillColor=(1, 0, 0),
            neutralHiliteStrokeColor=(0, 0, 1),
            neutralLoliteFillColor=(1, 1, 1),
            neutralLoliteStrokeColor=(.5, .5, .5),
            neutralDescription=None,
            popoutRadius=1000,
            ringRadius = 40,
            header=None,
            headerFillColor=(0, 0, 0),
            headerStrokeColor=None,
            headerTextColor=(1, 1, 1),
            headerFont='Sans 24',
            headerPadding=2,
            headerMargin=4,
            headerGap=4,
            headerXAdjust=0,
            headerYAdjust=3,
            footer=None,
            footerFillColor=(1, 1, 0),
            footerStrokeColor=(0, 0, 1),
            footerTextColor=(0, 0, 1),
            footerFont='Sans 12',
            footerPadding=2,
            footerMargin=4,
            footerGap=4,
            footerXAdjust=0,
            footerYAdjust=3,
            footerFixedHeight=0,
            footerDescriptions=True,
            clockwise=True,
            initialAngle=90,
            fixedRadius=0,
            minRadius=0,
            maxRadius=0,
            extraRadius=0,
            labelGapRadius=-8,
            marginRadius=5,
            radiusNotch=2,
            showBackground=True,
            backgroundImage=None,
            border=5,
            overlapFringe=2,
            maxPieItems=(8,),
            pieSlice=0,
            linear=False,
            linearDirection='down',
            linearOrderReversed=False,
            transparentItems=True,
            itemBorder=2,
            itemMargin=2,
            itemWidth=0,
            itemHeight=0,
            centerMargin=16,
            centerBorderWidth=2,
            centerVisible=1,
            parentPie=None,
            parentItem=None,
            pinX=0,
            pinY=0,
            shapeWindow=True,
            **args):

        # Create the toplevel window
        gtk.Window.__init__(
            self,
            type = gtk.WINDOW_POPUP,
            **args)
        try:
            self.set_screen(parent.get_screen())
        except AttributeError:
            self.connect('destroy', lambda *w: gtk.main_quit())

        self.action = action
        self.outsideFillColor = outsideFillColor
        self.outsideStrokeColor = outsideStrokeColor
        self.backgroundFillColor = backgroundFillColor
        self.backgroundStrokeColor = backgroundStrokeColor
        self.edgeStrokeColor = edgeStrokeColor
        self.sliceHiliteFillColor = sliceHiliteFillColor
        self.sliceHiliteStrokeColor = sliceHiliteStrokeColor
        self.neutralRadius = neutralRadius
        self.neutralHiliteFillColor = neutralHiliteFillColor
        self.neutralHiliteStrokeColor = neutralHiliteStrokeColor
        self.neutralLoliteFillColor = neutralLoliteFillColor
        self.neutralLoliteStrokeColor = neutralLoliteStrokeColor
        self.neutralDescription = neutralDescription
        self.popoutRadius = popoutRadius
        self.ringRadius = ringRadius
        self.header = header
        self.headerFillColor = headerFillColor
        self.headerStrokeColor = headerStrokeColor
        self.headerTextColor = headerTextColor
        self.headerFont = headerFont
        self.headerPadding = headerPadding
        self.headerMargin = headerMargin
        self.headerGap = headerGap
        self.headerXAdjust = headerXAdjust
        self.headerYAdjust = headerYAdjust
        self.footer = footer
        self.footerFillColor = footerFillColor
        self.footerStrokeColor = footerStrokeColor
        self.footerTextColor = footerTextColor
        self.footerFont = footerFont
        self.footerPadding = footerPadding
        self.footerMargin = footerMargin
        self.footerGap = footerGap
        self.footerXAdjust = footerXAdjust
        self.footerYAdjust = footerYAdjust
        self.footerFixedHeight = footerFixedHeight
        self.footerDescriptions = footerDescriptions
        self.clockwise = clockwise
        self.initialAngle = initialAngle
        self.fixedRadius = fixedRadius
        self.minRadius = minRadius
        self.maxRadius = maxRadius
        self.extraRadius = extraRadius
        self.labelGapRadius = labelGapRadius
        self.marginRadius = marginRadius
        self.radiusNotch = radiusNotch
        self.showBackground = showBackground
        self.backgroundImage = backgroundImage
        self.border = border
        self.overlapFringe = overlapFringe
        self.maxPieItems = maxPieItems
        self.pieSlice = pieSlice
        self.linear = linear
        self.linearDirection = linearDirection
        self.linearOrderReversed = linearOrderReversed
        self.transparentItems = transparentItems
        self.itemBorder = itemBorder
        self.itemMargin = itemMargin
        self.itemWidth = itemWidth
        self.itemHeight = itemHeight
        self.centerMargin = centerMargin
        self.centerBorderWidth = centerBorderWidth
        self.centerVisible = centerVisible
        self.parentPie = parentPie
        self.parentItem = parentItem
        self.shapeWindow = shapeWindow

        self.radius = 0
        self.centerX = 0
        self.centerY = 0
        self.innerRadius = 0
        self.headerX = 0
        self.headerY = 0
        self.headerWidth = 0
        self.headerHeight = 0
        self.footerX = 0
        self.footerY = 0
        self.footerWidth = 0
        self.footerHeight = 0
        self.curX = -1
        self.curY = -1
        self.lastX = -1
        self.lastY = -1
        self.mouseDeltaX = 0
        self.mouseDeltaY = 0
        self.direction = 0
        self.distance = 0
        self.items = []
        self.visibleItems = []
        self.pieRings = []
        self.pieItems = []
        self.linearItems = []
        self.itemDirections = {}
        self.item = None
        self.curRingIndex = 0
        self.curItem = -1
        self.lastItem = -1
        self.valid = False
        self.minX = 0
        self.maxX = 0
        self.minY = 0
        self.maxY = 0
        self.trackingFlag = False
        self.centerX = 0
        self.centerY = 0
        self.pinned = False
        self.winX = 0
        self.winY = 0
        self.winWidth = 1
        self.winHeight = 1
        self.x = 0
        self.y = 0
        self.width = 1
        self.height = 1

        d = PieMenuDrawingArea()
        self.d = d
        self.add(self.d)

        self.connect("show", self.handleShow)

        d.connect("expose_event", self.handleExpose)
        d.connect("size_allocate", self.handleSizeAllocate)
        d.connect("motion_notify_event", self.handleMotionNotifyEvent)
        d.connect("button_press_event", self.handleButtonPressEvent)
        d.connect("button_release_event", self.handleButtonReleaseEvent)
        d.connect("proximity_in_event", self.handleProximityInEvent)
        d.connect("proximity_out_event", self.handleProximityOutEvent)
        d.connect("grab_notify", self.handleGrabNotify)
        d.connect("grab_broken_event", self.handleGrabBrokenEvent)
        d.connect("key_press_event", self.handleKeyPressEvent)
        d.connect("key_release_event", self.handleKeyReleaseEvent)

        d.set_events(
            gtk.gdk.EXPOSURE_MASK |
            gtk.gdk.POINTER_MOTION_MASK |
            gtk.gdk.POINTER_MOTION_HINT_MASK |
            gtk.gdk.BUTTON_MOTION_MASK |
            gtk.gdk.BUTTON_PRESS_MASK |
            gtk.gdk.BUTTON_RELEASE_MASK |
            gtk.gdk.KEY_PRESS_MASK |
            gtk.gdk.KEY_RELEASE_MASK |
            gtk.gdk.PROXIMITY_IN_MASK |
            gtk.gdk.PROXIMITY_OUT_MASK)


    def addItem(self, item):

        item.pie = self
        self.items.append(item)
        self.invalidate()


    def invalidate(self):

        self.valid = False


    def validate(self, rect, context, pcontext, playout):

        print "VALIDATE", self.valid

        if self.valid:
            return

        self.valid = True

        self.layout(rect, context, pcontext, playout)


    def addItemDirection(self, item, deg):

        # Add an item to the map of directions to items in that direction.
        # Also set the item's dx and dy. Not sure why that's here.

        if deg == "center":
            dx = 0
            dy = 0
        else:
            angle = DegToRad(deg)
            deg = Round(RadToDeg(NormalizeAngleRad(angle)))
            dx = Cos(angle)
            dy = -Sin(angle)

        item.dx = dx
        item.dy = dy

        lastItem = None

        a = self.itemDirections.get(deg, None)
        if not a:
            a = []
            self.itemDirections[deg] = a
        else:
            lastItem = a[-1]

        a.append(item)

        return lastItem


    def layoutSelf(self):
        context = self.window.cairo_create()
        pcontext = self.create_pango_context()
        playout = pango.Layout(pcontext)

        rect = self.get_allocation()

        self.layout(rect, context, pcontext, playout)


    def layout(self, rect, context, pcontext, playout):

        print "PieMenu layout", self, rect, context, pcontext, playout

        # Just the visible items.
        visibleItems = []
        self.visibleItems = visibleItems

        # The pie rings.
        pieRings = []
        self.pieRings = pieRings

        # Just the visible pie menus items.
        pieItems = []
        self.pieItems = pieItems

        # Just the visible linear menu items.
        linearItems = []
        self.linearItems = linearItems

        # Map of item direction to array of items in that direction.
        # Note: Think about how this interacts with rings and linear menu items.
        itemDirections = {}
        self.itemDirections = itemDirections

        # Put just the visible items from items into the visibleItems array.
        visibleItems = [
            item
            for item in self.items
            if item.visible
        ]
        self.visibleItems = visibleItems

        # Count the visible items.
        itemCount = len(visibleItems)

        # We're done if no items.
        if itemCount == 0:
            return

        # Initialize constants and variables for layout.

        overlapFringe = self.overlapFringe
        pieSlice = self.pieSlice

        maxPieItems = self.maxPieItems
        if maxPieItems == None:
            maxPieItems = [1.0e+6]
        elif type(maxPieItems) not in (type(()), type([])):
            maxPieItems = [maxPieItems]

        maxPieItemsTotal = 0
        for i in maxPieItems:
            maxPieItemsTotal += i

        # Categorize items into pieItems and linearItems arrays.
        itemIndex = 0
        pieItemCount = 0
        maxRingItems = maxPieItems[0]
        for item in visibleItems:

          item.index = itemIndex
          itemIndex += 1
          item.valid = False

          # Limit the number of pie items to maxPieItemsTotal.
          # Classify overflow items as linear.
          if pieItemCount >= maxPieItemsTotal:
              item.linear = True

          if item.linear:
              # Handle reverse ordering linear items.
              if item.linearOrderReversed:
                  # Prepend the linear item to the beginning of the linearItems array.
                  linearItems.insert(0, item)
              else:
                  # Append the linear item to the end of the linearItems array.
                  linearItems.append(item)
          else:
              # If this is the first item, then make the first ring.
              if len(pieRings) == 0:
                  pieRings.append(pieItems)

              # If this item will overflow the current ring, then make a new ring.
              if len(pieItems) >= maxRingItems:
                  pieItems = []
                  pieRings.append(pieItems)
                  ringIndex = len(pieRings) - 1
                  if ringIndex < len(maxPieItems):
                      maxRingItems = maxPieItems[ringIndex]
                  else:
                      print "Layout error: too many pie items, maxPieItems array did not add up right."

              # Append the pie item to the end of the pieItems array.
              pieItems.append(item)
              pieItemCount += 1

        # Measure the items.
        for item in self.items:
            item.x = 0
            item.y = 0
            item.measure(context, pcontext, playout)

        ringCount = len(pieRings)
        linearItemCount = len(linearItems)

        # If there are any pie items, then calculate the pie menu parameters.
        if ringCount > 0:

            pieSubtend = TwoPI
            if pieSlice:
                pieSubtend *= pieSlice

            ringIndex = 0
            for ringItems in pieRings:

                ringItemCount = len(ringItems)

                # Calculate the subtend, angle, cosine, sine, quadrant, slope,
                # and size of each pie menu item.

                # Calculate twist, the angular width of each slice.
                twist = pieSubtend / ringItemCount

                # Twist the other way if clockwise.
                ringClockwise = self.getRingClockwise(ringIndex)
                if ringClockwise:
                    twist = -twist

                # Point ang towards the center of the first slice,
                # or the edge of the first slice, for non-zero pieSlice.

                ringInitialAngle = self.getRingInitialAngle(ringIndex)
                ang = DegToRad(ringInitialAngle)

                if pieSlice == 0:
                    # Twist backwards half a slice, to the edge of the slice.
                    ang -= twist / 2.0

                # Now calculate the size and other properties of the pie items.
                for item in ringItems:

                    # Calculate angle, the center of the current slice.
                    angle = ang + (twist / 2.0)

                    # Add self item to the piemenu's list of items in the same direction.
                    self.addItemDirection(item, RadToDeg(angle))

                    # Calculate the unit vectors of the slice edge directions.
                    # Calculate ang in the upside-down coordinate system, for drawing.
                    item.ringIndex = ringIndex
                    item.angle = ang
                    item.edge1Dx = Cos(ang)
                    item.edge1Dy = -Sin(ang)
                    item.edge1Angle = Atan2(item.edge1Dy, item.edge1Dx)
                    item.edge2Dx = Cos(ang + twist)
                    item.edge2Dy = -Sin(ang + twist)
                    item.edge2Angle = Atan2(item.edge2Dy, item.edge2Dx)

                    # Twist ang around to the edge of the next slice.
                    ang += twist

                ringIndex += 1

        # Determine the radius for the inner ring.
        fixedRadius = self.fixedRadius

        labelGapRadius = self.labelGapRadius

        # If the radius is not fixed, then calculate it dynamically.
        if fixedRadius > 0:

            # Use the fixed radius that has been specified.
            radius = fixedRadius
           
        else:

            # Start with the minRadius.
            radius = self.minRadius

            # If there are any pie items, then make sure they don't overlap.
            if ringCount > 0:

                # Only apply this to the inner ring of the pie menu, for now.
                # The outer rings will each be a fixed ringRadius thick.
                # I think it's better to have explicit control over the
                # inner fixedRadius and outer ringRadius, so designers can
                # tune it to be easy to use, instead of leaving it up to the
                # label layout algorithm to determine (which could badly effect usability).
                # Otherwise pie menus would be too big and the ring sizes would be irregular.
                # Could implement an array of ringRadius numbers to explicitly control
                # the radius of each ring, but that seems control-freakish, so not yet.

                ringItems = pieRings[0]

                # Increase the radius until there are no overlaps between
                # any pie items.
                # Start by wrapping last around to the end of the
                # circular menu.

                # Last index and last pie item, used to test for overlap.
                # Only test for last item overlap if more than one pie item.
                lastIndex = len(ringItems) - 1
                last = None
                if lastIndex > 0:
                    last = ringItems[lastIndex]

                # Loop over all pie items testing for overlap with last adjacent
                # pie item.
                for item in ringItems:

                    # Ignore fixedRadius items.
                    # XXX: The behavior of mixing adjacent fixedRadius and not
                    # fixedRadius pie menu items is not well defined.
                    # XXX: fixedRadius should be inherited from the piemenu.
                    if item.fixedRadius > 0:
                        continue

                    # Push the radius out until there are no overlaps.
                    # Give up after a while.
                    maxLoops = 200
                    radiusNotch = self.radiusNotch

                    for loopCount in range(maxLoops):

                        # Lay out the item at the current radius.
                        item.layoutPie(radius + labelGapRadius)

                        # If there is only one item, then we're done pushing out.
                        if last == None:
                            # Done pushing out.
                            break

                        # If there are more than one pie items, then test for adjacent overlaps.
                        # Lay out the last item at the current radius.
                        last.layoutPie(radius + labelGapRadius)

                        # Test for overlap. Takes two to tango.
                        if not item.itemsOverlap(last, overlapFringe):
                            # They don't overlap, so we're done pushing out.
                            break

                        # The two adjacent labels overlap, so we keep looping and
                        # pushing them out until they don't.

                        # Bump the radiusNotch.
                        radius += radiusNotch

                    last = item

            # Add in the extra radius.
            radius += self.extraRadius

        # Done calculating the radius.
        self.radius = radius

        ringRadius = self.ringRadius
        outerRingRadius = radius + ((len(pieRings) - 1) * ringRadius)

        # Calculate the bounding box of the items, as we lay them out.
        maxX = -1000000
        minX = 1000000
        maxY = -1000000
        minY = 1000000

        if ringCount > 0:
            # If there are any pie items, then make sure the bounding box
            # encompasses the radius.
            minX = -outerRingRadius
            minY = -outerRingRadius
            maxX = outerRingRadius
            maxY = outerRingRadius

        # Calculate the maximum radius (squared).
        maxRadius = 0

        # Loop over the pie items, and calculate their bounding box
        # and maxRadius.

        ringIndex = 0
        for ringItems in pieRings:
           
            for item in ringItems:

                # Lay out the pie item at the current radius.
                item.layoutPie(
                    radius + (ringIndex * ringRadius) + labelGapRadius)

                # Calculate the corners of the item bounding box.
                itw = item.width
                ith = item.height
                itx0 = item.x
                ity0 = item.y
                itx1 = itx0 + itw
                ity1 = ity0 + ith

                # Update the bounding box.
                if itx0 < minX:
                    minX = itx0
                if ity0 < minY:
                    minY = ity0
                if itx1 > maxX:
                    maxX = itx1
                if ity1 > maxY:
                    maxY = ity1

                # Update the maxRadius.
                farx = max(abs(itx0), abs(itx1))
                fary = max(abs(ity0), abs(ity1))
                rad = (farx * farx) + (fary * fary)

                if rad > self.maxRadius:
                    maxRadius = rad;

            ringIndex += 1

        # Loop over the linear items, lay them out,
        # and calculate their bounding box and maxRadius.

        # Calculate the max width of the north and south linear items.
        maxItemWidthNorth = 0
        maxItemWidthSouth = 0

        for item in linearItems:
          # Lay out the linear item.
          item.layoutLinear(
              minX,
              minY,
              maxX,
              maxY)

          # Calculate the max vertical item width.
          if abs(item.dx) < 0.01:
              if item.dy < 0:
                  if item.width > maxItemWidthNorth:
                      maxItemWidthNorth = item.width
                      #print "maxItemWidthNorth", maxItemWidthNorth
              else:
                  if item.width > maxItemWidthSouth:
                      maxItemWidthSouth = item.width
                      #print "maxItemWidthSouth", maxItemWidthSouth

          # Calculate the corners of the item bounding box.
          itw = item.width
          ith = item.height
          itx0 = item.x
          ity0 = item.y
          itx1 = itx0 + itw
          ity1 = ity0 + ith

          # Update the bounding box.
          if itx0 < minX:
              minX = itx0
          if ity0 < minY:
              minY = ity0
          if itx1 > maxX:
              maxX = itx1
          if ity1 > maxY:
              maxY = ity1

          # Update the maxRadius.
          farx = max(abs(itx0), abs(itx1))
          fary = max(abs(ity0), abs(ity1))
          rad = (farx * farx) + (fary * fary)
          if rad > maxRadius:
              maxRadius = rad;

        # Go over the linear items and fix the x and width of all vertical items.
        for item in linearItems:
          #print "item.dx", item.dx, item
          if abs(item.dx) < 0.01:
              w = 0
              if item.dy < 0:
                  w = maxItemWidthNorth
              else:
                  w = maxItemWidthSouth

              item.width = w
              item.x = Round((-0.5 * w) - 1)

        # Calculate the maxRadius.
        maxRadius = Floor(
            0.95 +
            Sqrt(maxRadius) +
            self.marginRadius)
        self.maxRadius = maxRadius

        # Expand the bounding box by the border.
        border = self.border
        minX -= border
        minY -= border
        maxX += border
        maxY += border

        # Expand the bounding box to integers.
        minX = Floor(minX)
        minY = Floor(minY)
        maxX = Ceil(maxX)
        maxY = Ceil(maxY)

        # Measure and position the header.
        self.measureHeader(context, pcontext, playout)
        (minX, minY, maxX, maxY) = self.positionHeader(minX, minY, maxX, maxY)

        # Measure and position the footer.
        self.measureFooter(context, pcontext, playout)
        (minX, minY, maxX, maxY) = self.positionFooter(minX, minY, maxX, maxY)

        # Done calculating the bounding box.
        self.minX = minX
        self.minY = minY
        self.maxX = maxX
        self.maxY = maxY

        # Set the pie menu center.
        centerX = Round(-minX)
        centerY = Round(-minY)
        self.centerX = centerX
        self.centerY = centerY

        # Set the window position and size.

        width = maxX - minX
        height = maxY - minY

        if self.pieRings:
            # If it's a pie menu, then center in the middle of the menu.
            x = self.pinX + minX
            y = self.pinY + minY
        else:
            # If it's a linear menu, then center on its header, or just below the mouse if no header.
            x = self.pinX - (width / 2)
            y = self.pinY - (self.headerHeight / 2)

        self.x = x
        self.y = y
        self.width = width
        self.height =  height

        # Offset the header.
        self.headerX -= minX
        self.headerY -= minY

        # Offset the footer.
        self.footerX -= minX
        self.footerY -= minY

        # Offset the items.
        for item in visibleItems:

            x = item.x
            y = item.y
            x -= minX
            y -= minY

            item.x = x
            item.y = y
            item.finalX = x
            item.finalY = y

        # Done laying out the pie menu. (Whew!)

        # FIXME: Just do this after popup?
        print "AFTER LAYOUT", "WIDTH", self.width, "HEIGHT", self.height
        self.setWindowShape()


    def getRingClockwise(self, ring=0):
        clockwise = self.clockwise
        if type(clockwise) not in (type(()), type([])):
            clockwise = [clockwise]
        return clockwise[min(ring, len(clockwise) - 1)]


    def getRingInitialAngle(self, ring=0):
        initialAngle = self.initialAngle
        if type(initialAngle) not in (type(()), type([])):
            initialAngle = [initialAngle]
        return initialAngle[min(ring, len(initialAngle) - 1)]


    def measureHeader(self, context, pcontext, playout):

        header = self.header

        if header == None:
            self.headerWidth = 0
            self.headerHeight = 0
            return

        font = GetFont(self.headerFont)

        playout.set_font_description(font)
        playout.set_markup(header)
        width, height = playout.get_pixel_size()

        headerPadding = self.headerPadding
        width += 2 * headerPadding
        height += 2 * headerPadding

        self.headerWidth = width
        self.headerHeight = height


    def positionHeader(self, minX, minY, maxX, maxY):

        if self.header == None:
            headerX = 0
            headerY = 0
        else:

            headerWidth = self.headerWidth
            headerHeight = self.headerHeight
            headerMargin = self.headerMargin

            # Position the header horizontally.
            headerX = Floor(headerWidth / -2)

            # Make vertical space above the bounding box for the header.
            headerY = minY - headerHeight - self.headerGap

            minX = Floor(min(minX, headerX - headerMargin))
            maxX = Ceil(max(maxX, headerX + headerWidth + headerMargin))

            minY = Floor(min(minY, headerY - headerMargin))
            maxY = Ceil(max(maxY, headerY + headerHeight + headerMargin))

        self.headerX = headerX
        self.headerY = headerY

        return (minX, minY, maxX, maxY)


    def measureFooter(self, context, pcontext, playout):

        footer = self.footer
        footerFixedHeight = self.footerFixedHeight

        if (footer == None) and (footerFixedHeight > 0):
            self.footerWidth = 0
            self.footerHeight = 0
            return

        if not footer:
            footer = ''

        font = GetFont(self.footerFont)

        playout.set_font_description(font)
        playout.set_markup(footer)
        width, height = playout.get_pixel_size()

        footerPadding = self.footerPadding
        width += 2 * footerPadding
        height += 2 * footerPadding

        if footerFixedHeight > 0:
            height = footerFixedHeight

        self.footerWidth = width
        self.footerHeight = height


    def positionFooter(self, minX, minY, maxX, maxY):

        if self.footer == None:
            footerX = 0
            footerY = 0
        else:

            footerWidth = self.footerWidth
            footerHeight = self.footerHeight
            footerMargin = self.footerMargin

            # Position the footer horizontally.
            footerX = Floor(footerWidth / -2)

            # Make vertical space above the bounding box for the footer.
            footerY = maxY + self.footerGap

            minX = Floor(min(minX, footerX - footerMargin))
            maxX = Ceil(max(maxX, footerX + footerWidth + footerMargin))

            minY = Floor(min(minY, footerY - footerMargin))
            maxY = Ceil(max(maxY, footerY + footerHeight + footerMargin))

            # If (always or) fixed height footer, then expand it out horizontally.
            #if True or self.footerFixedHeight:
            #    footerWidth = maxX - minX
            #    self.footerWidth = footerWidth

        self.footerX = footerX
        self.footerY = footerY

        return (minX, minY, maxX, maxY)


    def setWindowShape(self):

        #print "SETWINDOWSHAPE", self.x, self.y, self.width, self.height
        x = int(self.x)
        y = int(self.y)
        width = int(self.width)
        height = int(self.height)

        if ((x != self.winX) or
            (y != self.winY)):
            self.changePosition(x, y)

        if ((width != self.winWidth) or
            (height != self.winHeight)):
            self.changeSize(width, height)


    def changePosition(self, x, y):

        x = int(x)
        y = int(y)

        self.winX = x
        self.winY = y

        self.move(x, y)


    def changeSize(self, width, height):

        print "CHANGESIZE", width, height

        width = int(width)
        height = int(height)

        self.winWidth = width
        self.winHeight = height

        self.resize(width, height)


    def popUp(self, pinX, pinY, pinned=False):

        self.pinX = Floor(pinX + 0.5)
        self.pinY = Floor(pinY + 0.5)
        self.pinned = pinned

        self.curItem = -1
        self.curRingIndex = 0
        self.item = None

        if self.footerDescriptions:
            self.footer = self.neutralDescription

        self.invalidate()
        self.setWindowShape()
        self.redraw()
       
        self.show_all()

        d = self.d
       
        d.grab_add()
        d.grab_focus()

        #print "W", self.window

        print "POINTER_GRAB"
        gtk.gdk.pointer_grab(
            d.window,
            True,
            gtk.gdk.BUTTON_PRESS_MASK |
            gtk.gdk.BUTTON_RELEASE_MASK |
            gtk.gdk.ENTER_NOTIFY_MASK |
            gtk.gdk.LEAVE_NOTIFY_MASK |
            gtk.gdk.POINTER_MOTION_MASK)

        gtk.gdk.keyboard_grab(
            d.window,
            owner_events=True)

        self.handlePopUp()

        self.handleItemChanged()


    def popDown(self):

        self.d.grab_remove()

        print "POINTER_UNGRAB"
        gtk.gdk.pointer_ungrab()

        self.hide()
       
        self.handlePopDown()

        curItem = self.curItem
        if curItem != -1:
            item = self.visibleItems[curItem]
            item.handleLolite()

        self.curItem = -1
        self.curRingIndex = 0
        self.item = None

        # TODO: restore focus?


    def handleExpose(self, widget, event):

        self.draw(widget, event)

        return False


    def draw(self, widget, event):

        context = widget.window.cairo_create()
        pcontext = widget.create_pango_context()
        playout = pango.Layout(pcontext)

        self.setWindowShape()

        rect = self.get_allocation()

        print "DRAW", "allocation", rect.width, rect.height, "win", self.winWidth, self.winHeight

        #rect.width = int(self.winWidth)
        #rect.height = int(self.winHeight)

        self.validate(rect, context, pcontext, playout)

        context.rectangle(
            event.area.x,
            event.area.y,
            event.area.width,
            event.area.height)
        context.clip()

        self.drawBackground(rect, context, pcontext, playout)

        self.drawHeader(rect, context, pcontext, playout)
        self.drawFooter(rect, context, pcontext, playout)

        curItem = self.curItem
        if curItem != -1:
            curItemObj = self.visibleItems[curItem]
        else:
            curItemObj = None

        curRingIndex = self.curRingIndex
        drawItemLater = None

        for item in self.items[::-1]:
            if item.index == curItem:
                drawItemLater = item
            else:
                # Deal with pie items specially.
                if not item.linear:

                    # Don't draw pie labels inside of current ring.
                    if item.ringIndex < curRingIndex:
                        continue

                    # Draw pie labels outside of current ring transparently.
                    # Oops, we can't just set the opacity in the graphics state,
                    # so we would have to pass it into item.draw, but not yet. 
                    if item.ringIndex > curRingIndex:
                        opacity = 0.5

                item.draw(rect, context, pcontext, playout)

        # Draw the current item last so it overlaps all other items.
        if drawItemLater:
            drawItemLater.draw(rect, context, pcontext, playout)

        self.drawOverlay(rect, context, pcontext, playout)


    def drawBackground(self, rect, context, pcontext, playout):

        centerX = self.centerX
        centerY = self.centerY

        context.rectangle(rect)
        context.clip()

        fillColor = self.outsideFillColor
       
        if fillColor:
            context.rectangle(rect)

            context.set_source_rgb(*fillColor)
            context.fill()

        curItem = self.curItem
        if curItem != -1:
            curItemObj = self.visibleItems[curItem]
        else:
            curItemObj = None

        curRingIndex = self.curRingIndex
        pieRings = self.pieRings
        ringRadius = self.ringRadius
        neutralRadius = self.neutralRadius
        radius = self.radius

        if pieRings and radius:

            fillColor = self.backgroundFillColor
            strokeColor = self.backgroundStrokeColor
            edgeStrokeColor = self.edgeStrokeColor

            for ringIndex in range(len(pieRings) - 1, -1, -1):
                ringItems = pieRings[ringIndex]

                ########################################################################
                # Draw the ring.

                if ringIndex == 0:
                    innerRadius  = neutralRadius
                    outerRadius = radius
                else:
                    innerRadius = radius + ((ringIndex - 1) * ringRadius)
                    outerRadius = radius + ((ringIndex) * ringRadius)

                opacity = 1.0
                if (ringIndex > curRingIndex):
                    opacity = 0.5

                if ringIndex != curRingIndex:

                    # Don't draw rings inside the current ring.
                    if ringIndex < (curRingIndex - 1):
                        continue

                    if strokeColor:

                        context.arc(
                            centerX,
                            centerY,
                            radius + (ringIndex * ringRadius),
                            0,
                            TwoPI)

                        #strokeColorAlpha = strokeColor + (opacity,)
                        #context.set_source_rgba(*strokeColorAlpha)
                        context.set_source_rgb(*strokeColor)
                        context.stroke()

                ########################################################################
                # Draw a circle around the radius.

                if fillColor or strokeColor or edgeStrokeColor:

                    if fillColor:

                        context.arc(
                            centerX,
                            centerY,
                            outerRadius,
                            0,
                            TwoPI)

                        #fillColorAlpha = fillColor + (opacity,)
                        #context.set_source_rgba(*fillColorAlpha)
                        context.set_source_rgb(*fillColor)
                        context.fill()

                    if (edgeStrokeColor and
                        (len(ringItems) > 1)):

                        for item in ringItems:

                            edge1Dx = item.edge1Dx
                            edge1Dy = item.edge1Dy

                            context.move_to(
                                centerX + (edge1Dx * neutralRadius),
                                centerY + (edge1Dy * neutralRadius))
                            context.line_to(
                                centerX + (edge1Dx * outerRadius),
                                centerY + (edge1Dy * outerRadius))

                        # If the pie is sliced, and it's not a whole
                        # pie, then draw the last trailing edge.

                        pieSlice = self.pieSlice
                        if (pieSlice > 0) and (pieSlice < 1):
                            item = ringItems[-1]

                            edge2Dx = item.edge2Dx
                            edge2Dy = item.edge2Dy

                            context.move_to(
                                centerX + (edge2Dx * neutralRadius),
                                centerY + (edge2Dy * neutralRadius))
                            context.line_to(
                                centerX + (edge2Dx * outerRadius),
                                centerY + (edge2Dy * outerRadius))

                        #edgeStrokeColorAlpha = edgeStrokeColor + (opacity,)
                        #context.set_source_rgba(*edgeStrokeColorAlpha)
                        context.set_source_rgb(*edgeStrokeColor)
                        context.stroke()

                    if strokeColor:

                        context.arc(
                            centerX,
                            centerY,
                            outerRadius,
                            0,
                            TwoPI)

                        #strokeColorAlpha = strokeColor + (opacity,)
                        #context.set_source_rgba(*strokeColorAlpha)
                        context.set_source_rgb(*strokeColor)
                        context.stroke()


            ########################################################################
            # Draw a circle around the neutralRadius,
            # and hilite if no item selected.

            if curItem == -1:
                fillColor = self.neutralHiliteFillColor
                strokeColor = self.neutralHiliteStrokeColor
            else:
                fillColor = self.neutralLoliteFillColor
                strokeColor = self.neutralLoliteStrokeColor

            if fillColor or strokeColor:

                if fillColor:
                    context.arc(
                        centerX,
                        centerY,
                        neutralRadius,
                        0,
                        TwoPI)

                    context.set_source_rgb(*fillColor)
                    if strokeColor:
                        context.fill_preserve()
                    else:
                        context.fill()

                if strokeColor:
                    context.arc(
                        centerX,
                        centerY,
                        neutralRadius,
                        0,
                        TwoPI)

                    context.set_source_rgb(*strokeColor)
                    context.stroke()


            ########################################################################
            # Draw the hilited slice.
           
            sliceHiliteFillColor = self.sliceHiliteFillColor
            sliceHiliteStrokeColor = self.sliceHiliteStrokeColor

            if ((sliceHiliteFillColor or sliceHiliteStrokeColor) and
                curItemObj):

                if curItemObj.linear:
                    pass # TODO: hilite linear items
                else:
                    edge1Angle = curItemObj.edge1Angle
                    edge2Angle = curItemObj.edge2Angle

                    ringClockwise = self.getRingClockwise(curRingIndex)

                    if not ringClockwise:
                        temp = edge1Angle
                        edge1ang = edge2Angle
                        edge2ang = temp

                    # If outer ring, then extend highlight to screen edge.
                    if len(pieRings) == 1:
                        innerRadius = self.neutralRadius
                        outerRadius = self.radius + self.popoutRadius
                    else:
                        if curRingIndex == 0:
                            innerRadius = self.neutralRadius
                            outerRadius = self.radius
                        else:
                            innerRadius = self.radius + (ringRadius * (curRingIndex - 1))
                            outerRadius = self.radius + (ringRadius * curRingIndex)
                            if curRingIndex == (len(pieRings) - 1):
                                outerRadius += self.popoutRadius

                    context.arc(
                        centerX,
                        centerY,
                        outerRadius,
                        edge1Angle,
                        edge2Angle)

                    context.arc_negative(
                        centerX,
                        centerY,
                        innerRadius,
                        edge2Angle,
                        edge1Angle)

                    context.close_path()

                if fillColor:
                    context.set_source_rgb(*sliceHiliteFillColor)
                    #context.set_source_rgb(*sliceHiliteFillColor + (0.5,))
                    if strokeColor:
                        context.fill_preserve()
                    else:
                        context.fill()

                if strokeColor:
                    context.set_source_rgb(*sliceHiliteStrokeColor)
                    #context.set_source_rgb(*sliceHiliteStrokeColor + (0.5,))
                    context.stroke()


    def drawHeader(self, rect, context, pcontext, playout):

        header = self.header

        if header == None:
            return

        x = self.headerX
        y = self.headerY
        width = self.headerWidth
        height = self.headerHeight

        context.rectangle(x, y, width, height)

        fillColor = self.headerFillColor
       
        if fillColor:
            context.rectangle(x, y, width, height)

            context.set_source_rgb(*fillColor)
            context.fill()

        strokeColor = self.headerStrokeColor
       
        if strokeColor:
            context.rectangle(x, y, width, height)

            context.set_source_rgb(*strokeColor)
            context.stroke()

        textColor = self.headerTextColor

        if textColor:

            context.set_source_rgb(*textColor)

            font = GetFont(self.headerFont)

            playout.set_font_description(font)
            playout.set_markup(header)

            headerPadding = self.headerPadding

            context.move_to(
                x + headerPadding,
                y + headerPadding)

            context.show_layout(playout)


    def drawFooter(self, rect, context, pcontext, playout):

        footer = self.footer

        if footer == None:
            return

        x = self.footerX
        y = self.footerY
        width = self.footerWidth
        height = self.footerHeight

        context.rectangle(x, y, width, height)

        fillColor = self.footerFillColor
       
        if fillColor:
            context.rectangle(x, y, width, height)

            context.set_source_rgb(*fillColor)
            context.fill()

        strokeColor = self.footerStrokeColor
       
        if strokeColor:
            context.rectangle(x, y, width, height)

            context.set_source_rgb(*strokeColor)
            context.stroke()

        textColor = self.footerTextColor

        if textColor:

            context.set_source_rgb(*textColor)

            font = GetFont(self.footerFont)

            playout.set_font_description(font)
            playout.set_markup(footer)

            footerPadding = self.footerPadding

            context.move_to(
                x + footerPadding,
                y + footerPadding)

            context.show_layout(playout)


    def drawOverlay(self, rect, context, pcontext, playout):

        strokeColor = self.outsideStrokeColor
       
        if strokeColor:
            context.rectangle(rect)

            context.set_source_rgb(*strokeColor)
            context.stroke()


    def setFooter(self, footer):
        self.footer = footer
        #self.layoutSelf()


    def redraw(self):
        self.d.queue_draw()


    def trackMouseMove(self, cx, cy):

        curX = self.curX
        curY = self.curY

        if ((cx == curX) and
            (cy == curY)):
            return
       
        self.lastX = curX
        self.lastY = curY
        self.curX = cx
        self.curY = cy

        # Track the selection based on the cursor offset from the menu center.

        dx = cx - self.centerX
        dy = cy - self.centerY

        # Add in and clear out any virtual mouse motion.
        dx += self.mouseDeltaX
        dy += self.mouseDeltaY
        self.mouseDeltaX = 0
        self.mouseDeltaY = 0

        self.dx = dx
        self.dy = dy

        self.distance = (
            Sqrt(
                (dx * dx) +
                (dy * dy)))

        if self.distance <= 0:
            self.direction = 0
        else:
            self.direction = NormalizeAngleRad(Atan2(-dy, dx))

        visibleItems = self.visibleItems
        itemCount = len(visibleItems)

        self.handleMotion()

        # If there aren't any items, there's nothing to do.
        if itemCount == 0:
            return

        newItem = -1
        lastItem = self.curItem

        self.lastItem = lastItem

        #print "trackMouseMove", "cx", cx, "cy", cy, "dx", dx, "dy", dy

        pieRings = self.pieRings

        curRingIndex = (
            max(0,
                min(len(pieRings) - 1,
                    Floor(
                        (self.distance - self.radius) /
                        self.ringRadius) + 1)))

        self.curRingIndex = curRingIndex
        curItemEntered = -1

        for i in range(0, itemCount):
            it = self.items[i]

            # Ignore the item if
            # there is more than one ring,
            # the item is a pie item,
            # and it's not in the currently selected ring.
            if ((len(pieRings) > 1) and
                (not it.linear) and
                (it.ringIndex != curRingIndex)):
                continue

            x0 = it.finalX
            y0 = it.finalY
            x1 = x0 + it.width
            y1 = y0 + it.height
            #print "trackMouseMove ITEM", "i", i, "cx", cx, "cy", cy, "x0", x0, "y0", y0, "x1", x1, "y1", y1

            if ((cx >= x0) and (cx < x1) and
                (cy >= y0) and (cy < y1)):
                #print "ENTERED", i
                curItemEntered = i
                break

        if curItemEntered != -1:
            newItem = curItemEntered
        else:
            if (pieRings and
                (self.distance > self.neutralRadius)):

                ringItems = pieRings[curRingIndex]
                ringItemCount = len(ringItems)

                pieSlice = self.pieSlice
                pieSubtend = TwoPI
                if pieSlice > 0:
                    pieSubtend *= pieSlice

                if ringItemCount == 1:
                    newItem = ringItems[0].index
                else:

                    if ringItemCount > 0:
                        ringInitialAngle = self.getRingInitialAngle(curRingIndex)
                        ang = DegToRad(ringInitialAngle)

                        ringClockwise = self.getRingClockwise(curRingIndex)

                        if pieSlice == 0:
                            twist = pieSubtend / ringItemCount / 2.0
                        else:
                            twist = 0

                        if ringClockwise:
                            ang = ang - self.direction + twist
                        else:
                            ang = ang + self.direction - twist

                        ang = NormalizeAngleRad(ang)

                        newPieItem = Floor((ang / pieSubtend) * ringItemCount)

                        if newPieItem < 0:
                            newPieItem = -1
                        elif newPieItem >= ringItemCount:
                            newPieItem = -1

                        if newPieItem == -1:
                            newItem = -1
                        else:
                            newItem = ringItems[newPieItem].index

        # Now we've figured out the selected newItem,
        # so update the display if necessary.

        if newItem >= len(self.visibleItems):
            newItem = itemCount - 1

        if newItem != lastItem:
            self.curItem = newItem

            item = None
            for i in range(0, itemCount):
                it = self.items[i]
                hilited = (i == newItem)
                if hilited:
                    item = it
                    break
            self.item = item

            if self.footerDescriptions:
                footer = None
                if item == None:
                    footer = self.neutralDescription
                else:
                    footer = item.description
                self.setFooter(footer)

            now = time.time()

            if lastItem != -1:
                it = self.items[lastItem]
                if it.enterTime == 0:
                    it.enterTime = now
                it.exitTime = now
                elapsed = now - it.enterTime
                it.totalTime += elapsed
                it.handleLolite()

            if newItem != -1:
                it = self.items[newItem]
                it.enterTime = now
                it.handleHilite()

            self.redraw()

            self.handleItemChanged()

        if newItem != -1:
            item = self.items[newItem]
            item.handleMotion()


    def handleMotion(self):

        # TODO: notify menu about mouse motion
        #print "handleMotion", self, "curItem", self.curItem, "distance", self.distance, "direction", self.direction
        pass


    def handleItemChanged(self):

        # TODO: notify menu that item changed
        #print "handleItemChanged", self, "curItem", self.curItem
        pass


    def handlePopUp(self):

        # TODO: notify menu that current item changed
        #print "handlePopUp", self
        pass


    def handlePopDown(self):

        # TODO: notify menu that current item changed
        #print "handlePopDown", self
        pass


    def handleAction(self):

        # TODO: notify menu that item was selected
        #print "handleAction", self, "curItem", self.curItem

        if self.action:
            self.action(self)


    def handleCancel(self):

        # TODO: notify menu that menu was canceled
        #print "handleCancel", self
        pass


    def handleSubmenu(self):

        # TODO: notify menu that submenu was popped up
        #print "handleSubmenu", self
        pass


    def trackMouseDown(self):

        pass


    def trackMouseUp(self):

        if ((self.curItem == -1) and
            (not self.pinned)):
            self.pinned = True
            return

        self.doAction()
        self.popDown()


    def doAction(self):

        curItem = self.curItem
        if curItem == -1:
            self.handleCancel()
            return

        item = self.visibleItems[curItem]
       
        #print "DOACTION", self, self.curItem, item, item.label

        subPie = item.subPie
        if subPie:
            x = self.curX + self.x
            y = self.curY + self.y
            subPie.popUp(x, y, True)
            self.handleSubmenu()
        else:
            self.handleAction()
            item.handleAction()


    def handleShow(self, widget):

        print "handleShow", self, widget
        pass


    def handleSizeAllocate(self, widget, rect):

        r = self.get_allocation()
        print "handleSizeAllocate", self, "rect", rect.width, rect.height, "allocation", r.width, r.height

        self.d.queue_draw()


    def handleMotionNotifyEvent(self, widget, event, *args):
        #print "handleMotionNotifyEvent", self, widget, event, args

        if (hasattr(event, 'is_hint') and
            event.is_hint):
            x, y, state = event.window.get_pointer()
        else:
            x = event.x
            y = event.y
            state = event.state

        self.trackMouseMove(x, y)


    def handleButtonPressEvent(self, widget, event, *args):
        print "handleButtonPressEvent", self, widget, event, args
        self.handleMotionNotifyEvent(widget, event, *args)
        self.trackMouseDown()


    def handleButtonReleaseEvent(self, widget, event, *args):
        print "handleButtonReleaseEvent", self, widget, event, args
        self.handleMotionNotifyEvent(widget, event, *args)
        self.trackMouseUp()


    def handleProximityInEvent(self, widget, event, *args):
        #print "handleProximityInEvent", self, widget, event, args
        self.handleMotionNotifyEvent(widget, event, *args)


    def handleProximityOutEvent(self, widget, event, *args):
        #print "handleProximityOutEvent", self, widget, event, args
        self.handleMotionNotifyEvent(widget, event, *args)


    def handleGrabNotify(self, widget, event, *args):
        print "handleGrabNotify", self, widget, event, args
        pass


    def handleGrabBrokenEvent(self, widget, event, *args):
        print "handleGrabBrokenEvent", self, widget, event, args
        pass


    def handleKeyPressEvent(self, widget, event, *args):
        #print "handleKeyPressEvent", self, widget, event, args
        #print help(event)
        pass


    def handleKeyReleaseEvent(self, widget, event, *args):
        #print "handleKeyReleaseEvent", self, widget, event, args
        pass


########################################################################


class PieMenuDrawingArea(gtk.DrawingArea):


    def __init__(self):

        gtk.DrawingArea.__init__(self)


########################################################################


class PieMenuTarget(gtk.Button):


    def __init__(
        self,
        label='',
        **args):

        gtk.Button.__init__(self, label=label, **args)

        self.connect("button_press_event", self.handleButtonPressEvent)
        self.connect("button_release_event", self.handleButtonReleaseEvent)

        #print "INIT"
       
        self.set_events(
            gtk.gdk.EXPOSURE_MASK |
            gtk.gdk.BUTTON_PRESS_MASK |
            gtk.gdk.BUTTON_RELEASE_MASK)


        self.pie = None


    def handleButtonPressEvent(self, widget, event):

        #print "PRESS"

        pie = self.pie

        if not pie:
            return False

        winX, winY, state = event.window.get_pointer()
       
        #print "WIN", winX, winY

        x, y = event.get_root_coords()

        #print "ROOT", x, y

        pie.popUp(x, y, False)

        return False


    def handleButtonReleaseEvent(self, widget, event):

        return False
   

    def setPie(self, pie):

        self.pie = pie


########################################################################


class LinearMenu(PieMenu):

    def __init__(
        self,
        **args):

        PieMenu.__init__(
            self,
            maxPieItems=0,
            **args)

   
########################################################################


class PurePieMenu(PieMenu):

    def __init__(
        self,
        **args):

        PieMenu.__init__(
            self,
            maxPieItems=1.0e+6,
            **args)


########################################################################


class DonutPieMenu(PieMenu):


    def __init__(
        self,
        **args):

        PieMenu.__init__(
            self,
            neutralRadius=60,
            minRadius=180,
            labelGapRadius=-85,
            **args)

   
########################################################################



def main():


    ########################################################################
    # Make Window and PieMenuTarget.


    win = gtk.Window()
    win.set_title("Pie Menus, by Don Hopkins")

    target = PieMenuTarget(label="Pie Menus")

    win.add(target)


    ########################################################################
    # Make diagSwitchPie.


    diagSwitchPie = PieMenu(
        header="Switch",
        initialAngle=45,
        minRadius=40)

    diagSwitchPie.addItem(
        PieItem(
            label='On'))

    diagSwitchPie.addItem(
        PieItem(
            label='Off'))


    ########################################################################
    # Make compassPie with extra linear overflow items.


    compassPie = PieMenu(
        header="Compass",
        footer="This menu has eight pie items,\nplus three overflow linear items.")

    for label in (
        'North', 'NE',
        'East', 'SE',
        'South', 'SW',
        'West', 'NW',
        'Linear', 'Overflow', 'Items',
    ):

        compassPie.addItem(
            PieItem(
                label=label))


    ########################################################################
    # Make clockPie with clock face.


    labels = (
        '12:', '01:', '02:', '03:', '04:', '05:', '06:', '07:', '08:', '09:', '10:', '11:',
        '24:', '13:', '14:', '15:', '16:', '17:', '18:', '19:', '20:', '21:', '22:', '23:',
        ':00', ':05', ':10', ':15', ':20', ':25', ':30', ':35', ':40', ':45', ':50', ':55'
    )

    clockPie = PieMenu(
        header="Clock",
        maxPieItems=(12, 12, 12,),
        fixedRadius=70,
        ringRadius=40)

    for label in labels:

        clockPie.addItem(
            PieItem(
                label=label,
                labelFont='Sans 12',
                loliteFillColor=None,
                loliteStrokeColor=None))


    ########################################################################
    # Make twoRingedPie with rings of items.


    twoRingedPie = PieMenu(
        header="Two Ringed",
        maxPieItems=(4, 8,),
        minRadius=20,
        ringRadius=50)

    labels = (
        'Top', 'Next', 'Bottom', 'Back',
        'North', 'NE', 'East', 'SE', 'South', 'SW', 'West', 'NW',
        'Linear', 'Overflow', 'Items'
    )

    for label in labels:

        twoRingedPie.addItem(
            PieItem(
                label=label,
                labelFont='Sans 12',
                loliteFillColor=None,
                loliteStrokeColor=None))


    ########################################################################
    # Make threeRingedPie with rings of items.


    threeRingedPie = PieMenu(
        header="Three Ringed",
        maxPieItems=(4, 8, 12,),
        minRadius=20,
        ringRadius=50)

    labels = (
        'Top', 'Next', 'Bottom', 'Back',
        'North', 'NE', 'East', 'SE', 'South', 'SW', 'West', 'NW',
        '12', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11',
        'Linear', 'Overflow', 'Items'
    )

    for label in labels:

        threeRingedPie.addItem(
            PieItem(
                label=label,
                labelFont='Sans 12',
                loliteFillColor=None,
                loliteStrokeColor=None))


    ########################################################################
    # Make fourRingedPie with rings of items.


    labels = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.?!/@#$%^&*()_-+=|\\`~<>[]{}'

    fourRingedPie = PieMenu(
        header="Four Ringed",
        maxPieItems=(4, 8, 24, 1000,),
        fixedRadius=30,
        ringRadius=40,
        minRadius=20)

    for label in labels:

        # Quote the label since we're using markup.
        label = label.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')

        fourRingedPie.addItem(
            PieItem(
                label=label,
                labelFont='Sans 10',
                loliteFillColor=None,
                loliteStrokeColor=None))


    ########################################################################
    # Make monthsLiner, monthsPie.


    months = (
      'January',
      'February',
      'March',
      'April',
      'May',
      'June',
      'July',
      'August',
      'September',
      'October',
      'November',
      'December')

    monthsLinear = LinearMenu(
        header="Months\n(Linear)")

    monthsPie = PieMenu(
        header="Months (Pie)",
        maxPieItems=(12,))

    for label in months:

        monthsLinear.addItem(
            PieItem(
                label=label))

        monthsPie.addItem(
            PieItem(
                label=label))


    ########################################################################
    # Make top level rootPie, with items with submenus.


    rootPie = PieMenu(
        fixedRadius=80,
        ringRadius=50,
        pieSlice=0.25,
        initialAngle=0,
        clockwise=True,
        maxPieItems=(3, 5, 8, 8, 16, 16, 16, 32, 32, 32, 32, 64, 64, 64, 64, 1000),
        header="<b>Pie Menus</b>",
        neutralDescription="This pie menu has submenus\nwith various pie menu demos!")

    rootPie.addItem(
        PieItem(
            #icon='/home/simcity/sugar/sugar-jhbuild/source/pycairo/examples/cairo_snippets/data/romedalen.png',
            iconSide='top',
            label="<b>Compass</b>...",
            description="Eight item compass pie menu,\nwith three overflow linear items.",
            subPie=compassPie))

    rootPie.addItem(
        PieItem(
            label="<b>Switch</b>...",
            description="Two item diagonal pie menu,\nalong same axis as submenu item.",
            subPie=diagSwitchPie))

    rootPie.addItem(
        PieItem(
            #icon='/home/simcity/sugar/sugar-jhbuild/source/pycairo/examples/cairo_snippets/data/romedalen.png',
            iconSide='right',
            label="<b>Clock</b>...",
            description="Three ringed pie menu clock,\nwith 24 hours plus minutes.",
            subPie=clockPie))

    rootPie.addItem(
        PieItem(
            label="<b><i>Four</i> Ringed</b>...",
            description="Four ringed pie menu,\nwith a whole bunch of items.",
            subPie=fourRingedPie))

    rootPie.addItem(
        PieItem(
            #icon='/home/simcity/sugar/sugar-jhbuild/source/pycairo/examples/cairo_snippets/data/romedalen.png',
            iconSide='bottom',
            label="<b><i>Three</i> Ringed</b>...",
            description="Three ringed pie menu,\nwith lots of items.",
            subPie=threeRingedPie))

    rootPie.addItem(
        PieItem(
            label="<b><i>Two</i> Ringed</b>...",
            description="Two ringed pie menu,\nwith a dozen items.",
            subPie=twoRingedPie))

    rootPie.addItem(
        PieItem(
            #icon='/home/simcity/sugar/sugar-jhbuild/source/pycairo/examples/cairo_snippets/data/romedalen.png',
            iconSide='left',
            label="<b><i>Pie</i> Months</b>...",
            description="Twelve month pie menu.",
            subPie=monthsPie))

    rootPie.addItem(
        PieItem(
            label="<b><i>Linear</i> Months</b>...",
            description="Twelve month linear menu.",
            subPie=monthsLinear))


    for i in range(0, 256):
        rootPie.addItem(
            PieItem(
                label="<b>" + str(i + 1) + "</b>",
                description="A dummy pie menu item."))


    ########################################################################


    target.setPie(rootPie)
    #target.setPie(compassPie)

    win.resize(300, 300)

    win.show_all()

    gtk.main()


########################################################################


if __name__ == '__main__':
    main()


########################################################################
