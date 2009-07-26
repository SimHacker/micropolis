# micropolisgenericengine.py
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
# Micropolis Generic Engine
# Don Hopkins


########################################################################
# Import stuff


import sys
import os
import time
import random
import micropolisengine
import gobject
import cairo
from xmlutilities import *
from pyMicropolis.tileEngine import tileengine
from micropolisengine import WORLD_W, WORLD_H
from micropolisengine import WORLD_W_2, WORLD_H_2
from micropolisengine import WORLD_W_4, WORLD_H_4
from micropolisengine import WORLD_W_8, WORLD_H_8
import xml.dom.minidom
from xml.dom.minidom import Node
import pprint
from StringIO import StringIO


########################################################################
# Globals


__version__ = "0.9"


########################################################################
# MicropolisGenericEngine Class


class MicropolisGenericEngine(micropolisengine.Micropolis):


    dataColorMap = \
        cairo.ImageSurface.create_from_png(
            'images/micropolisEngine/dataColorMap.png')

    rateColorMap = \
        cairo.ImageSurface.create_from_png(
            'images/micropolisEngine/rateColorMap.png')

    powerGridColorMap = \
        cairo.ImageSurface.create_from_png(
            'images/micropolisEngine/powerGridColorMap.png')

    terrainColorMap = \
        cairo.ImageSurface.create_from_png(
            'images/micropolisEngine/terrainColorMap.png')


    def __init__(
            self,
            running=False,
            timeDelay=10,
            *args,
            **kwargs):
        #print "MicropolisGenericEngine.__init__", self, "calling micropolisengine.Micropolis.__init__", micropolisengine.Micropolis.__init__, args, kwargs

        micropolisengine.Micropolis.__init__(self, *args, **kwargs)

        self.resourceDir = 'res'
        self.running = running
        self.timeDelay = timeDelay
        self.timerActive = False
        self.timerId = None
        self.views = []
        self.interests = {}
        self.residentialImage = None
        self.commercialImage = None
        self.industrialImage = None
        self.transportationImage = None
        self.populationDensityImage = None
        self.rateOfGrowthImage = None
        self.landValueImage = None
        self.crimeRateImage = None
        self.pollutionDensityImage = None
        self.trafficDensityImage = None
        self.powerGridImage = None
        self.fireCoverageImage = None
        self.policeCoverageImage = None
        self.dataTileEngine = tileengine.TileEngine()
        self.robots = []
        self.robotDict = {}
        self.saveFileDir = None
        self.saveFileName = None
        self.metaFileName = None
        self.title = ''
        self.description = ''
        self.readOnly = False
        self.gameMode = 'stopped'


        # NOTE: Because of a bug in SWIG, printing out the wrapped
        # objects results in a crash.  So don't do that! I hope this
        # bug in SWIG gets fixed.
        # TODO: Report SWIG bug, if it's not already known or fixed.

        # Hook the engine up so it has a handle on its Python object side.
        self.userData = micropolisengine.getPythonCallbackData(self)
        #print "USERDATA"#, self.userData

        # Hook up the language independent callback mechanism to our
        # low level C++ Python dependent callback handler.

        self.callbackHook = micropolisengine.getPythonCallbackHook()
        #print "CALLBACKHOOK"#, self.callbackHook

        # Hook up the Python side of the callback handler, defined in
        # our scripted subclass of the SWIG wrapper.

        self._invokeCallback = self.invokeCallback # Cache to prevent GC
        self.callbackData = \
            micropolisengine.getPythonCallbackData(
                self._invokeCallback)
        #print "CALLBACKDATA"#, self.callbackData

        self.expressInterest(
            self,
            ('paused',))

        self.initGame()
        self.initGamePython()

        if self.running:
            self.startTimer()

        #print "MicropolisEngine.__init__ done", self


    def initGamePython(self):
        pass


    def __del__(
        self):

        self.stopTimer()

        micropolisengine.Micropolis.__del__(self)

        # TODO: Clean up everything.


    def formatMoney(
        self,
        money):
        return '$' + self.formatNumber(money)


    def formatPercent(
        self,
        percent):
        return str(int(0.5 + (percent * 100))) + "%"


    def formatDelta(
        self,
        delta):
        if delta < 0:
            return self.formatNumber(delta)
        else:
            return '+' + self.formatNumber(delta)


    def formatNumber(
        self,
        number):
        negative = number < 0
        if negative:
            number = -number
        s = str(number)
        if len(s) > 9:
            s = s[-12:-9] + ',' + s[-9:-6] + ',' + s[-6:-3] + ',' + s[-3:]
        elif len(s) > 6:
            s = s[:-6] + ',' + s[-6:-3] + ',' + s[-3:]
        elif len(s) > 3:
            s = s[:-3] + ',' + s[-3:]
        if negative:
            s = '-' + s
        return s


    def addView(self, view):
        views = self.views
        if view not in views:
            self.views.append(view)
            print "ADDVIEW", view


    def removeView(self, view):
        views = self.views
        if view in views:
            views.remove(view)
            print "REMOVEVIEW", view


    def getMapImage(
        self,
        width=micropolisengine.WORLD_W,
        height=micropolisengine.WORLD_H,
        overlay='all'):

        #print "getMapImage"

        tileSize = micropolisengine.EDITOR_TILE_SIZE

        worldW = micropolisengine.WORLD_W
        worldH = micropolisengine.WORLD_H

        tileWidth = int(width / worldW)
        tileHeight = int(height / worldH)

        #print width, height, tileSize, width % tileWidth, height % tileHeight, tileWidth != tileHeight

        if (# Size must not be zero or negative.
            (width < worldW) or
            (height < worldH) or
            # Size must not be bigger than 16x16 tile.
            (width > worldW * tileSize) or
            (height > worldH * tileSize) or
            # Size must be multiple of tile size.
            ((width % tileWidth) != 0) or
            ((height % tileHeight) != 0) or
            # Size must be 1:1 aspect ratio.
            (tileWidth != tileHeight)):
            print "MicropolisGenericEngine getMapImage invalid size", width, height
            return None

        tileSize = tileWidth

        surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
        ctx = cairo.Context(surface)

        alpha = 1.0

        self.renderTiles(
            ctx,
            tileSize,
            0,
            0,
            worldW,
            worldH,
            alpha)

        overlayImage, overlayAlpha, overlayWidth, overlayHeight = \
            self.getDataImageAlphaSize(overlay)
        #print "OVERLAY", overlay, "IMAGE", overlayImage, overlayAlpha, overlayWidth, overlayHeight
        if overlayImage:
            overlayWidth = 1.0 / overlayWidth
            overlayHeight = 1.0 / overlayHeight

            ctx.save()

            ctx.scale(
                worldW * tileSize,
                worldH * tileSize)

            ctx.rectangle(0, 0, 1, 1)
            ctx.clip()

            imageWidth = overlayImage.get_width()
            imageHeight = overlayImage.get_height()

            ctx.scale(
                overlayWidth / imageWidth,
                overlayHeight / imageHeight)

            ctx.set_source_surface(
                overlayImage,
                0,
                0)
            ctx.paint_with_alpha(overlayAlpha)

            ctx.restore()

        return surface


    def getDataImageAlphaSize(self, name):
        # @todo: cache images
        if name == 'all':
            return self.getAllImageAlphaSize()
        elif name == 'residential':
            return self.getResidentialImageAlphaSize()
        elif name == 'commercial':
            return self.getCommercialImageAlphaSize()
        elif name == 'industrial':
            return self.getIndustrialImageAlphaSize()
        elif name == 'transportation':
            return self.getTransportationImageAlphaSize()
        elif name == 'populationdensity':
            return self.getPopulationDensityImageAlphaSize()
        elif name == 'rateofgrowth':
            return self.getRateOfGrowthImageAlphaSize()
        elif name == 'landvalue':
            return self.getLandValueImageAlphaSize()
        elif name == 'crimerate':
            return self.getCrimeRateImageAlphaSize()
        elif name == 'pollutiondensity':
            return self.getPollutionDensityImageAlphaSize()
        elif name == 'trafficdensity':
            return self.getTrafficDensityImageAlphaSize()
        elif name == 'powergrid':
            return self.getPowerGridImageAlphaSize()
        elif name == 'firecoverage':
            return self.getFireCoverageImageAlphaSize()
        elif name == 'policecoverage':
            return self.getPoliceCoverageImageAlphaSize()
        else:
            print "MicropolisGenericEngine: getImageAlphaSize: Invalid data image name:", name
            return None, 0.0, 0.0, 0.0


    def getAllImageAlphaSize(self):
        return None, 1.0, 1.0, 1.0


    def getResidentialImageAlphaSize(self):
        image = self.residentialImage
        if not image:
            image = self.makeImage(WORLD_W, WORLD_H)
            self.residentialImage = image
        tengine = self.dataTileEngine

        buffer = self.getMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W
        tengine.height = WORLD_H

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = micropolisengine.BYTES_PER_TILE * micropolisengine.WORLD_H
        tengine.rowBytes = micropolisengine.BYTES_PER_TILE

        from micropolisengine import \
            ZONEBIT, PWRBIT, CONDBIT, ALLBITS, LOMASK, RIVER, \
            COMBASE

        transparent = 0
        land = 1
        water = 2

        def tileFunction(col, row, tile):
            tileMasked = tile & LOMASK
            if tileMasked == RIVER:
                result = water
            elif tileMasked >= COMBASE:
                result = land
            else:
                result = transparent
            return result

        tengine.renderPixels(
            image,
            self.terrainColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W,
            WORLD_H)

        return image, 0.8, 1.0, 1.0


    def getCommercialImageAlphaSize(self):
        image = self.commercialImage
        if not image:
            image = self.makeImage(WORLD_W, WORLD_H)
            self.commercialImage = image
        tengine = self.dataTileEngine

        buffer = self.getMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W
        tengine.height = WORLD_H

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = micropolisengine.BYTES_PER_TILE * micropolisengine.WORLD_H
        tengine.rowBytes = micropolisengine.BYTES_PER_TILE

        from micropolisengine import \
            ZONEBIT, PWRBIT, CONDBIT, ALLBITS, LOMASK, RIVER, \
            COMBASE, COMLAST, LVRAIL6

        transparent = 0
        land = 1
        water = 2

        def tileFunction(col, row, tile):
            tileMasked = tile & LOMASK
            if tileMasked == RIVER:
                result = water
            elif ((tileMasked > COMLAST) or
                  ((tileMasked >= LVRAIL6) and
                   (tileMasked < COMBASE))):
                result = land
            else:
                result = transparent
            return result

        tengine.renderPixels(
            image,
            self.terrainColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W,
            WORLD_H)

        return image, 0.8, 1.0, 1.0


    def getIndustrialImageAlphaSize(self):
        image = self.industrialImage
        if not image:
            image = self.makeImage(WORLD_W, WORLD_H)
            self.industrialImage = image
        tengine = self.dataTileEngine

        buffer = self.getMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W
        tengine.height = WORLD_H

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = micropolisengine.BYTES_PER_TILE * micropolisengine.WORLD_H
        tengine.rowBytes = micropolisengine.BYTES_PER_TILE

        from micropolisengine import \
            ZONEBIT, PWRBIT, CONDBIT, ALLBITS, LOMASK, RIVER, \
            RESBASE, INDBASE, PORTBASE, SMOKEBASE, TINYEXP, \
            TINYEXPLAST, FOOTBALLGAME1

        transparent = 0
        land = 1
        water = 2

        def tileFunction(col, row, tile):
            tileMasked = tile & LOMASK
            if tileMasked == RIVER:
                result = water
            elif (((tileMasked >= RESBASE) and (tileMasked < INDBASE)) or
                  ((tileMasked >= PORTBASE) and (tileMasked < SMOKEBASE)) or
                  ((tileMasked >= TINYEXP) and (tileMasked <= TINYEXPLAST)) or
                  (tileMasked >= FOOTBALLGAME1)):
                result = land
            else:
                result = transparent
            return result

        tengine.renderPixels(
            image,
            self.terrainColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W,
            WORLD_H)

        return image, 0.8, 1.0, 1.0


    def getTransportationImageAlphaSize(self):
        image = self.commercialImage
        if not image:
            image = self.makeImage(WORLD_W, WORLD_H)
            self.commercialImage = image
        tengine = self.dataTileEngine

        buffer = self.getMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W
        tengine.height = WORLD_H

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = micropolisengine.BYTES_PER_TILE * micropolisengine.WORLD_H
        tengine.rowBytes = micropolisengine.BYTES_PER_TILE

        from micropolisengine import \
            ZONEBIT, PWRBIT, CONDBIT, ALLBITS, LOMASK, RIVER, \
            RESBASE, BRWXXX7, LVPOWER10, UNUSED_TRASH6

        transparent = 0
        land = 1
        water = 2

        # FIXME: Exclude the smokestack animations used by the coal power plant.

        def tileFunction(col, row, tile):
            tileMasked = tile & LOMASK
            if tileMasked == RIVER:
                result = water
            elif ((tileMasked >= RESBASE) or
                  ((tileMasked >= BRWXXX7) and (tileMasked <= LVPOWER10)) or
                  (tileMasked == UNUSED_TRASH6)):
                result = land
            else:
                result = transparent
            return result

        tengine.renderPixels(
            image,
            self.terrainColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W,
            WORLD_H)

        return image, 0.8, 1.0, 1.0


    def getPopulationDensityImageAlphaSize(self):
        image = self.populationDensityImage
        if not image:
            image = self.makeImage(WORLD_W_2, WORLD_H_2)
            self.populationDensityImage = image
        tengine = self.dataTileEngine

        buffer = self.getPopulationDensityMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_2
        tengine.height = WORLD_H_2

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_BYTE_UNSIGNED
        tengine.colBytes = WORLD_H_2
        tengine.rowBytes = 1

        tengine.renderPixels(
            image,
            self.dataColorMap,
            None,
            None,
            0,
            0,
            WORLD_W_2,
            WORLD_H_2)

        return image, 0.5, 1.0, 1.0


    def getRateOfGrowthImageAlphaSize(self):
        image = self.rateOfGrowthImage
        if not image:
            image = self.makeImage(WORLD_W_8, WORLD_H_8)
            self.rateOfGrowthImage = image
        tengine = self.dataTileEngine

        buffer = self.getRateOfGrowthMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_8
        tengine.height = WORLD_H_8

        # Signed short tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_SIGNED
        tengine.colBytes = WORLD_H_8 * 2
        tengine.rowBytes = 2

        def tileFunction(col, row, tile):
            return max(0, min(int(((tile * 256) / 200) + 128), 255))

        tengine.renderPixels(
            image,
            self.rateColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W_8,
            WORLD_H_8)

        ratio = (float(WORLD_H) / 8.0) / float(WORLD_H_8)
        return image, 0.5, 1.0, ratio


    def getLandValueImageAlphaSize(self):
        image = self.landValueImage
        if not image:
            image = self.makeImage(WORLD_W_2, WORLD_H_2)
            self.landValueImage = image
        tengine = self.dataTileEngine

        buffer = self.getLandValueMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_2
        tengine.height = WORLD_H_2

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_BYTE_UNSIGNED
        tengine.colBytes = WORLD_H_2
        tengine.rowBytes = 1

        tengine.renderPixels(
            image,
            self.dataColorMap,
            None,
            None,
            0,
            0,
            WORLD_W_2,
            WORLD_H_2)

        return image, 0.5, 1.0, 1.0


    def getCrimeRateImageAlphaSize(self):
        image = self.crimeRateImage
        if not image:
            image = self.makeImage(WORLD_W_2, WORLD_H_2)
            self.crimeRateImage = image
        tengine = self.dataTileEngine

        buffer = self.getCrimeRateMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_2
        tengine.height = WORLD_H_2

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_BYTE_UNSIGNED
        tengine.colBytes = WORLD_H_2
        tengine.rowBytes = 1

        tengine.renderPixels(
            image,
            self.dataColorMap,
            None,
            None,
            0,
            0,
            WORLD_W_2,
            WORLD_H_2)

        return image, 0.5, 1.0, 1.0


    def getPollutionDensityImageAlphaSize(self):
        image = self.pollutionDensityImage
        if not image:
            image = self.makeImage(WORLD_W_2, WORLD_H_2)
            self.pollutionDensityImage = image
        tengine = self.dataTileEngine

        buffer = self.getPollutionDensityMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_2
        tengine.height = WORLD_H_2

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_BYTE_UNSIGNED
        tengine.colBytes = WORLD_H_2
        tengine.rowBytes = 1

        tengine.renderPixels(
            image,
            self.dataColorMap,
            None,
            None,
            0,
            0,
            WORLD_W_2,
            WORLD_H_2)

        return image, 0.5, 1.0, 1.0


    def getTrafficDensityImageAlphaSize(self):
        image = self.trafficDensityImage
        if not image:
            image = self.makeImage(WORLD_W_2, WORLD_H_2)
            self.trafficDensityImage = image
        tengine = self.dataTileEngine

        buffer = self.getTrafficDensityMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_2
        tengine.height = WORLD_H_2

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_BYTE_UNSIGNED
        tengine.colBytes = WORLD_H_2
        tengine.rowBytes = 1

        tengine.renderPixels(
            image,
            self.dataColorMap,
            None,
            None,
            0,
            0,
            WORLD_W_2,
            WORLD_H_2)

        return image, 0.5, 1.0, 1.0


    def getPowerGridImageAlphaSize(self):
        image = self.powerGridImage
        if not image:
            image = self.makeImage(WORLD_W, WORLD_H)
            self.powerGridImage = image
        tengine = self.dataTileEngine

        buffer = self.getPowerGridMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W
        tengine.height = WORLD_H

        # Unsigned byte tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_BYTE_UNSIGNED
        tengine.colBytes = WORLD_H
        tengine.rowBytes = 1

        from micropolisengine import \
            ZONEBIT, PWRBIT, CONDBIT, ALLBITS, LOMASK, LASTFIRE

        transparent = 0
        unpowered = 1
        powered = 2
        conductive = 3
        getTile = self.getTile

        def tileFunction(col, row, tile):
            t = getTile(col, row)
            tileMasked = t & LOMASK
            if tileMasked < LASTFIRE:
                result = transparent
            elif t & ZONEBIT:
                if t & PWRBIT:
                    result = powered
                else:
                    result = unpowered
            else:
                if t & CONDBIT:
                    result = conductive
                else:
                    result = transparent
            return result

        tengine.renderPixels(
            image,
            self.powerGridColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W,
            WORLD_H)

        return image, 0.85, 1.0, 1.0


    def getFireCoverageImageAlphaSize(self):
        image = self.fireCoverageImage
        if not image:
            image = self.makeImage(WORLD_W_8, WORLD_H_8)
            self.fireCoverageImage = image
        tengine = self.dataTileEngine

        buffer = self.getFireCoverageMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_8
        tengine.height = WORLD_H_8

        # Unsigned short tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = WORLD_H_8 * 2
        tengine.rowBytes = 2

        def tileFunction(col, row, tile):
            return max(0, min(tile, 255))

        tengine.renderPixels(
            image,
            self.dataColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W_8,
            WORLD_H_8)

        ratio = (float(WORLD_H) / 8.0) / float(WORLD_H_8)
        return image, 0.5, 1.0, ratio


    def getPoliceCoverageImageAlphaSize(self):
        image = self.policeCoverageImage
        if not image:
            image = self.makeImage(WORLD_W_8, WORLD_H_8)
            self.policeCoverageImage = image
        tengine = self.dataTileEngine

        buffer = self.getPoliceCoverageMapBuffer()
        #print "Map buffer", buffer
        tengine.setBuffer(buffer)
        tengine.width = WORLD_W_8
        tengine.height = WORLD_H_8

        # Unsigned short tile values, in column major order.
        tengine.tileFormat = tileengine.TILE_FORMAT_SHORT_UNSIGNED
        tengine.colBytes = WORLD_H_8 * 2
        tengine.rowBytes = 2

        def tileFunction(col, row, tile):
            return max(0, min(tile, 255))

        tengine.renderPixels(
            image,
            self.dataColorMap,
            tileFunction,
            None,
            0,
            0,
            WORLD_W_8,
            WORLD_H_8)

        ratio = (float(WORLD_H) / 8.0) / float(WORLD_H_8)
        return image, 0.5, 1.0, ratio


    def makeImage(self, width, height):
        return self.dataColorMap.create_similar(
            cairo.CONTENT_COLOR_ALPHA, width, height)


    def startTimer(
        self):

        pass # Override in subclasses.


    def stopTimer(
        self):

        pass # Override in subclasses.


    def update(
        self,
        name,
        *args):

        #print "MicropolisEngine update", self, name, args

        if name == 'paused':
            self.running = not self.simPaused
            print "PAUSED", self.simPaused, "running", self.running
            if self.running:
                self.startTimer()
            else:
                self.stopTimer()


    def sendUpdate(self, name, *args):
        #print "SENDUPDATE", name, args
        interests = self.interests
        a = interests.get(name)
        if a:
            for view in a:
                view.update(name, *args)


    def expressInterest(self, view, names):
        interests = self.interests
        for name in names:
            a = interests.get(name, None) or []
            if not a:
                interests[name] = a
            if view not in a:
                a.append(view)


    def revokeInterest(self, view, names):
        interests = self.interests
        for name in names:
            a = interests.get(name, None)
            if a and (view in a):
                a.remove(view)


    def loadMetaCity(self, metaFilePath):

        doc = xml.dom.minidom.parse(metaFilePath)
        el = doc.firstChild
        if el.nodeName != 'metaCity':
            raise Exception("Expected top level 'metaCity' element in meta city file")

        saveFileDir, metaFileName = os.path.split(os.path.abspath(metaFilePath))
        saveFileName = GetSubElementText(el, u'saveFileName', None)
        title = GetSubElementText(el, u'title', '')
        description = GetSubElementText(el, u'description', '')
        readOnly = GetSubElementBool(el, u'readOnly', False)

        self.saveFileDir = saveFileDir
        self.metaFileName = metaFileName
        self.saveFileName = saveFileName
        self.title = title
        self.description = description
        self.readOnly = readOnly

        saveFilePath = os.path.join(saveFileDir, saveFileName)

        saveFilePath = saveFilePath.encode('utf8')
        #print "Loading city file:", saveFilePath
        success = self.loadCity(saveFilePath)

        if not success:
            raise Exception('Error loading city file')

        self.sendUpdate('load')


    def saveMetaCity(self, metaFileName=None):

        saveFileDir = self.saveFileDir
        metaFileName = metaFileName or self.metaFileName
        saveFileName = self.saveFileName
        readOnly = self.readOnly

        if not metaFileName:
            raise Exception('Undefined metaFileName')

        if readOnly or not saveFileDir:
            readOnly = False
            self.readOnly = readOnly
            saveFileDir = os.path.expanduser('~/cities')
            self.saveFileDir = saveFileDir
            os.makedirs(saveFileDir)

        baseName, ext = os.path.splitext(metaFileName)

        saveFileName = baseName + '.cty'
        self.saveFileName = saveFileName

        saveFilePath = os.path.join(saveFileDir, saveFileName)

        success = self.saveCityAs(saveFilePath)

        if not success:
            raise('Error writing to city file')

        xmlText = self.getMetaData(saveFileName)

        metaFilePath = os.path.join(saveFileDir, metaFileName)

        f = open(metaFilePath, 'wb')
        print xmlText
        f.write(xmlText)
        f.close()


    def getMetaData(self):
        doc = xml.dom.minidom.Document()
        el = doc.createElement(u'metaCity')
        doc.appendChild(el)

        SetSubElementText(el, u'title', self.title)
        SetSubElementText(el, u'description', self.description)
        SetSubElementBool(el, u'readOnly', self.readOnly)

        return doc.toxml()


    def loadMetaScenario(self, id):
        if ((id <= micropolisengine.SC_NONE) or
            (id >= micropolisengine.SC_COUNT)):
            print "loadMetaScenario: Invalid scenario id:", id
            return

        scenario = self.scenarios[id]
        self.title = scenario['title']
        self.description = scenario['description']
        self.loadScenario(id)
        self.sendUpdate('load')

        self.sendUpdate('message', id, -1, -1, True, True)

    def generateNewMetaCity(self):
        self.title = 'New City'
        self.description = 'A randomly generated city.'
        self.generateMap()
        self.sendUpdate('load')


    def setGameMode(self, gameMode):
        self.gameMode = gameMode
        self.sendUpdate('gameMode')


    def addRobot(self, robot):
        #print "ADDROBOT", robot

        self.removeRobot(robot)

        robots = self.robots
        robots.append(robot)

        robotDict = self.robotDict
        robotDict[robot.robotID] = robot

        robot.engine = self
        #print "*** SET ENGINE", self, robot


    def removeRobot(self, robot):
        #print "REMOVEROBOT", robot

        robots = self.robots
        if robot in robots:
            robots.remove(robot)

        robotDict = self.robotDict
        robotID = robot.robotID
        if robotID in robotDict:
            del robotDict[robotID]

        robot.engine = None


    def getRobot(self, robotID):
        return self.robotDict.get(robotID, None)


    def clearRobots(self):
        for robot in list(self.robots):
            self.removeRobot(robot)


    def simRobots(self):
        #print "SIMROBOTS"
        for robot in self.robots:
            #print "SIMROBOT LOOP", robot
            robot.simulate()
            #print "SIMULATED ROBOT", robot


    def __repr__(self):
        return "<MicropolisGenericEngine>"


    def invokeCallback(self, micropolis, name, *args):
        # In this case, micropolis is the same as self, so ignore it.
        handler = getattr(self, 'handle_' + name, None)
        if handler:
            #print "Calling handler", handler, args
            handler(*args)
            #print "Called handler."
        else:
            print "No handler for", name


    def handle_autoGoto(self, x, y):
        print "handle_autoGoto(self, x, y)", (self, x, y)


    def handle_didGenerateMap(self):
        print "handle_didGenerateMap(self)", (self,)


    def handle_didLoadCity(self):
        print "handle_didLoadCity(self)", (self,)


    def handle_didLoadScenario(self):
        print "handle_didLoadScenario(self)", (self,)


    def handle_didSaveCity(self):
        print "handle_didSaveCity(self)", (self,)


    def handle_didTool(self, name, x, y):
        print "handle_didTool(self, name, x, y)", (self, name, x, y)


    def handle_didntLoadCity(self, msg):
        print "handle_didntLoadCity(self, msg)", (self, msg)


    def handle_didntSaveCity(self, msg):
        print "handle_didntSaveCity(self, msg)", (self, msg)


    def handle_playNewCity(self):
        print "handle_playNewCity(self)", (self,)


    def handle_makeSound(self, channel, sound, x, y):
        #print "handle_makeSound(self, channel, sound, x, y)", (self, channel, sound, x, y)
        pass # print "SOUND", channel, sound, x, y


    def handle_newGame(self):
        print "handle_newGame(self)", (self,)


    def handle_loseGame(self):
        print "handle_loseGame(self)", (self,)


    def handle_reallyStartGame(self):
        print "handle_reallyStartGame(self)", (self,)


    def handle_saveCityAs(self):
        print "handle_saveCityAs(self)", (self,)


    def handle_showBudgetAndWait(self):
        print "handle_showBudgetAndWait(self)", (self,)
        # @todo Show budget window. Actually pause the engine here. Maybe start a timeout to un-pause it.


    def handle_showPicture(self, id):
        #print "handle_showPicture(self, id)", (self, id)
        print "SHOWPICTURE", id


    def handle_showZoneStatus(self, str, s0, s1, s2, s3, s4, x, y):
        print "handle_showZoneStatus(self, str, s0, s1, s2, s3, s4, x, y)", (self, str, s0, s1, s2, s3, s4, x, y)


    def handle_startEarthquake(self, magnitude):
        print "handle_startEarthquake(self, magnitude)", (self, magnitude,)


    def handle_startScenario(self, scenario):
        print "handle_startScenario(self, scenario)", (self, scenario)


    def handle_startLoad(self):
        print "handle_startLoad(self)", (self,)


    def handle_winGame(self):
        print "handle_winGame(self)", (self,)


    def handle_update(self, name, *args):
        #print "handle_update(self, name, *args)", (self, name, args)

        self.sendUpdate(name, *args)


    def handle_simRobots(self):
        #print "handle_simRobots(self)", (self,)
        self.simRobots()


    def handle_invalidateEditors(self):
        pass


    def handle_invalidateMaps(self):
        pass


    def handle_simulateChurch(self, x, y, tile):
        pass


########################################################################
