# micropoliswebserver.py
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
# Micropolis Web Server
# Don Hopkins


########################################################################
# Import stuff


import sys
import os
import time
import math
import posixpath
import BaseHTTPServer
import urllib
import urlparse
import cgi
import shutil
import mimetypes
try:
    from cStringIO import StringIO
except ImportError:
    from StringIO import StringIO


########################################################################
# Import our modules


from pyMicropolis.simEngine import micropolisengine
from pyMicropolis.simEngine import micropolisutils

from pyMicropolis.tileEngine import tileengine


########################################################################
# Globals


__version__ = "0.9"


StaticContentRoot = 'http://toronto.activlab.com/static'


########################################################################
# MicropolisWebServer class


class MicropolisHTTPRequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):

    server_version = "MicropolisHTTP/" + __version__

    def __init__(
            self,
            *args,
            **params):

        print "__INIT__ MicropolisHTTPRequestHandler", "SELF", self, "ARGS", args, "PARAMS", params

        BaseHTTPServer.BaseHTTPRequestHandler.__init__(
            self,
            *args,
            **params)


    def do_GET(self):
        """Serve a GET request."""
        path = self.path
        print "do_GET", self, "path", path

        server = self.server
        m = server.m
        view = server.view

        now = time.time()
        fracTime = now - math.floor(now)
        m.flagBlink = fracTime < 0.5

        m.sim_tick()
        m.animateTiles()

        content = (
            """<html><head><title>Micropolis</title></head><body>\n""" +
            view.renderSummaryAsHtml() +
            view.renderClippedTilesAsHtml() +
            """\n</body><html>"""
        )

        self.send_response(200)

        contentType = "text/html"
        self.send_header("Content-Type", contentType)

        contentLength = len(content)
        self.send_header("Content-Length", str(contentLength))

        self.end_headers()

        f = self.wfile
        f.write(content)
        f.flush()


########################################################################
# MicropolisWebServer class


class MicropolisHTTPServer(BaseHTTPServer.HTTPServer):


    def __init__(
            self,
            *args,
            **params):

        print "__INIT__ MicropolisHTTPServer", "SELF", self, "ARGS", args, "PARAMS", params

        BaseHTTPServer.HTTPServer.__init__(
            self,
            *args,
            **params)

        m = micropolisengine.Micropolis()
        self.m = m
        print "Created Micropolis simulator engine:", m

        m.resourceDir = 'res'
        m.initGame()

        # Load a city file.
        path = os.path.basename(__file__)
        path = os.path.abspath(os.path.join(path, "../cities"))
        cityFileName = path + os.sep + 'haight.cty'
        print "Loading city file:", cityFileName
        m.loadFile(cityFileName)

        # Initialize the simulator engine.

        m.resume()
        m.setSpeed(2)
        m.setPasses(1000)
        m.setFunds(1000000000)
        m.autoGoto = 0
        m.cityTax = 12

        view = micropolisutils.MicropolisView(m)
        self.view = view


########################################################################


def test(HandlerClass = MicropolisHTTPRequestHandler,
         ServerClass = MicropolisHTTPServer):
    print "Starting web server:", HandlerClass, ServerClass
    BaseHTTPServer.test(HandlerClass, ServerClass)


if __name__ == '__main__':
    test()


########################################################################
