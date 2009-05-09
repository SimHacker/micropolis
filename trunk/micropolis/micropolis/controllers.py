########################################################################
# controllers.py
# TurboGears controller classes.
# Written for Micropolis, by Don Hopkins.
# Licensed under GPLv3.


########################################################################
# Imports


import turbogears as tg
from turbogears import controllers, expose, validate, validators, flash
from micropolis import model
from model import *
from turbogears import identity, redirect
import cherrypy
from cherrypy import request, response
from micropolis import json
import re, os, sys, zipfile, time, math, tempfile, array, random
import genshi
from genshi import XML
import xml.etree.ElementTree as ElementTree
#import xml.etree.cElementTree as ElementTree
from StringIO import StringIO

import logging
log = logging.getLogger("micropolis.controllers")

import cairo, pango
from pyMicropolis.simEngine import micropolisengine, micropolisturbogearsengine
from pyMicropolis.tileEngine import tileengine


########################################################################
# Utilities


########################################################################
# Takes a string, and parses it into an XML element.
#
def StringToElement(s):
    f = StringIO(s)
    et = ElementTree.parse(f)
    el = et.getroot()
    return el


########################################################################
# Classes


########################################################################
# CherryPy filter class.
#
# From Etienne Posthumus:
# http://www.epoz.org/blog/entries/20060630.1/view
#
# Enables getting the raw post body in request.body.
# FIXME: I can't get this to work. Still not getting raw post body.


class StreamFilter(cherrypy.filters.basefilter.BaseFilter):


    ####################################
    # Before request body callback.
    #
    def before_request_body(self):

        print "STREAMFILTER BEFORE_REQUEST_BODY PATH", cherrypy.request.path
        if cherrypy.request.path == '/micropolisPoll':
            # If you don't check that it is a post method the server might lock up
            # we also check to make sure something was submitted
            if not 'Content-Length' in cherrypy.request.headerMap or \
                   (cherrypy.request.method != 'POST'):
                raise cherrypy.HTTPRedirect('/')
            else:
                # Tell CherryPy not to parse the POST data itself for this URL
                print "Don't process request body!", cherrypy.request.processRequestBody
                cherrypy.request.processRequestBody = False


########################################################################
# Root controller class.


