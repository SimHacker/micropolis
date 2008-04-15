#!/usr/bin/env python

########################################################################
# Pie Menus for pygtk and OLPC Sugar.
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

'''PieMenu

Pie menu component for Python, GTK, Cairo, Pango and OLPC Sugar.
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


def DegToRad(deg):
    return (2 * math.pi * deg) / 360.0


def RadToDeg(rad):
    return (360.0 * rad) / (2 * math.pi)


def NormalizeAngleRad(ang):
    two_pi = 2 * math.pi

    while ang < 0:
      ang += two_pi

    while ang >= two_pi:
      ang -= two_pi

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
            label_font='Helvetica 14',
            label_padding=2,
            label_x_adjust=0,
            label_y_adjust=2,
            icon=None,
            icon_hilite=None,
            icon_padding=2,
            icon_side='top',
            visible=True,
            pie=None,
            sub_pie=None,
            fixed_radius=0,
            linear=False,
            linear_direction='down',
            linear_order_reversed=False,
            linear_gap=0,
            hilite_fill_color=(1, 1, 0),
            hilite_stroke_color=(0, 0, 1),
            hilite_text_color=(0, 0, 1),
            lolite_fill_color=(1, 1, 1),
            lolite_stroke_color=(0, 0, 0),
            lolite_text_color=(0, 0, 0),
            scale=1.0,
            **args):

        #print "PIEITEM INIT", self, label, args
        self.label = label
        self.description = description
        self.action = action
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.label_font = label_font
        self.label_padding = label_padding
        self.label_x_adjust = label_x_adjust
        self.label_y_adjust = label_y_adjust
        self.icon = icon
        self.icon_hilite = icon_hilite
        self.icon_padding = icon_padding
        self.icon_side = icon_side
        self.visible = visible
        self.pie = None # Added later.
        self.sub_pie = sub_pie
        self.fixed_radius = fixed_radius
        self.linear = linear
        self.linear_direction = linear_direction
        self.linear_order_reversed = linear_order_reversed
        self.linear_gap = linear_gap
        self.hilite_fill_color = hilite_fill_color
        self.hilite_stroke_color = hilite_stroke_color
        self.hilite_text_color = hilite_text_color
        self.lolite_fill_color = lolite_fill_color
        self.lolite_stroke_color = lolite_stroke_color
        self.lolite_text_color = lolite_text_color
        self.scale = scale

        self.valid = False
        self.index = -1
        self.dx = 0
        self.dy = 0
        self.enter_time = 0
        self.exit_time = 0
        self.total_time = 0
        self.final_x = 0
        self.final_y = 0
        self.ring_index = None
        self.angle = 0
        self.edge1dx = 0
        self.edge1dy = 0
        self.edge1angle = 0
        self.edge2dx = 0
        self.edge2dy = 0
        self.edge2angle = 0
        self.label_x = 0
        self.label_y = 0
        self.label_width = 0
        self.label_height = 0
        self.icon_x = 0
        self.icon_y = 0
        self.icon_width = 0
        self.icon_height = 0

        if pie != None:
            pie.add_item(self)


    def measure(self, context, pcontext, playout):

        label = self.label

        width = 0
        height = 0

        label_x = 0
        label_y = 0
        label_width = 0
        label_height = 0

        icon_x = 0
        icon_y = 0
        icon_width = 0
        icon_height = 0

        label_padding = self.label_padding
        icon_padding = self.icon_padding

        if label != None:

            font = GetFont(self.label_font)

            playout.set_font_description(font)
            playout.set_markup(label)
            label_width, label_height = playout.get_pixel_size()

            label_width += 2 * label_padding
            label_height += 2 * label_padding

        icon = self.icon

        if icon != None:

            image = GetImage(icon)
            if image:

                icon_width = image.get_width()
                icon_height = image.get_height()

                icon_width += 2 * icon_padding
                icon_height += 2 * icon_padding

        if not label:
            if not icon:
                # No icon, no label.
                pass
            else:
                # Just an icon.
                width = icon_width
                height = icon_height
                icon_x = icon_padding
                icon_y = icon_padding
        else:
            if not icon:
                # Just a label.
                width = label_width
                height = label_height
                label_x = label_padding
                label_y = label_padding
            else:
                # Icon and label.
                icon_side = self.icon_side
                if icon_side in ('n', 'up', 'top'):
                    width = max(label_width, icon_width)
                    height = label_height + icon_height
                    icon_x = int(math.floor((width - icon_width) / 2)) + icon_padding
                    icon_y = icon_padding
                    label_x = int(math.floor((width - label_width) / 2)) + label_padding
                    label_y = icon_height + label_padding
                elif icon_side in ('nw', 'topleft'):
                    width = max(label_width, icon_width)
                    height = label_height + icon_height
                    icon_x = icon_padding
                    icon_y = icon_padding
                    label_x = label_padding
                    label_y = icon_height + label_padding
                elif icon_side in ('ne', 'topright'):
                    width = max(label_width, icon_width)
                    height = label_height + icon_height
                    icon_x = width - icon_width + icon_padding
                    icon_y = icon_padding
                    label_x = width - label_width + label_padding
                    label_y = icon_height + label_padding
                elif icon_side in ('s', 'down', 'bottom'):
                    width = max(label_width, icon_width)
                    height = label_height + icon_height
                    label_x = int(math.floor((width - label_width) / 2)) + label_padding
                    label_y = label_padding
                    icon_x = int(math.floor((width - icon_width) / 2)) + icon_padding
                    icon_y = label_height + icon_padding
                elif icon_side in ('sw', 'bottomleft'):
                    width = max(label_width, icon_width)
                    height = label_height + icon_height
                    label_x = label_padding
                    label_y = label_padding
                    icon_x = icon_padding
                    icon_y = label_height + icon_padding
                elif icon_side in ('se', 'bottomright'):
                    width = max(label_width, icon_width)
                    height = label_height + icon_height
                    label_x = width - label_width + label_padding
                    label_y = label_padding
                    icon_x = width - icon_width + icon_padding
                    icon_y = label_height + icon_padding
                elif icon_side in ('w', 'left'):
                    width = label_width + icon_width
                    height = max(label_height, icon_height)
                    icon_x = icon_padding
                    icon_y = int(math.floor((height - icon_height) / 2)) + icon_padding
                    label_x = icon_width + label_padding
                    label_y = int(math.floor((height - label_height) / 2)) + label_padding
                elif icon_side in ('e', 'right'):
                    width = label_width + icon_width
                    height = max(label_height, icon_height)
                    label_x = label_padding
                    label_y = int(math.floor((height - label_height) / 2)) + label_padding
                    icon_x = label_width + icon_padding
                    icon_y = int(math.floor((height - icon_height) / 2)) + icon_padding
                else:
                    print "Invalid icon_side: " + repr(icon_side)

        self.width = width
        self.height = height

        self.label_x = label_x
        self.label_y = label_y
        self.label_width = label_width
        self.label_height = label_height

        self.icon_x = icon_x
        self.icon_y = icon_y
        self.icon_width = icon_width
        self.icon_height = icon_height


    def layoutForPie(self, radius):
        if self.fixed_radius > 0:
            radius = self.fixed_radius;

        gap = 1
        rdx = radius * self.dx
        rdy = radius * self.dy

        # Special cases for top, bottom, left, and right
        if (math.floor(rdx + 0.5)) == 0: # top or bottom
            rdx -= (self.width / 2.0) + 1
            if rdy > 0:
                rdy += gap - 3; # top
            else:
                rdy -= self.height + gap - 2; # bottom
        else:
            if (math.floor(rdy + 0.5)) == 0: # left or right
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

        self.x = int(math.floor(rdx + 0.5))
        self.y = int(math.floor(rdy + 0.5))


    def layoutForLinear(self, min_x, min_y, max_x, max_y):
        pie = self.pie

        linear_direction = self.linear_direction
        if ((linear_direction == None) or
            (linear_direction == "")):
          linear_direction = self.pie.linear_direction

        pie = self.pie
        gap = self.linear_gap
        x_center_offset = 0
        y_center_offset = 0

        if linear_direction in ('c', 'center'):
            other_item = pie.addItemDirection(self, 'center')
            x_center_offset = int(math.floor(self.width / -2.0) - 1)
            y_center_offset = int(math.floor(self.height / -2.0))
        elif linear_direction in ('e', 'right'):
            other_item = pie.addItemDirection(self, 0)
            if other_item:
                x_center_offset = int(math.floor(other_item.x + other_item.width) + gap)
                y_center_offset = int(math.floor(self.height / -2.0))
            else:
                self.layoutForPie(pie.radius)
                return
        elif linear_direction in ('ne', 'topright'):
            other_item = pie.addItemDirection(self, 45)
            if other_item:
                x_center_offset = int(math.floor(other_item.x + other_item.width) + gap)
                y_center_offset = int(math.floor((other_item.y + other_item.height) - self.height))
            else:
                self.layoutForPie(pie.radius)
                return
        elif linear_direction in ('se', 'bottomright'):
            other_item = pie.addItemDirection(self, 315)
            if other_item:
                x_center_offset = int(math.floor(other_item.x + other_item.width) + gap)
                y_center_offset = int(math.floor(other_item.y))
            else:
                self.layoutForPie(pie.radius)
                return
        elif linear_direction in ('w', 'left'):
            other_item = pie.addItemDirection(self, 180)
            if other_item:
                x_center_offset = int(math.floor(other_item.x - self.width) - gap)
                y_center_offset = int(math.floor(self.height / -2.0))
            else:
                self.layoutForPie(pie.radius)
                return
        elif linear_direction in ('nw', 'topleft'):
            other_item = pie.addItemDirection(self, 135)
            if other_item:
                x_center_offset = int(math.floor(other_item.x - other_item.width) - gap)
                y_center_offset = int(math.floor((other_item.y + other_item.height) - self.height))
            else:
                self.layoutForPie(pie.radius)
                return
        elif linear_direction in ('sw', 'bottomleft'):
            other_item = pie.addItemDirection(self, 225)
            if other_item:
                x_center_offset = int(math.floor(other_item.x - self.width) - gap)
                y_center_offset = int(math.floor(other_item.y))
            else:
                self.layoutForPie(pie.radius)
                return
        elif linear_direction in ('n', 'up', 'top'):
            other_item = pie.addItemDirection(self, 90)
            if other_item:
                x_center_offset = int(math.floor(self.width / -2.0) - 1)
                y_center_offset = int(math.floor(other_item.y - self.height) - gap)
            else:
                self.layoutForPie(pie.radius)
                return
        elif linear_direction in ('s', 'down', 'bottom'):
            other_item = pie.addItemDirection(self, 270)
            if other_item:
                x_center_offset = int(math.floor(self.width / -2.0) - 1)
                y_center_offset = int(math.floor(other_item.y + other_item.height) + gap)
            else:
                self.layoutForPie(pie.radius)
                return

        self.x = int(math.floor(0.5 + x_center_offset))
        self.y = int(math.floor(0.5 + y_center_offset))

        self.ring_index = None


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

        hilited = self.index == self.pie.cur_item

        if hilited:
            fill_color = self.hilite_fill_color
            stroke_color = self.hilite_stroke_color
            text_color = self.hilite_text_color
        else:
            fill_color = self.lolite_fill_color
            stroke_color = self.lolite_stroke_color
            text_color = self.lolite_text_color

        if fill_color or stroke_color:

            context.rectangle(x, y, width, height)

            if fill_color:
                context.set_source_rgb(*fill_color)
                if stroke_color:
                    context.fill_preserve()
                else:
                    context.fill()

            if stroke_color:
                context.set_source_rgb(*stroke_color)
                context.stroke()

        label = self.label
        if text_color and (label != None):

            context.set_source_rgb(*text_color)

            font = GetFont(self.label_font)

            playout.set_font_description(font)
            playout.set_markup(label)

            context.move_to(
                x + self.label_x,
                y + self.label_y)

            context.show_layout(playout)

        hilited = self.index == self.pie.cur_item
        if hilited:
            icon = self.icon_hilite or self.icon
        else:
            icon = self.icon

        if icon:

            image = GetImage(icon)
            if image:

                context.set_source_surface(
                    image,
                    x + self.icon_x,
                    y + self.icon_y)

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
            outside_fill_color=(.9, .9, .9),
            outside_stroke_color=(0, 0, 0),
            background_fill_color=(1, 1, 1),
            background_stroke_color=(.5, .5, .5),
            edge_stroke_color=(.5, .5, .5),
            slice_hilite_fill_color=(0, 1, 0),
            slice_hilite_stroke_color=(0, 0, 1),
            neutral_radius=12,
            neutral_hilite_fill_color=(1, 0, 0),
            neutral_hilite_stroke_color=(0, 0, 1),
            neutral_lolite_fill_color=(1, 1, 1),
            neutral_lolite_stroke_color=(.5, .5, .5),
            neutral_description=None,
            popout_radius=1000,
            ring_radius = 40,
            header=None,
            header_fill_color=(0, 0, 0),
            header_stroke_color=None,
            header_text_color=(1, 1, 1),
            header_font='Helvetica 24',
            header_padding=2,
            header_margin=4,
            header_gap=4,
            header_x_adjust=0,
            header_y_adjust=3,
            footer=None,
            footer_fill_color=(1, 1, 0),
            footer_stroke_color=(0, 0, 1),
            footer_text_color=(0, 0, 1),
            footer_font='Helvetica 12',
            footer_padding=2,
            footer_margin=4,
            footer_gap=4,
            footer_x_adjust=0,
            footer_y_adjust=3,
            footer_fixed_height=0,
            footer_descriptions=True,
            clockwise=True,
            initial_angle=90,
            fixed_radius=0,
            min_radius=0,
            max_radius=0,
            extra_radius=0,
            label_gap_radius=8,
            margin_radius=5,
            radius_notch=2,
            popupedness=1,
            show_background=True,
            background_image=None,
            border=5,
            overlap_fringe=2,
            max_pie_items=8,
            linear=False,
            linear_direction='down',
            linear_order_reversed=False,
            transparent_items=True,
            item_border=2,
            item_margin=2,
            item_width=0,
            item_height=0,
            center_margin=16,
            center_border_width=2,
            center_visible=1,
            parent_pie=None,
            parent_item=None,
            pin_x=0,
            pin_y=0,
            popup_animation_duration=500,
            animate_popup_labels=True,
            animate_popup_opacity=False,
            animate_popup_background=True,
            shape_window=True,
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

        d = PieMenuDrawingArea()
        self.d = d
        self.add(self.d)

        self.connect("show", self.handle_show)

        d.connect("expose_event", self.handle_expose)
        d.connect("motion_notify_event", self.handle_motion_notify_event)
        d.connect("button_press_event", self.handle_button_press_event)
        d.connect("button_release_event", self.handle_button_release_event)
        d.connect("proximity_in_event", self.handle_proximity_in_event)
        d.connect("proximity_out_event", self.handle_proximity_out_event)
        d.connect("grab_notify", self.handle_grab_notify)
        d.connect("grab_broken_event", self.handle_grab_broken_event)
        d.connect("key_press_event", self.handle_key_press_event)
        d.connect("key_release_event", self.handle_key_release_event)

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
        
        self.action = action
        self.outside_fill_color = outside_fill_color
        self.outside_stroke_color = outside_stroke_color
        self.background_fill_color = background_fill_color
        self.background_stroke_color = background_stroke_color
        self.edge_stroke_color = edge_stroke_color
        self.slice_hilite_fill_color = slice_hilite_fill_color
        self.slice_hilite_stroke_color = slice_hilite_stroke_color
        self.neutral_radius = neutral_radius
        self.neutral_hilite_fill_color = neutral_hilite_fill_color
        self.neutral_hilite_stroke_color = neutral_hilite_stroke_color
        self.neutral_lolite_fill_color = neutral_lolite_fill_color
        self.neutral_lolite_stroke_color = neutral_lolite_stroke_color
        self.neutral_description = neutral_description
        self.popout_radius = popout_radius
        self.ring_radius = ring_radius
        self.header = header
        self.header_fill_color = header_fill_color
        self.header_stroke_color = header_stroke_color
        self.header_text_color = header_text_color
        self.header_font = header_font
        self.header_padding = header_padding
        self.header_margin = header_margin
        self.header_gap = header_gap
        self.header_x_adjust = header_x_adjust
        self.header_y_adjust = header_y_adjust
        self.footer = footer
        self.footer_fill_color = footer_fill_color
        self.footer_stroke_color = footer_stroke_color
        self.footer_text_color = footer_text_color
        self.footer_font = footer_font
        self.footer_padding = footer_padding
        self.footer_margin = footer_margin
        self.footer_gap = footer_gap
        self.footer_x_adjust = footer_x_adjust
        self.footer_y_adjust = footer_y_adjust
        self.footer_fixed_height = footer_fixed_height
        self.footer_descriptions = footer_descriptions
        self.clockwise = clockwise
        self.initial_angle = initial_angle
        self.fixed_radius = fixed_radius
        self.min_radius = min_radius
        self.max_radius = max_radius
        self.extra_radius = extra_radius
        self.label_gap_radius = label_gap_radius
        self.margin_radius = margin_radius
        self.radius_notch = radius_notch
        self.popupedness = popupedness
        self.show_background = show_background
        self.background_image = background_image
        self.border = border
        self.overlap_fringe = overlap_fringe
        self.max_pie_items = max_pie_items
        self.linear = linear
        self.linear_direction = linear_direction
        self.linear_order_reversed = linear_order_reversed
        self.transparent_items = transparent_items
        self.item_border = item_border
        self.item_margin = item_margin
        self.item_width = item_width
        self.item_height = item_height
        self.center_margin = center_margin
        self.center_border_width = center_border_width
        self.center_visible = center_visible
        self.parent_pie = parent_pie
        self.parent_item = parent_item
        self.popup_animation_duration = popup_animation_duration
        self.animate_popup_labels = animate_popup_labels
        self.animate_popup_opacity = animate_popup_opacity
        self.animate_popup_background = animate_popup_background
        self.shape_window = shape_window

        self.radius = 0
        self.center_x = 0
        self.center_y = 0
        self.inner_radius = 0
        self.header_x = 0
        self.header_y = 0
        self.header_width = 0
        self.header_height = 0
        self.footer_x = 0
        self.footer_y = 0
        self.footer_width = 0
        self.footer_height = 0
        self.cur_x = -1
        self.cur_y = -1
        self.last_x = -1
        self.last_y = -1
        self.mouse_delta_x = 0
        self.mouse_delta_y = 0
        self.direction = 0
        self.distance = 0
        self.items = []
        self.visible_items = []
        self.pie_rings = []
        self.linear_items = []
        self.item_directions = {}
        self.item = None
        self.cur_ring_index = 0
        self.cur_item = -1
        self.last_item = -1
        self.valid = False
        self.min_x = 0
        self.max_x = 0
        self.min_y = 0
        self.max_y = 0
        self.tracking_flag = False
        self.center_x = 0
        self.center_y = 0
        self.pinned = False
        self.win_x = None
        self.win_y = None
        self.win_width = None
        self.win_height = None


    def add_item(self, item):

        item.pie = self
        self.items.append(item)
        self.invalidate()


    def invalidate(self):

        self.valid = False


    def validate(self, rect, context, pcontext, playout):

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
            deg = \
                int(
                    math.floor(0.5 + 
                        RadToDeg(
                            NormalizeAngleRad(
                                angle))))
            dx = math.cos(angle)
            dy = -math.sin(angle)

        item.dx = dx
        item.dy = dy

        last_item = None

        a = self.item_directions.get(deg, None)
        if not a:
            a = []
            self.item_directions[deg] = a
        else:
            last_item = a[-1]

        a.append(item)

        return last_item


    def layoutSelf(self):
        context = self.window.cairo_create()
        pcontext = self.create_pango_context()
        playout = pango.Layout(pcontext)

        rect = self.get_allocation()

        self.layout(rect, context, pcontext, playout)


    def layout(self, rect, context, pcontext, playout):

        #print "PieMenu layout", self, rect, context, pcontext, playout

        # Just the visible items.
        visible_items = []
        self.visible_items = visible_items

        # The pie rings.
        pie_rings = []
        self.pie_rings = pie_rings

        # Just the visible pie menus items.
        pie_items = []

        # Just the visible linear menu items.
        linear_items = []
        self.linear_items = linear_items

        # Map of item direction to array of items in that direction.
        # Note: Think about how this interacts with rings and linear menu items. 
        item_directions = {}
        self.item_directions = item_directions

        # Put just the visible items from items into the visible_items array.
        for item in self.items:
            if item.visible:
                visible_items.append(item)

        # Count the visible items.
        item_count = len(visible_items)

        # We're done if no items. 
        if item_count == 0:
            return

        # Initialize constants and variables for layout.

        two_pi = 2 * math.pi
        overlap_fringe = self.overlap_fringe
        max_pie_items = self.max_pie_items
        if max_pie_items == None:
            max_pie_items = [1.0e+6]
        elif type(max_pie_items) not in (type(()), type([])):
            max_pie_items = [max_pie_items]
        max_pie_items_total = 0
        for i in max_pie_items:
            max_pie_items_total += i

        # Categorize items into pie_items and linear_items arrays. 
        item_index = 0
        pie_item_count = 0
        max_ring_items = max_pie_items[0]
        for item in visible_items:

          item.index = item_index
          item_index += 1
          item.valid = False

          # Limit the number of pie items to max_pie_items_total. 
          # Classify overflow items as linear.
          if pie_item_count >= max_pie_items_total:
              item.linear = True

          if item.linear:
              # Handle reverse ordering linear items.
              if item.linear_order_reversed:
                  # Prepend the linear item to the beginning of the linearItems array.
                  linear_items.insert(0, item)
              else:
                  # Append the linear item to the end of the linearItems array.
                  linear_items.append(item)
          else:
              # If this is the first item, then make the first ring.
              if len(pie_rings) == 0:
                  pie_rings.append(pie_items)

              # If this item will overflow the current ring, then make a new ring. 
              if len(pie_items) >= max_ring_items:
                  pie_items = []
                  pie_rings.append(pie_items)
                  ring_index = len(pie_rings) - 1
                  if ring_index < len(max_pie_items):
                      max_ring_items = max_pie_items[ring_index]
                  else:
                      print "Layout error: too many pie items, max_pie_items array did not add up right."

              # Append the pie item to the end of the pie_items array.
              pie_items.append(item)
              pie_item_count += 1

        # Measure the items.
        for item in self.items:
            item.x = 0
            item.y = 0
            item.measure(context, pcontext, playout)

        ring_count = len(pie_rings)
        linear_item_count = len(linear_items)

        # If there are any pie items, then calculate the pie menu parameters. 
        if ring_count > 0:
            
            ring_index = 0
            for pie_items in pie_rings:

                ring_item_count = len(pie_items)

                # Calculate the subtend, angle, cosine, sine, quadrant, slope,
                # and size of each pie menu item.

                # Calculate twist, the angular width of each slice.
                twist = two_pi / ring_item_count

                # Twist the other way if clockwise.
                ring_clockwise = self.getRingClockwise(ring_index)
                if ring_clockwise:
                    twist = -twist

                # Point ang towards the center of the first slice.

                ring_initial_angle = self.getRingInitialAngle(ring_index)
                ang = DegToRad(ring_initial_angle)

                # Twist backwards half a slice, to the edge of the slice.
                ang -= twist / 2.0

                # Now calculate the size and other properties of the pie items.
                for item in pie_items:

                    # Calculate angle, the center of the current slice.
                    angle = ang + (twist / 2.0)

                    # Add self item to the piemenu's list of items in the same direction.
                    self.addItemDirection(item, RadToDeg(angle))

                    # Calculate the unit vectors of the slice edge directions.
                    # Calculate ang in the upside-down coordinate system, for drawing.
                    item.ring_index = ring_index
                    item.angle = ang
                    item.edge1dx = math.cos(ang)
                    item.edge1dy = -math.sin(ang)
                    item.edge1angle = math.atan2(item.edge1dy, item.edge1dx)
                    item.edge2dx = math.cos(ang + twist)
                    item.edge2dy = -math.sin(ang + twist)
                    item.edge2angle = math.atan2(item.edge2dy, item.edge2dx)

                    # Twist ang around to the edge of the next slice.
                    ang += twist

                ring_index += 1

        # Determine the radius for the inner ring.
        fixed_radius = self.fixed_radius

        label_gap_radius = self.label_gap_radius

        # If the radius is not fixed, then calculate it dynamically.
        if fixed_radius > 0:

            # Use the fixed radius that has been specified.
            radius = fixed_radius
            
        else:

            # Start with the min_radius.
            radius = self.min_radius

            # If there are any pie items, then make sure they don't overlap. 
            if ring_count > 0:

                # Only apply this to the inner ring of the pie menu, for now.
                # The outer rings will each be a fixed ring_radius thick.
                # I think it's better to have explicit control over the
                # inner fixed_radius and outer ring_radius, so designers can
                # tune it to be easy to use, instead of leaving it up to the
                # label layout algorithm to determine (which could badly effect usability). 
                # Otherwise pie menus would be too big and the ring sizes would be irregular.
                # Could implement an array of ring_radius numbers to explicitly control
                # the radius of each ring, but that seems control-freakish, so not yet. 

                pie_items = pie_rings[0]

                # Increase the radius until there are no overlaps between 
                # any pie items.
                # Start by wrapping last around to the end of the 
                # circular menu.

                # Last index and last pie item, used to test for overlap.
                # Only test for last item overlap if more than one pie item. 
                last_index = len(pie_items) - 1
                last = None
                if last_index > 0:
                    last = pie_items[last_index]

                # Loop over all pie items testing for overlap with last adjacent
                # pie item. 
                for item in pie_items:

                    # Ignore fixed_radius items. 
                    # XXX: The behavior of mixing adjacent fixed_radius and not 
                    # fixed_radius pie menu items is not well defined.
                    # XXX: fixed_radius should be inherited from the piemenu.
                    if item.fixed_radius > 0:
                        continue

                    # Push the radius out until there are no overlaps.
                    # Give up after a while.
                    max_loops = 200
                    radius_notch = self.radius_notch

                    for loop_count in range(max_loops):

                        # Lay out the item at the current radius.
                        item.layoutForPie(radius + label_gap_radius)

                        # If there is only one item, then we're done pushing out. 
                        if last == None:
                            # Done pushing out.
                            break

                        # If there are more than one pie items, then test for adjacent overlaps. 
                        # Lay out the last item at the current radius.
                        last.layoutForPie(radius + label_gap_radius)

                        # Test for overlap. Takes two to tango.
                        if not item.itemsOverlap(last, overlap_fringe):
                            # They don't overlap, so we're done pushing out.
                            break

                        # The two adjacent labels overlap, so we keep looping and
                        # pushing them out until they don't.

                        # Bump the radius_notch.
                        radius += radius_notch

                    last = item

            # Add in the extra radius.
            radius += self.extra_radius

        # Done calculating the radius.
        self.radius = radius

        ring_radius = self.ring_radius
        outer_ring_radius = radius + ((len(pie_rings) - 1) * ring_radius)

        # Calculate the bounding box of the items, as we lay them out.
        max_x = -1000000
        min_x = 1000000
        max_y = -1000000
        min_y = 1000000

        if ring_count > 0:
            # If there are any pie items, then make sure the bounding box
            # encompasses the radius.
            min_x = -outer_ring_radius
            min_y = -outer_ring_radius
            max_x = outer_ring_radius
            max_y = outer_ring_radius

        # Calculate the maximum radius (squared).
        max_radius = 0

        # Loop over the pie items, and calculate their bounding box
        # and max_radius.

        ring_index = 0
        for pie_items in pie_rings:
            
            for item in pie_items:

                # Lay out the pie item at the current radius.
                item.layoutForPie(
                    radius + (ring_index * ring_radius) + label_gap_radius)

                # Calculate the corners of the item bounding box.
                itw = item.width
                ith = item.height
                itx0 = item.x
                ity0 = item.y
                itx1 = itx0 + itw
                ity1 = ity0 + ith

                # Update the bounding box.
                if itx0 < min_x:
                    min_x = itx0
                if ity0 < min_y:
                    min_y = ity0
                if itx1 > max_x:
                    max_x = itx1
                if ity1 > max_y:
                    max_y = ity1

                # Update the max_radius.
                farx = max(abs(itx0), abs(itx1))
                fary = max(abs(ity0), abs(ity1))
                rad = (farx * farx) + (fary * fary)

                if rad > self.max_radius: 
                    max_radius = rad; 

            ring_index += 1

        # Loop over the linear items, lay them out, 
        # and calculate their bounding box and max_radius.

        # Calculate the max width of the north and south linear items.
        max_item_width_north = 0
        max_item_width_south = 0

        for item in linear_items:
          # Lay out the linear item.
          item.layoutForLinear(
              min_x,
              min_y,
              max_x,
              max_y)

          # Calculate the max vertical item width.
          if abs(item.dx) < 0.01:
              if item.dy < 0:
                  if item.width > max_item_width_north:
                      max_item_width_north = item.width
                      #print "max_item_width_north", max_item_width_north
              else:
                  if item.width > max_item_width_south:
                      max_item_width_south = item.width
                      #print "max_item_width_south", max_item_width_south

          # Calculate the corners of the item bounding box.
          itw = item.width
          ith = item.height
          itx0 = item.x
          ity0 = item.y
          itx1 = itx0 + itw
          ity1 = ity0 + ith

          # Update the bounding box.
          if itx0 < min_x:
              min_x = itx0
          if ity0 < min_y:
              min_y = ity0
          if itx1 > max_x:
              max_x = itx1
          if ity1 > max_y:
              max_y = ity1

          # Update the max_radius.
          farx = max(abs(itx0), abs(itx1))
          fary = max(abs(ity0), abs(ity1))
          rad = (farx * farx) + (fary * fary)
          if rad > max_radius: 
              max_radius = rad; 

        # Go over the linear items and fix the x and width of all vertical items. 
        for item in linear_items:
          #print "item.dx", item.dx, item
          if abs(item.dx) < 0.01:
              w = 0
              if item.dy < 0:
                  w = max_item_width_north
              else:
                  w = max_item_width_south

              item.width = w
              item.x = int(math.floor(-0.5 * w) - 1)

        # Calculate the max_radius.
        max_radius = (
            int(
                math.floor(
                    0.95 + 
                    math.sqrt(
                        max_radius)) +
                self.margin_radius))
        self.max_radius = max_radius

        # Expand the bounding box by the border.
        border = self.border
        min_x -= border
        min_y -= border
        max_x += border
        max_y += border

        # Expand the bounding box to integers.
        min_x = int(math.floor(min_x))
        min_y = int(math.floor(min_y))
        max_x = int(math.ceil(max_x))
        max_y = int(math.ceil(max_y))

        # Measure the header and footer.
        self.measureHeader(context, pcontext, playout)
        self.measureFooter(context, pcontext, playout)

        # Position the header.
        header_width = self.header_width
        header_height = self.header_height
        header_margin = self.header_margin
        
        if self.header == None:
            header_x = 0
            header_y = 0
        else:
            # Position the header horizontally.
            header_x = int(math.floor(header_width / -2))

            # Make vertical space above the bounding box for the header.
            header_y = min_y - header_height - self.header_gap

            min_x = int(math.floor(min(min_x, header_x - header_margin)))
            max_x = int(math.ceil(max(max_x, header_x + header_width + header_margin)))

            min_y = int(math.floor(min(min_y, header_y - header_margin)))
            max_y = int(math.ceil(max(max_y, header_y + header_height + header_margin)))

        self.header_x = header_x
        self.header_y = header_y

        # Position the footer.
        footer_width = self.footer_width
        footer_height = self.footer_height
        footer_margin = self.footer_margin
        
        if self.footer == None:
            footer_x = 0
            footer_y = 0
        else:
            # Position the footer horizontally.
            footer_x = int(math.floor(footer_width / -2))

            # Make vertical space above the bounding box for the footer.
            footer_y = max_y + self.footer_gap

            min_x = int(math.floor(min(min_x, footer_x - footer_margin)))
            max_x = int(math.ceil(max(max_x, footer_x + footer_width + footer_margin)))

            min_y = int(math.floor(min(min_y, footer_y - footer_margin)))
            max_y = int(math.ceil(max(max_y, footer_y + footer_height + footer_margin)))

            # If (always or) fixed height footer, then expand it out horizontally.
            #if True or self.footer_fixed_height:
            #    footer_width = max_x - min_x
            #    self.footer_width = footer_width

        self.footer_x = footer_x
        self.footer_y = footer_y

        # Done calculating the bounding box.
        self.min_x = min_x
        self.min_y = min_y
        self.max_x = max_x
        self.max_y = max_y

        # Set the pie menu center.
        center_x = int(math.floor(0.5 + -min_x))
        center_y = int(math.floor(0.5 + -min_y))
        self.center_x = center_x
        self.center_y = center_y

        # Set the window position and size.

        width = max_x - min_x
        height = max_y - min_y

        if self.pie_rings:
            # If it's a pie menu, then center in the middle of the menu.
            x = self.pin_x + min_x
            y = self.pin_y + min_y
        else:
            # If it's a linear menu, then center on its header, or just below the mouse if no header. 
            x = self.pin_x - (width / 2)
            y = self.pin_y - (header_height / 2)

        self.x = x
        self.y = y
        self.width = width
        self.height =  height

        # Offset the header.
        self.header_x -= min_x
        self.header_y -= min_y

        # Offset the footer.
        self.footer_x -= min_x
        self.footer_y -= min_y

        # Offset the items.
        for item in visible_items:

            x = item.x
            y = item.y
            x -= min_x
            y -= min_y

            item.x = x
            item.y = y
            item.final_x = x
            item.final_y = y

        # Done laying out the pie menu. (Whew!)

        # FIXME: Just do this after popup?
        self.shapeWindow()


    def getRingClockwise(self, ring=0):
        clockwise = self.clockwise
        if type(clockwise) not in (type(()), type([])):
            clockwise = [clockwise]
        return clockwise[min(ring, len(clockwise) - 1)]


    def getRingInitialAngle(self, ring=0):
        initial_angle = self.initial_angle
        if type(initial_angle) not in (type(()), type([])):
            initial_angle = [initial_angle]
        return initial_angle[min(ring, len(initial_angle) - 1)]


    def measureHeader(self, context, pcontext, playout):
        header = self.header

        if header == None:
            self.header_width = 0
            self.header_height = 0
            return

        font = GetFont(self.header_font)

        playout.set_font_description(font)
        playout.set_markup(header)
        width, height = playout.get_pixel_size()

        header_padding = self.header_padding
        width += 2 * header_padding
        height += 2 * header_padding

        self.header_width = width
        self.header_height = height


    def measureFooter(self, context, pcontext, playout):
        footer = self.footer
        footer_fixed_height = self.footer_fixed_height

        if (footer == None) and (footer_fixed_height > 0):
            self.footer_width = 0
            self.footer_height = 0
            return

        if not footer:
            footer = ''

        font = GetFont(self.footer_font)

        playout.set_font_description(font)
        playout.set_markup(footer)
        width, height = playout.get_pixel_size()

        footer_padding = self.footer_padding
        width += 2 * footer_padding
        height += 2 * footer_padding

        if footer_fixed_height > 0:
            height = footer_fixed_height

        self.footer_width = width
        self.footer_height = height


    def shapeWindow(self):

        #print "SHAPEWINDOW", self.x, self.y, self.width, self.height
        x = int(self.x)
        y = int(self.y)
        width = int(self.width)
        height = int(self.height)

        if ((x != self.win_x) or
            (y != self.win_y)):
            self.move(x, y)
            self.win_x = x
            self.win_y = y

        if ((width != self.win_width) or
            (height != self.win_height)):
            self.resize(width, height)
            self.win_width = width
            self.win_height = height


    def popup(self, pin_x, pin_y, pinned=False):

        self.pin_x = int(math.floor(pin_x + 0.5))
        self.pin_y = int(math.floor(pin_y + 0.5))
        self.pinned = pinned

        self.cur_item = -1
        self.cur_ring_index = 0
        self.item = None

        if self.footer_descriptions:
            self.footer = self.neutral_description

        self.invalidate()
        self.queue_draw()
        
        self.show_all()

        d = self.d
        
        d.grab_add()
        d.grab_focus()

        #print "W", self.window

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

        self.handlePopup()

        self.handleItemChanged()


    def popdown(self):

        self.d.grab_remove()

        gtk.gdk.pointer_ungrab()

        self.hide()
        
        self.handlePopdown()

        cur_item = self.cur_item
        if cur_item != -1:
            item = self.visible_items[cur_item]
            item.handleLolite()

        self.cur_item = -1
        self.cur_ring_index = 0
        self.item = None

        # TODO: restore focus?


    def handle_expose(self, widget, event):

        self.draw(widget, event)

        return False


    def draw(self, widget, event):

        context = widget.window.cairo_create()
        pcontext = widget.create_pango_context()
        playout = pango.Layout(pcontext)

        rect = self.get_allocation()

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

        cur_item = self.cur_item
        if cur_item != -1:
            cur_item_obj = self.visible_items[cur_item]
        else:
            cur_item_obj = None

        cur_ring_index = self.cur_ring_index
        draw_item_later = None

        for item in self.items:
            if item.index == cur_item:
                draw_item_later = item
            else:
                # Don't draw labels inside of current ring.
                if ((not item.linear) and
                    (item.ring_index < cur_ring_index)):
                    continue
                
                item.draw(rect, context, pcontext, playout)

        # Draw the current item last so it overlaps all other items. 
        if draw_item_later:
            draw_item_later.draw(rect, context, pcontext, playout)

        self.drawOverlay(rect, context, pcontext, playout)


    def drawBackground(self, rect, context, pcontext, playout):

        center_x = self.center_x
        center_y = self.center_y

        context.rectangle(rect)
        context.clip()

        fill_color = self.outside_fill_color
        
        if fill_color:
            context.rectangle(rect)

            context.set_source_rgb(*fill_color)
            context.fill()

        cur_item = self.cur_item
        if cur_item != -1:
            cur_item_obj = self.visible_items[cur_item]
        else:
            cur_item_obj = None

        cur_ring_index = self.cur_ring_index
        pie_rings = self.pie_rings
        ring_radius = self.ring_radius
        neutral_radius = self.neutral_radius
        radius = self.radius

        if pie_rings and radius:

            fill_color = self.background_fill_color
            stroke_color = self.background_stroke_color
            edge_stroke_color = self.edge_stroke_color

            for ring_index in range(len(pie_rings) - 1, -1, -1):
                pie_items = pie_rings[ring_index]

                if ring_index == 0:
                    inner_radius  = neutral_radius
                    outer_radius = radius
                else:
                    inner_radius = radius + ((ring_index - 1) * ring_radius)
                    outer_radius = radius + ((ring_index) * ring_radius)

                if ring_index != cur_ring_index:
                    if ring_index < cur_ring_index:
                        continue

                    if stroke_color:

                        context.arc(
                            center_x,
                            center_y,
                            radius + (ring_index * ring_radius),
                            0,
                            2 * math.pi)

                        context.set_source_rgb(*stroke_color)
                        context.stroke()

                    continue


                ########################################################################
                # Draw a circle around the radius.

                if fill_color or stroke_color or edge_stroke_color:

                    if fill_color:

                        context.arc(
                            center_x,
                            center_y,
                            outer_radius,
                            0,
                            2 * math.pi)

                        context.set_source_rgb(*fill_color)
                        context.fill()

                    if (edge_stroke_color and
                        (len(pie_items) > 1)):

                        for item in pie_items:

                            edge1dx = item.edge1dx
                            edge1dy = item.edge1dy
                            edge2dx = item.edge2dx
                            edge2dy = item.edge2dy

                            context.move_to(
                                center_x + (edge1dx * neutral_radius),
                                center_y + (edge1dy * neutral_radius))
                            context.line_to(
                                center_x + (edge1dx * outer_radius),
                                center_y + (edge1dy * outer_radius))

                        context.set_source_rgb(*edge_stroke_color)
                        context.stroke()

                    if stroke_color:

                        context.arc(
                            center_x,
                            center_y,
                            outer_radius,
                            0,
                            2 * math.pi)

                        context.set_source_rgb(*stroke_color)
                        context.stroke()


            ########################################################################
            # Draw a circle around the neutral_radius,
            # and hilite if no item selected.

            if cur_item == -1:
                fill_color = self.neutral_hilite_fill_color
                stroke_color = self.neutral_hilite_stroke_color
            else:
                fill_color = self.neutral_lolite_fill_color
                stroke_color = self.neutral_lolite_stroke_color

            if fill_color or stroke_color:

                if fill_color:
                    context.arc(
                        center_x,
                        center_y,
                        neutral_radius,
                        0,
                        2 * math.pi)

                    context.set_source_rgb(*fill_color)
                    if stroke_color:
                        context.fill_preserve()
                    else:
                        context.fill()

                if stroke_color:
                    context.arc(
                        center_x,
                        center_y,
                        neutral_radius,
                        0,
                        2 * math.pi)

                    context.set_source_rgb(*stroke_color)
                    context.stroke()


            ########################################################################
            # Draw the hilited slice.
            
            slice_hilite_fill_color = self.slice_hilite_fill_color
            slice_hilite_stroke_color = self.slice_hilite_stroke_color

            if ((slice_hilite_fill_color or slice_hilite_stroke_color) and
                cur_item_obj):

                if cur_item_obj.linear:
                    pass # TODO: hilite linear items
                else:
                    edge1angle = cur_item_obj.edge1angle
                    edge2angle = cur_item_obj.edge2angle

                    ring_clockwise = self.getRingClockwise(cur_ring_index)

                    if not ring_clockwise:
                        temp = edge1angle
                        edge1ang = edge2angle
                        edge2ang = temp

                    if ring_index == 0:
                        inner_radius  = self.neutral_radius
                        outer_radius = self.radius
                    else:
                        inner_radius  = self.radius + ((ring_index - 1) * ring_radius)
                        outer_radius = self.radius + ((ring_index) * ring_radius)


                    # If outer ring, then extend highlight to screen edge.
                    if len(pie_rings) == 1:
                        inner_radius = self.neutral_radius
                        outer_radius = self.radius + self.popout_radius
                    else:
                        if cur_ring_index == 0:
                            inner_radius = self.neutral_radius
                            outer_radius = self.radius + ring_radius
                        else:
                            inner_radius = self.radius + (ring_radius * cur_ring_index)
                            outer_radius = self.radius + (ring_radius * (cur_ring_index + 1))
                            if cur_ring_index == (len(pie_rings) - 1):
                                outer_radius += self.popout_radius

                    context.arc(
                        center_x,
                        center_y,
                        outer_radius,
                        edge1angle,
                        edge2angle)

                    context.arc_negative(
                        center_x,
                        center_y,
                        inner_radius,
                        edge2angle,
                        edge1angle)

                    context.close_path()

                if fill_color:
                    context.set_source_rgb(*slice_hilite_fill_color)
                    #context.set_source_rgb(*slice_hilite_fill_color + (0.5,))
                    if stroke_color:
                        context.fill_preserve()
                    else:
                        context.fill()

                if stroke_color:
                    context.set_source_rgb(*slice_hilite_stroke_color)
                    #context.set_source_rgb(*slice_hilite_stroke_color + (0.5,))
                    context.stroke()


    def drawHeader(self, rect, context, pcontext, playout):

        header = self.header

        if header == None:
            return

        x = self.header_x
        y = self.header_y
        width = self.header_width
        height = self.header_height

        context.rectangle(x, y, width, height)

        fill_color = self.header_fill_color
        
        if fill_color:
            context.rectangle(x, y, width, height)

            context.set_source_rgb(*fill_color)
            context.fill()

        stroke_color = self.header_stroke_color
        
        if stroke_color:
            context.rectangle(x, y, width, height)

            context.set_source_rgb(*stroke_color)
            context.stroke()

        text_color = self.header_text_color

        if text_color:

            context.set_source_rgb(*text_color)

            font = GetFont(self.header_font)

            playout.set_font_description(font)
            playout.set_markup(header)

            header_padding = self.header_padding

            context.move_to(
                x + header_padding,
                y + header_padding)

            context.show_layout(playout)


    def drawFooter(self, rect, context, pcontext, playout):

        footer = self.footer

        if footer == None:
            return

        x = self.footer_x
        y = self.footer_y
        width = self.footer_width
        height = self.footer_height

        context.rectangle(x, y, width, height)

        fill_color = self.footer_fill_color
        
        if fill_color:
            context.rectangle(x, y, width, height)

            context.set_source_rgb(*fill_color)
            context.fill()

        stroke_color = self.footer_stroke_color
        
        if stroke_color:
            context.rectangle(x, y, width, height)

            context.set_source_rgb(*stroke_color)
            context.stroke()

        text_color = self.footer_text_color

        if text_color:

            context.set_source_rgb(*text_color)

            font = GetFont(self.footer_font)

            playout.set_font_description(font)
            playout.set_markup(footer)

            footer_padding = self.footer_padding

            context.move_to(
                x + footer_padding,
                y + footer_padding)

            context.show_layout(playout)


    def drawOverlay(self, rect, context, pcontext, playout):

        stroke_color = self.outside_stroke_color
        
        if stroke_color:
            context.rectangle(rect)

            context.set_source_rgb(*stroke_color)
            context.stroke()


    def setFooter(self, footer):
        self.footer = footer
        self.layoutSelf()


    def trackMouseMove(self, cx, cy):

        cur_x = self.cur_x
        cur_y = self.cur_y
        
        if ((cx == cur_x) and
            (cy == cur_y)):
            return
        
        self.last_x = cur_x
        self.lasy_y = cur_y
        self.cur_x = cx
        self.cur_y = cy

        # Track the selection based on the cursor offset from the menu center. 

        two_pi = 2 * math.pi

        dx = cx - self.center_x
        dy = cy - self.center_y

        # Add in and clear out any virtual mouse motion.
        dx += self.mouse_delta_x
        dy += self.mouse_delta_y
        self.mouse_delta_x = 0
        self.mouse_delta_y = 0

        self.dx = dx
        self.dy = dy

        self.distance = (
            math.sqrt(
                (dx * dx) +
                (dy * dy)))

        if self.distance <= 0:
            self.direction = 0
        else:
            self.direction = NormalizeAngleRad(math.atan2(-dy, dx))

        visible_items = self.visible_items
        item_count = len(visible_items)

        self.handleMotion()

        # If there aren't any items, there's nothing to do.
        if item_count == 0:
            return

        new_item = -1
        last_item = self.cur_item

        self.last_item = last_item

        #print "trackMouseMove", "cx", cx, "cy", cy, "dx", dx, "dy", dy

        pie_rings = self.pie_rings

        cur_ring_index = (
            max(0,
                min(len(pie_rings) - 1,
                    int(math.floor(
                        (self.distance - self.radius) /
                        self.ring_radius)))))

        self.cur_ring_index = cur_ring_index
        cur_item_entered = -1

        for i in range(0, item_count):
            it = self.items[i]

            # Ignore the item if
            # there is more than one ring,
            # the item is a pie item,
            # and it's not in the currently selected ring.
            if ((len(pie_rings) > 1) and
                (not it.linear) and
                (it.ring_index != cur_ring_index)):
                continue

            x0 = it.final_x
            y0 = it.final_y
            x1 = x0 + it.width
            y1 = y0 + it.height
            #print "trackMouseMove ITEM", "i", i, "cx", cx, "cy", cy, "x0", x0, "y0", y0, "x1", x1, "y1", y1

            if ((cx >= x0) and (cx < x1) and
                (cy >= y0) and (cy < y1)):
                #print "ENTERED", i
                cur_item_entered = i
                break

        if cur_item_entered != -1:
            new_item = cur_item_entered
        else:
            if (pie_rings and
                (self.distance > self.neutral_radius)):
                pie_items = pie_rings[cur_ring_index]
                pie_item_count = len(pie_items)

                if pie_item_count == 1:
                    new_item = pie_items[0].index
                else:
                    if cur_item_entered != -1:
                        new_item = cur_item_entered
                    else:
                        
                        if pie_item_count > 0:
                            twist = math.pi / pie_item_count

                            ring_initial_angle = self.getRingInitialAngle(cur_ring_index)
                            ang = DegToRad(ring_initial_angle)

                            ring_clockwise = self.getRingClockwise(cur_ring_index)
                            
                            if ring_clockwise:
                                ang = ang - self.direction + twist
                            else:
                                ang = ang + self.direction - twist

                            ang = NormalizeAngleRad(ang)

                            new_pie_item = int(math.floor((ang / two_pi) * pie_item_count))

                            if new_pie_item < 0:
                                new_pie_item = 0
                            elif new_pie_item >= pie_item_count:
                                new_pie_item = pie_item_count - 1

                            new_item = pie_items[new_pie_item].index

        # Now we've figured out the selected new_item,
        # so update the display if necessary.

        if new_item >= len(self.visible_items):
            new_item = item_count - 1

        if new_item != last_item:
            self.cur_item = new_item

            item = None
            for i in range(0, item_count):
                it = self.items[i]
                hilited = (i == new_item)
                if hilited:
                    item = it
                    break
            self.item = item

            if self.footer_descriptions:
                footer = None
                if item == None:
                    footer = self.neutral_description
                else:
                    footer = item.description
                self.setFooter(footer)

            now = time.time()

            if last_item != -1:
                it = self.items[last_item]
                if it.enter_time == 0:
                    it.enter_time = now
                it.exit_time = now
                elapsed = now - it.enter_time
                it.total_time += elapsed
                it.handleLolite()

            if new_item != -1:
                it = self.items[new_item]
                it.enter_time = now
                it.handleHilite()

            self.queue_draw()

            self.handleItemChanged()

        if new_item != -1:
            item = self.items[new_item]
            item.handleMotion()


    def handleMotion(self):

        # TODO: notify menu about mouse motion
        #print "handleMotion", self, "cur_item", self.cur_item, "distance", self.distance, "direction", self.direction
        pass


    def handleItemChanged(self):

        # TODO: notify menu that item changed
        #print "handleItemChanged", self, "cur_item", self.cur_item
        pass


    def handlePopup(self):

        # TODO: notify menu that current item changed
        #print "handlePopup", self
        pass


    def handlePopdown(self):

        # TODO: notify menu that current item changed
        #print "handlePopdown", self
        pass


    def handleAction(self):

        # TODO: notify menu that item was selected
        #print "handleAction", self, "cur_item", self.cur_item

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

        if ((self.cur_item == -1) and
            (not self.pinned)):
            self.pinned = True
            return

        self.doAction()
        self.popdown()


    def doAction(self):

        cur_item = self.cur_item
        if cur_item == -1:
            self.handleCancel()
            return

        item = self.visible_items[cur_item]
        
        #print "DOACTION", self, self.cur_item, item, item.label

        sub_pie = item.sub_pie
        if sub_pie:
            x = self.cur_x + self.x
            y = self.cur_y + self.y
            sub_pie.popup(x, y, True)
            self.handleSubmenu()
        else:
            self.handleAction()
            item.handleAction()


    def handle_show(self, widget):
        #print "handle_show", self, widget
        pass


    def handle_motion_notify_event(self, widget, event, *args):
        #print "handle_motion_notify_event", self, widget, event, args

        if (hasattr(event, 'is_hint') and
            event.is_hint):
            x, y, state = event.window.get_pointer()
        else:
            x = event.x
            y = event.y
            state = event.state

        self.trackMouseMove(x, y)


    def handle_button_press_event(self, widget, event, *args):
        #print "handle_button_press_event", self, widget, event, args
        self.handle_motion_notify_event(widget, event, *args)
        self.trackMouseDown()


    def handle_button_release_event(self, widget, event, *args):
        #print "handle_button_release_event", self, widget, event, args
        self.handle_motion_notify_event(widget, event, *args)
        self.trackMouseUp()


    def handle_proximity_in_event(self, widget, event, *args):
        #print "handle_proximity_in_event", self, widget, event, args
        self.handle_motion_notify_event(widget, event, *args)


    def handle_proximity_out_event(self, widget, event, *args):
        #print "handle_proximity_out_event", self, widget, event, args
        self.handle_motion_notify_event(widget, event, *args)


    def handle_grab_notify(self, widget, event, *args):
        #print "handle_grab_notify", self, widget, event, args
        pass


    def handle_grab_broken_event(self, widget, event, *args):
        #print "handle_grab_broken_event", self, widget, event, args
        pass


    def handle_key_press_event(self, widget, event, *args):
        #print "handle_key_press_event", self, widget, event, args
        #print help(event)
        pass


    def handle_key_release_event(self, widget, event, *args):
        #print "handle_key_release_event", self, widget, event, args
        pass


########################################################################


class PieMenuDrawingArea(gtk.DrawingArea):


    def __init__(self):

        gtk.DrawingArea.__init__(self)


    def handle_expose(self, widget, event):

        self.parent.handle_expose(widget, event)
        return False


########################################################################


class PieMenuTarget(gtk.Button):


    def __init__(
        self,
        label='',
        **args):

        gtk.Button.__init__(self, label=label, **args)

        self.connect("button_press_event", self.handle_button_press_event)
        self.connect("button_release_event", self.handle_button_release_event)

        #print "INIT"
        
        self.set_events(
            gtk.gdk.EXPOSURE_MASK |
            gtk.gdk.BUTTON_PRESS_MASK |
            gtk.gdk.BUTTON_RELEASE_MASK)


        self.pie = None


    def handle_button_press_event(self, widget, event):

        #print "PRESS"

        pie = self.pie

        if not pie:
            return False

        win_x, win_y, state = event.window.get_pointer()
        
        #print "WIN", win_x, win_y

        x, y = event.get_root_coords()

        #print "ROOT", x, y

        pie.popup(x, y, False)

        return False


    def handle_button_release_event(self, widget, event):

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
            max_pie_items=0,
            **args)

    
########################################################################


class PurePieMenu(PieMenu):

    def __init__(
        self,
        **args):

        PieMenu.__init__(
            self,
            max_pie_items=1.0e+6,
            **args)


########################################################################


class DozenPieMenu(PieMenu):

    def __init__(
        self,
        **args):

        PieMenu.__init__(
            self,
            max_pie_items=12,
            **args)

    
########################################################################


class DonutPieMenu(PieMenu):


    def __init__(
        self,
        **args):

        PieMenu.__init__(
            self,
            neutral_radius=60,
            min_radius=180,
            label_gap_radius=-85,
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
    # Make diag_switch_pie.


    diag_switch_pie = PieMenu(
        header="Switch",
        initial_angle=45,
        min_radius=40)

    diag_switch_pie.add_item(
        PieItem(
            label='On'))

    diag_switch_pie.add_item(
        PieItem(
            label='Off'))


    ########################################################################
    # Make compass_pie with extra linear overflow items.


    compass_pie = PieMenu(
        header="Compass",
        footer="This menu has eight pie items,\nplus three overflow linear items.")

    for label in (
        'North', 'NE',
        'East', 'SE',
        'South', 'SW',
        'West', 'NW',
        'Linear', 'Overflow', 'Items',
    ):

        compass_pie.add_item(
            PieItem(
                label=label))


    ########################################################################
    # Make clock_pie with clock face.


    labels = (
        '12:', '01:', '02:', '03:', '04:', '05:', '06:', '07:', '08:', '09:', '10:', '11:',
        '24:', '13:', '14:', '15:', '16:', '17:', '18:', '19:', '20:', '21:', '22:', '23:',
        ':00', ':05', ':10', ':15', ':20', ':25', ':30', ':35', ':40', ':45', ':50', ':55'
    )

    clock_pie = PieMenu(
        header="Clock",
        max_pie_items=[12, 12, 12],
        fixed_radius=70,
        ring_radius=40)

    for label in labels:

        clock_pie.add_item(
            PieItem(
                label=label,
                label_font='Helvetica 12',
                lolite_fill_color=None,
                lolite_stroke_color=None))


    ########################################################################
    # Make two_ringed_pie with rings of items.


    two_ringed_pie = PieMenu(
        header="Two Ringed",
        max_pie_items=[4, 8],
        min_radius=20,
        ring_radius=50)

    labels = (
        'Top', 'Next', 'Bottom', 'Back',
        'North', 'NE', 'East', 'SE', 'South', 'SW', 'West', 'NW',
        'Linear', 'Overflow', 'Items'
    )

    for label in labels:

        two_ringed_pie.add_item(
            PieItem(
                label=label,
                label_font='Helvetica 12',
                lolite_fill_color=None,
                lolite_stroke_color=None))


    ########################################################################
    # Make three_ringed_pie with rings of items.


    three_ringed_pie = PieMenu(
        header="Three Ringed",
        max_pie_items=[4, 8, 12],
        min_radius=20,
        ring_radius=50)

    labels = (
        'Top', 'Next', 'Bottom', 'Back',
        'North', 'NE', 'East', 'SE', 'South', 'SW', 'West', 'NW',
        '12', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11',
        'Linear', 'Overflow', 'Items'
    )

    for label in labels:

        three_ringed_pie.add_item(
            PieItem(
                label=label,
                label_font='Helvetica 12',
                lolite_fill_color=None,
                lolite_stroke_color=None))


    ########################################################################
    # Make four_ringed_pie with rings of items.


    labels = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.?!/@#$%^&*()_-+=|\\`~<>[]{}'

    four_ringed_pie = PieMenu(
        header="Four Ringed",
        max_pie_items=[4, 8, 24, 1000],
        min_radius=20)

    for label in labels:

        # Quote the label since we're using markup.
        label = label.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')

        four_ringed_pie.add_item(
            PieItem(
                label=label,
                label_font='Helvetica 10',
                lolite_fill_color=None,
                lolite_stroke_color=None))


    ########################################################################
    # Make months_liner, months_pie.


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

    months_linear = LinearMenu(
        header="Months (Linear)")

    months_pie = DozenPieMenu(
        header="Months (Pie)")

    for label in months:

        months_linear.add_item(
            PieItem(
                label=label))

        months_pie.add_item(
            PieItem(
                label=label))


    ########################################################################
    # Make top level root_pie, with items with submenus.


    root_pie = PieMenu(
        min_radius=40,
        header="<b>Pie Menus</b>",
        neutral_description="This pie menu has submenus\nwith various pie menu demos!")

    root_pie.add_item(
        PieItem(
            #icon='/home/simcity/sugar/sugar-jhbuild/source/pycairo/examples/cairo_snippets/data/romedalen.png',
            icon_side='top',
            label="<b>Compass</b>...",
            description="Eight item compass pie menu,\nwith three overflow linear items.",
            sub_pie=compass_pie))

    root_pie.add_item(
        PieItem(
            label="<b>Switch</b>...",
            description="Two item diagonal pie menu,\nalong same axis as submenu item.",
            sub_pie=diag_switch_pie))

    root_pie.add_item(
        PieItem(
            #icon='/home/simcity/sugar/sugar-jhbuild/source/pycairo/examples/cairo_snippets/data/romedalen.png',
            icon_side='right',
            label="<b>Clock</b>...",
            description="Three ringed pie menu clock,\nwith 24 hours plus minutes.",
            sub_pie=clock_pie))

    root_pie.add_item(
        PieItem(
            label="<b><i>Four</i> Ringed</b>...",
            description="Four ringed pie menu,\nwith a whole bunch of items.",
            sub_pie=four_ringed_pie))

    root_pie.add_item(
        PieItem(
            #icon='/home/simcity/sugar/sugar-jhbuild/source/pycairo/examples/cairo_snippets/data/romedalen.png',
            icon_side='bottom',
            label="<b><i>Three</i> Ringed</b>...",
            description="Three ringed pie menu,\nwith lots of items.",
            sub_pie=three_ringed_pie))

    root_pie.add_item(
        PieItem(
            label="<b><i>Two</i> Ringed</b>...",
            description="Two ringed pie menu,\nwith a dozen items.",
            sub_pie=two_ringed_pie))

    root_pie.add_item(
        PieItem(
            #icon='/home/simcity/sugar/sugar-jhbuild/source/pycairo/examples/cairo_snippets/data/romedalen.png',
            icon_side='left',
            label="<b><i>Pie</i> Months</b>...",
            description="Twelve month pie menu.",
            sub_pie=months_pie))

    root_pie.add_item(
        PieItem(
            label="<b><i>Linear</i> Months</b>...",
            description="Twelve month linear menu.",
            sub_pie=months_linear))


    ########################################################################


    target.setPie(root_pie)
    #target.setPie(compass_pie)

    win.resize(300, 300)

    win.show_all()

    gtk.main()


########################################################################


if __name__ == '__main__':
    main()


########################################################################
