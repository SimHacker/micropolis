# micropolisnotebook.py
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
# Micropolis Notebook
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
# MicropolisNotebook

class MicropolisNotebook(gtk.Notebook):


    def __init__(
        self,
        target=None,
        groupID=1,
        **args):

        gtk.Notebook.__init__(self, **args)

        self.target = target

        self.set_group_id(groupID)

        self.drag_dest_set(
            gtk.DEST_DEFAULT_ALL, # gtk.DEST_DEFAULT_MOTION,
            [("tab-move", gtk.TARGET_SAME_APP, groupID)],
            gtk.gdk.ACTION_MOVE)

        def on_dnd_drag_leave(sender, context, time):
            print "on_dnd_drag_leave"
            sender.modify_bg(gtk.STATE_NORMAL, None)

        def on_dnd_drag_motion(sender, context, x, y, time):
            print "on_dnd_drag_motion"
            color = gtk.gdk.Color(65535,0,0)
            sender.modify_bg(gtk.STATE_NORMAL, color)

        def on_dnd_drag_drop(sender, context, x, y, time):
            print "on_dnd_drag_drop"

        self.connect("drag-leave", on_dnd_drag_leave)
        self.connect("drag-motion", on_dnd_drag_motion)
        self.connect("drag-drop", on_dnd_drag_drop)
        self.connect('change-current-page', self.changeCurrentPage)
        self.connect('create-window', self.createWindow)
        self.connect('focus-tab', self.focusTab)
        self.connect('move-focus-out', self.moveFocusOut)
        self.connect('page-added', self.pageAdded)
        self.connect('page-removed', self.pageRemoved)
        self.connect('page-reordered', self.pageReordered)
        self.connect('select-page', self.selectPage)
        self.connect('switch-page', self.switchPage)


    def addLabelTab(self, label, tab):
        tabLabel = gtk.Label(label)
        self.append_page(tab, tabLabel)
        self.set_tab_reorderable(tab, True)
        self.set_tab_detachable(tab, True)


    def changeCurrentPage(
        self,
        notebook,
        offset):
        print "CHANGECURRENTPAGE", self, notebook, offset


    def createWindow(
        self,
        notebook,
        page,
        x,
        y):
        print "==== CREATEWINDOW", self, notebook, page, x, y
        return self.target.createWindowNotebook(self, notebook, page, x, y)


    def focusTab(
        self,
        notebook,
        type):
        print "FOCUSTAB", self, notebook, type
        return False


    def moveFocusOut(
        self,
        notebook,
        directionType):
        print "MOVEFOCUSOUT", self, notebook, directionType


    def pageAdded(
        self,
        notebook,
        child,
        pageNumber):
        print "PAGEADDED", self, child, pageNumber


    def pageRemoved(
        self,
        notebook,
        child,
        pageNumber):
        print "PAGEREMOVED", self, notebook, child, pageNumber


    def pageReordered(
        self,
        notebook,
        child,
        pageNumber):
        print "PAGEREORDERED", self, notebook, child, pageNumber


    def selectPage(
        self,
        notebook,
        moveFocus):
        print "SELECTPAGE", self, notebook, moveFocus
        return False


    def switchPage(
        self,
        notebook,
        page,
        pageNum):

        # This gets called when the user switches to a page, and while
        # we're adding them.

        print "SWITCHPAGE", self, notebook, page, pageNum


########################################################################
