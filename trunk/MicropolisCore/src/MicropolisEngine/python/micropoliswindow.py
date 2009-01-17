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


import micropolisengine
import micropolismodel
import micropolisutils
import micropolispiemenus
import micropolisdrawingarea
import micropolisgraphview
import micropolisevaluationview
import micropolisstatusview
import micropolisnoticeview


########################################################################
# MicropolisWindow

class MicropolisWindow(gtk.Window):


    def __init__(
        self,
        engine=None,
        tileViewClass=micropolisdrawingarea.EditableMicropolisDrawingArea,
        **args):
        
        gtk.Window.__init__(self, **args)

        self.connect('destroy', gtk.main_quit)

        self.set_title("OLPC Micropolis for Python/Cairo/Pango, by Don Hopkins")

        self.engine = engine

        self.da = \
            tileViewClass(
                engine=self.engine)

        self.add(self.da)
        engine.addView(self.da)


########################################################################
# MicropolisPanelWindow

class MicropolisPanedWindow(gtk.Window):


    def __init__(
        self,
        engine=None,
        tileViewClass=micropolisdrawingarea.EditableMicropolisDrawingArea,
        evaluationViewClass=micropolisevaluationview.MicropolisEvaluationView,
        graphViewClass=micropolisgraphview.MicropolisGraphView,
        statusViewClass=micropolisstatusview.MicropolisStatusView,
        noticeViewClass=micropolisnoticeview.MicropolisNoticeView,
        **args):
        
        gtk.Window.__init__(self, **args)

        self.connect('destroy', gtk.main_quit)
        self.connect('realize', self.handleRealize)
        self.connect('size-allocate', self.handleResize)

        self.set_title("OLPC Micropolis for Python/Cairo/Pango, by Don Hopkins")

        self.firstResize = True

        self.engine = engine

        # Views

        self.tileView = \
            tileViewClass(
                engine=self.engine)

        self.evaluationView = \
            evaluationViewClass(
                engine=engine)

        self.graphView = \
            graphViewClass(
                engine=self.tileView.engine)

        self.statusView = \
            statusViewClass(
                engine=engine)
        self.noticeView = \
            noticeViewClass(
                engine=engine)

        # Frames

        self.frameCenter = gtk.Frame()
        self.frameCenter.set_shadow_type(gtk.SHADOW_IN)
        self.frameCenter.add(self.tileView)
        engine.addView(self.tileView)

        self.frameTop = gtk.Frame()
        self.frameTop.set_shadow_type(gtk.SHADOW_IN)
        self.frameTop.add(self.evaluationView)
        engine.addEvaluation(self.evaluationView)

        self.frameBottom = gtk.Frame()
        self.frameBottom.set_shadow_type(gtk.SHADOW_IN)
        self.frameBottom.add(self.graphView)
        engine.addGraph(self.graphView)

        self.frameLeft = gtk.Frame()
        self.frameLeft.set_shadow_type(gtk.SHADOW_IN)
        self.frameLeft.add(self.statusView)

        self.frameRight = gtk.Frame()
        self.frameRight.set_shadow_type(gtk.SHADOW_IN)
        self.frameRight.add(self.noticeView)

        # Panes

        self.vpaned1 = gtk.VPaned()
        self.vpaned2 = gtk.VPaned()
        self.hpaned1 = gtk.HPaned()
        self.hpaned2 = gtk.HPaned()

        # Put the top level pane in this window.

        self.add(
            self.vpaned1)

        # Nest the vertical and horizontal panes into a tree.

        self.vpaned1.pack2(
            self.vpaned2,
            resize=False,
            shrink=False)

        self.vpaned2.pack1(
            self.hpaned1,
            resize=False,
            shrink=False)

        self.hpaned1.pack2(
            self.hpaned2,
            resize=False,
            shrink=False)

        # Populate the leaves of the tree with the view and control panels.

        self.hpaned2.pack1(
            self.frameCenter,
            resize=False,
            shrink=False)

        self.vpaned1.pack1(
            self.frameTop,
            resize=False,
            shrink=False)

        self.vpaned2.pack2(
            self.frameBottom,
            resize=False,
            shrink=False)

        self.hpaned1.pack1(
            self.frameLeft,
            resize=False,
            shrink=False)

        self.hpaned2.pack2(
            self.frameRight,
            resize=False,
            shrink=False)


    def resizeEdges(
        self):

        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        print "WINDOW SIZE", winWidth, winHeight

        extra = 4
        padding = 14

        leftEdge = 16
        rightEdge = 16
        topEdge = 150
        bottomEdge = 150

        self.hpaned1.set_position(leftEdge + extra)
        self.hpaned2.set_position(winWidth - (extra + leftEdge + padding + rightEdge))

        self.vpaned1.set_position(topEdge + extra)
        self.vpaned2.set_position(winHeight - (extra + topEdge + padding + bottomEdge))

        self.tileView.panTo(-100, -100)


    def handleRealize(
        self,
        *args):

        #print "handleRealize MicropolisPanedWindow", self, "ARGS", args

        self.firstResize = True


    def handleResize(
        self,
        widget,
        event,
        *args):

        #print "handleResize MicropolisPanedWindow", self, "WIDGET", widget, "EVENT", event, "ARGS", args

        if self.firstResize:
            self.firstResize = False
            self.resizeEdges()


########################################################################
