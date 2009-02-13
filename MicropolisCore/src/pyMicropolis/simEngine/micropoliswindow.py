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
import micropolisutils
import micropolisnotebook
import micropolisstartpanel
import micropolisgaugeview
import micropolisnoticepanel
import micropolismessagespanel
import micropolisdrawingarea
import micropolisevaluationpanel
import micropolishistorypanel
import micropolisbudgetpanel
import micropolismappanel
import micropolisdisasterspanel
import micropoliscontrolpanel


########################################################################
# MicropolisPanelWindow

class MicropolisPanedWindow(gtk.Window):


    def __init__(
        self,
        engine=None,
        **args):

        gtk.Window.__init__(self, **args)

        self.connect('destroy', gtk.main_quit)
        self.connect('realize', self.handleRealize)
        self.connect('size-allocate', self.handleResize)

        self.set_title("Open Source Micropolis on Python / GTK / Cairo / Pango")

        self.firstResize = True

        self.engine = engine

        # Make the big map view.

        bigMapView = micropolisdrawingarea.EditableMicropolisDrawingArea(
                engine=self.engine)
        self.bigMapView = bigMapView
        engine.addView(bigMapView)

        # Make the small map view.

        smallMapView = micropolisdrawingarea.MiniMicropolisDrawingArea(
                engine=self.engine)
        self.smallMapView = smallMapView
        smallMapView.set_size_request(
            micropolisengine.WORLD_W,
            micropolisengine.WORLD_H)

        # Make the gauge view.

        gaugeView = micropolisgaugeview.MicropolisGaugeView(engine=self.engine)
        self.gaugeView = gaugeView

        # Make the vbox for the gauge and small map views.

        vbox1 = gtk.VBox(False, 0)
        self.vbox1 = vbox1

        # Make the notebooks.

        modeNotebook = gtk.Notebook()
        self.modeNotebook = modeNotebook
        modeNotebook.set_group_id(0)
        modeNotebook.set_show_tabs(False)

        startPanel = micropolisstartpanel.MicropolisStartPanel(
            engine=engine,
            target=self)
        self.startPanel = startPanel

        notebook1 = micropolisnotebook.MicropolisNotebook(target=self)
        self.notebook1 = notebook1

        notebook2 = micropolisnotebook.MicropolisNotebook(target=self)
        self.notebook2 = notebook2

        notebook3 = micropolisnotebook.MicropolisNotebook(target=self)
        self.notebook3 = notebook3

        # Make the panels in the notebooks.

        noticePanel = micropolisnoticepanel.MicropolisNoticePanel(
            engine=engine,
            centerOnTileHandler=self.centerOnTileHandler)
        self.noticePanel = noticePanel
        notebook1.addLabelTab('Notice', noticePanel)

        messagesPanel = micropolismessagespanel.MicropolisMessagesPanel(
            engine=engine)
        self.messagesPanel = messagesPanel
        notebook1.addLabelTab('Messages', messagesPanel)

        evaluationPanel = micropolisevaluationpanel.MicropolisEvaluationPanel(
            engine=engine)
        self.evaluationPanel = evaluationPanel
        notebook1.addLabelTab('Evaluation', evaluationPanel)

        historyPanel = micropolishistorypanel.MicropolisHistoryPanel(
            engine=engine)
        self.historyPanel = historyPanel
        notebook1.addLabelTab('History', historyPanel)

        budgetPanel = micropolisbudgetpanel.MicropolisBudgetPanel(
            engine=engine)
        self.budgetPanel = budgetPanel
        notebook1.addLabelTab('Budget', budgetPanel)

        mapPanel = micropolismappanel.MicropolisMapPanel(
            engine=engine,
            mapViews=[self.smallMapView, self.bigMapView,])
        self.mapPanel = mapPanel
        notebook1.addLabelTab('Map', mapPanel)

        disastersPanel = micropolisdisasterspanel.MicropolisDisastersPanel(
            engine=engine)
        self.disastersPanel = disastersPanel
        notebook1.addLabelTab('Disasters', disastersPanel)

        controlPanel = micropoliscontrolpanel.MicropolisControlPanel(
            engine=engine,
            target=self)
        self.controlPanel = controlPanel
        notebook1.addLabelTab('Control', controlPanel)

        # Panes

        vpaned1 = gtk.VPaned()
        self.vpaned1 = vpaned1

        vpaned2 = gtk.VPaned()
        self.vpaned2 = vpaned2

        hpaned1 = gtk.HPaned()
        self.hpaned1 = hpaned1

        hpaned2 = gtk.HPaned()
        self.hpaned2 = hpaned2

        # Pack the views into the panes.

        vbox1.pack_start(gaugeView, False, False, 0)
        vbox1.pack_start(smallMapView, True, True, 0)

        hpaned1.pack1(vbox1, resize=False, shrink=False)
        hpaned1.pack2(notebook1, resize=False, shrink=False)

        hpaned2.pack1(notebook2, resize=False, shrink=False)
        hpaned2.pack2(notebook3, resize=False, shrink=False)

        vpaned1.pack1(hpaned1, resize=False, shrink=False)
        vpaned1.pack2(vpaned2, resize=False, shrink=False)

        vpaned2.pack1(bigMapView, resize=False, shrink=False)
        vpaned2.pack2(hpaned2, resize=False, shrink=False)

        modeNotebook.append_page(startPanel)

        modeNotebook.append_page(vpaned1)

        # Put the top level mode notebook in this window.

        self.add(modeNotebook)

        # Load a city file.

        self.startGame()


    def startGame(self):

        engine = self.engine

        if False:
            cityFileName = 'cities/haight.cty'
            #cityFileName = 'cities/yokohama.cty'
            print "Loading city file:", cityFileName
            engine.loadFile(cityFileName)
        else:
            self.generateCity()

        # Initialize the simulator engine.

        engine.setSpeed(2)
        engine.setCityTax(9)
        engine.setEnableDisasters(False)
        engine.setFunds(1000000000)

        self.startMode()

        self.resize(800, 600)


    def startScenario(self, id):
        print "STARTSCENARIO", id
        engine = self.engine
        engine.loadScenario(id)


    def loadCityDialog(self):
        print "LOAD CITY DIALOG"

        dialog = gtk.FileChooserDialog(
            title='Select a city to load.',
            action=gtk.FILE_CHOOSER_ACTION_OPEN,
            buttons=(
                gtk.STOCK_CANCEL,
                gtk.RESPONSE_CANCEL,
                gtk.STOCK_OPEN,
                gtk.RESPONSE_OK,
            ))
       
        filter = gtk.FileFilter()
        filter.set_name("Micropolis Cities")
        filter.add_pattern("*.cty")
        dialog.add_filter(filter)

        citiesFolder = 'cities'
        dialog.set_current_folder(citiesFolder)

        response = dialog.run()
        if response == gtk.RESPONSE_OK:
            fileName = dialog.get_filename()
            print "FILENAME", fileName
            result = self.engine.loadFile(fileName)
            print "RESULT", result
        elif response == gtk.RESPONSE_CANCEL:
            print 'Closed, no files selected'
        dialog.destroy()


    def generateCity(self):
        print "GENERATECITY"
        self.engine.generateNewCity()


    def playCity(self):
        print "PLAYCITY"
        self.engine.setSpeed(2)
        self.engine.setPasses(1)
        self.engine.resume()
        self.playMode()


    def startMode(self):
        print "STARTMODE"
        engine = self.engine
        mediumMapView = self.startPanel.mediumMapView
        smallMapView = self.smallMapView
        bigMapView = self.bigMapView
        smallMapView = self.smallMapView

        engine.removeView(smallMapView)
        engine.removeView(bigMapView)
        engine.addView(mediumMapView)
        mediumMapView.updateView()
        engine.pause()
        self.modeNotebook.set_current_page(0)


    def playMode(self):
        engine = self.engine
        mediumMapView = self.startPanel.mediumMapView
        smallMapView = self.smallMapView
        bigMapView = self.bigMapView
        smallMapView = self.smallMapView

        engine.addView(smallMapView)
        engine.addView(bigMapView)
        engine.removeView(mediumMapView)
        smallMapView.updateView()
        bigMapView.updateView()
        engine.resume()
        self.modeNotebook.set_current_page(1)


    def aboutDialog(self):
        print "ABOUT DIALOG"

        def handleEmail(dialog, link, data):
            print "HANDLE EMAIL", dialog, link, data

        def handleUrl(dialog, link, data):
            print "HANDLE EMAIL", dialog, link, data

        engine = self.engine
        dialog = gtk.AboutDialog()
        dialog.set_name('Micropolis')
        dialog.set_version(engine.getMicropolisVersion())
        dialog.set_copyright('Copyright (C) 2009')
        dialog.set_comments('Developed by the EduVerse project')
        dialog.set_license('GPLv3')
        #dialog.set_wrap_license(???)
        dialog.set_website('http://www.MicropolisOnline.com')
        dialog.set_website_label('Micropolis Online')
        dialog.set_authors(('Will Wright', 'Fred Haslam', 'Don Hopkins', '[AUTHORS...]',))
        dialog.set_documenters(('[DOCUMENTERS...]',))
        dialog.set_artists(('[ARTISTS...]',))
        dialog.set_translator_credits('[TRANSLATORS...]')
        #dialog.set_logo(pixbuf)
        dialog.set_program_name('Micropolis')
        #dialog.set_email_hook(handleEmail, None)
        #dialog.set_url_hook(handleUrl, None)
        response = dialog.run()
        dialog.destroy()


    def saveCityDialog(self):
        # @todo "Save city" dialog.
        print "SAVE CITY DIALOG"


    def saveCityAsDialog(self):
        # @todo "Save city as..." dialog.
        print "SAVE CITY AS DIALOG"


    def newCityDialog(self):
        # @todo "Are you sure you want to start a new game?" dialog.
        print "NEW CITY DIALOG"
        self.startGame()


    def quitDialog(self):
        # @todo "Are you sure you want to quit?" dialog.
        print "QUIT DIALOG"


    def centerOnTileHandler(
        self,
        tileX,
        tileY):

        #print "CENTERONTILEHANDLER", self, tileX, tileY

        bigMapView = self.bigMapView
        bigMapView.setScale(1.0)
        bigMapView.centerOnTile(
            tileX,
            tileY)


    def resizeEdges(
        self):

        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        print "WINDOW SIZE", winWidth, winHeight

        extra = 4
        padding = 14

        leftEdge = 120
        topEdge = 120

        self.vpaned1.set_position(topEdge + extra)
        self.vpaned2.set_position(1000)
        self.hpaned1.set_position(150)
        self.hpaned2.set_position(1000)

        bigMapView = self.bigMapView
        bigMapView.panTo(-200, -200)
        bigMapView.setScale(1.0)


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


    def createNotebookWindowNotebook(
        otherNotebook, notebook, page, x, y):

        print "createWindowNotebook", otherNotebook, notebook, page, x, y

        parent = page.get_parent()
        print "parent", parent, parent == self.target.notebook1, parent == self.target.notebook2, parent == self.target.notebook3

        for n in (self.notebook1, self.notebook2, self.notebook3):
            print n

        if parent == self.target.notebook1:
            return self.target.notebook2
        else:
            return self.target.notebook1


########################################################################