class Root(controllers.RootController):


    # FIXME: Doesn't seem to work. Should give us the raw post body. 
    #_cpFilterList = [StreamFilter()]


    ########################################################################
    # Initialization.
    #
    # Initialize the web server.
    #
    def __init__(self, *args, **kw):
        super(Root, self).__init__(*args, **kw)

        self.initMicropolis()


    ########################################################################
    # Utilities


    ########################################################################
    # initMicropolis
    #
    # Initialize the global Micropolis engine.
    # TODO: Don't use the global engine. Use an anonymous session's engine.
    #
    def initMicropolis(self):

        self.sessions = {}

        self.engine = micropolisturbogearsengine.CreateTurboGearsEngine()


    ########################################################################
    # expectationFailed
    #
    # Report an unexpected error, given a message.
    #
    def expectationFailed(
        self,
        message):
        
        self.fatalError(
            417,
            message)

        
    ########################################################################
    # fatalError
    #
    # Report a fatal error, given a status code and a message.
    #
    def fatalError(
        self,
        status,
        message):
        
        raise cherrypy.HTTPError(
            status,
            message)


    ########################################################################
    # getSession
    #
    # Return a session, given a session id, or create a new session if it's not defined.
    # Touch the session to update its last used time.
    #
    def getSession(self, sessionID):

        self.expireSessions()

        sessions = self.sessions
        session = sessions.get(sessionID, None)
        if session:
            session.touch()
            return session

        session = micropolisturbogearsengine.Session(sessionID)
        sessions[sessionID] = session

        session.createEngine()
        session.touch()

        return session


    ########################################################################
    # expireSessions
    #
    # Expire any sessions that have not been used in a while.
    #
    def expireSessions(self):
        oldSessions = []
        for session in self.sessions.values():
            if session.isExpired():
                oldSessions.append(session)
        for session in oldSessions:
            session.expire()


    ########################################################################
    # checkTileBounds
    #
    # Check to see if the rectangle is within the bounds of the map.
    #
    def checkTileBounds(self, col, row, cols, rows):
        if ((col < 0) or
            (row < 0) or
            (cols <= 0) or
            (rows <= 0) or
            ((col + cols) > micropolisengine.WORLD_W) or
            ((row + rows) > micropolisengine.WORLD_H)):
            self.expectationFailed("Invalid tile coordinates.");


    ########################################################################
    # Web pages and services.


    ########################################################################
    # index
    #
    # Main index page.
    #
    @expose(
        template="micropolis.templates.index")
    def index(
        self):

        return {}


    ########################################################################
    # console
    #
    # Administrative command console.
    #
    @expose(
        template="micropolis.templates.console")
    @identity.require(
        identity.in_group("admin"))
    def console(
        self):

        return {
            'commands': [
                {
                    'name': 'Micropolis View',
                    'link': '/micropolisView',
                    'description': 'View the Micropolis game.',
                },
                {
                    'name': 'Get Tiles',
                    'link': '/micropolisGetTles',
                    'description': 'Get an image of all the Micropolis game tiles.',
                },
            ],
        }


    ########################################################################
    # login
    #
    # Login page.
    #
    @expose(template="micropolis.templates.login")
    def login(
        self,
        forward_url=None,
        **kw):

        if forward_url:
            if isinstance(forward_url, list):
                forward_url = forward_url.pop(0)
            else:
                del request.params['forward_url']

        if ((not identity.current.anonymous) and
            identity.was_login_attempted() and
            (not identity.get_identity_errors())):
            redirect(tg.url(forward_url or '/', kw))

        if identity.was_login_attempted():
            msg = _("The credentials you supplied were not correct or "
                   "did not grant access to this resource.")
        elif identity.get_identity_errors():
            msg = _("You must provide your credentials before accessing "
                   "this resource.")
            if not forward_url:
                forward_url = request.path_info
        else:
            msg = _("Please log in.")
            if not forward_url:
                forward_url = request.headers.get("Referer", "/")

        # we do not set the response status here anymore since it
        # is now handled in the identity exception.
        return {
            'logging_in': True,
            'message': msg,
            'forward_url': forward_url,
            'previous_url': request.path_info,
            'original_parameters': request.params,
        }


    ########################################################################
    # logout
    #
    # Logout page.
    #
    @expose()
    def logout(
        self):
        
        identity.current.logout()
        redirect("/")


    ########################################################################
    # micropolisSessionStart
    #
    # Start a new session.
    #
    @expose(
        template="micropolis.templates.micropolisSessionStart",
        content_type="text/xml")
    def micropolisSessionStart(
        self):
        session = self.getSession(micropolisturbogearsengine.UniqueID('SESSION_'))
        return {
            'session': session,
        }


    ########################################################################
    # micropolisGetTiles
    #
    # Get some tiles from the session's engine.
    #
    @expose(
        content_type="application/x-micropolis-tiles")
    @validate(validators = {
        'col': validators.Int(),
        'row': validators.Int(),
        'cols': validators.Int(),
        'rows': validators.Int(),
        'code': validators.Int(),
    })
    def micropolisGetTiles(
        self,
        sessionID='',
        col=0,
        row=0,
        cols=micropolisengine.WORLD_W,
        rows=micropolisengine.WORLD_H,
        code=0,
        **kw):

        self.checkTileBounds(col, row, cols, rows)

        if not sessionID:
            self.expectationFailed("Invalid sessionID parameter.");
        session = self.getSession(sessionID)
        engine = session.engine

        tiles = engine.getTileData(col, row, cols, rows, code)

        return tiles


    ########################################################################
    # micropolisPoll
    #
    # Poll the session.
    #
    @expose(
        template="micropolis.templates.micropolisPoll",
        content_type="text/xml")
    @validate(validators = {
        'ref': validators.Int(),
    })
    def micropolisPoll(
        self,
        sessionID='',
        ref=0,
        body='',
        **kw):

        global request
        method = request.method
        if method != 'POST':
            self.expectationFailed("Expected post.");

        if not sessionID:
            self.expectationFailed("Invalid sessionID parameter.");
        session = self.getSession(sessionID)

        # FIXME: should get this from the post reqest body, but that doesn't work.
        #print "BODY", body
        try:
            poll = StringToElement(body)
        except Exception, e:
            self.expectationFailed("Error parsing XML body: " + str(e))

        tileviews = session.handlePoll(poll)
        messages = session.receiveMessages()

        return {
            'ref': ref,
            'tileviews': tileviews,
            'messages': messages,
        }


    ########################################################################
    # micropolisGetMapPicture
    #
    # Get a picture of the session's map.
    #
    @expose(
        content_type="image/png")
    @validate(validators = {
        'width': validators.Int(),
        'height': validators.Int(),
    })
    def micropolisGetMapImage(
        self,
        sessionID='',
        width=120,
        height=100,
        overlay='',
        **kw):

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
            self.expectationFailed("Invalid size.");

        session = self.getSession(sessionID)
        engine = session.engine

        tileSize = tileWidth

        surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
        ctx = cairo.Context(surface)

        alpha = 1.0

        engine.renderTiles(
            ctx,
            tileSize,
            0,
            0,
            worldW,
            worldH,
            alpha)

        overlayImage, overlayAlpha, overlayWidth, overlayHeight = \
            engine.getDataImageAlphaSize(overlay)
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

        fd, tempFileName = tempfile.mkstemp()
        os.close(fd)
        
        surface.write_to_png(tempFileName)
        surface.finish()
        f = open(tempFileName, 'rb')
        data = f.read()
        f.close()
        os.unlink(tempFileName)

        return data


    ########################################################################
    # micropolisGetTilesImage
    #
    # Get an image of some tiles.
    #
    # TODO: Don't use the global engine. Use an anonymous session's engine.
    #
    @expose(
        content_type="image/png")
    @validate(validators = {
        'col': validators.Int(),
        'row': validators.Int(),
        'cols': validators.Int(),
        'rows': validators.Int(),
        'ticks': validators.Int(),
    })
    def micropolisGetTilesImage(
        self,
        col=0,
        row=0,
        cols=micropolisengine.WORLD_W,
        rows=micropolisengine.WORLD_H,
        ticks=1,
        **kw):

        if ((col < 0) or
            (row < 0) or
            (cols <= 0) or
            (rows <= 0) or
            ((col + cols) > micropolisengine.WORLD_W) or
            ((row + rows) > micropolisengine.WORLD_H)):
            self.expectationFailed("Invalid tile coordinates.");

        engine = self.engine
        engine.tickSim(ticks)

        tileSize = 16
        width = cols * tileSize
        height = rows * tileSize

        surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
        ctx = cairo.Context(surface)

        alpha = 1.0

        engine.renderTiles(
            ctx,
            micropolisengine.EDITOR_TILE_SIZE,
            col,
            row,
            cols,
            rows,
            alpha)

        fd, tempFileName = tempfile.mkstemp()
        os.close(fd)
        
        surface.write_to_png(tempFileName)
        surface.finish()
        f = open(tempFileName, 'rb')
        data = f.read()
        f.close()
        os.unlink(tempFileName)

        return data


    ########################################################################
    # amfGateway
    #
    # Gateway to Flash AMF protocol.
    #
    @expose(
        content_type="amf")
    def amfGateway(
        self,
        **kw):

        print "AMF GATEWAY", kw

        # @todo: Integrate PyAMF module here.

        return ""


########################################################################
