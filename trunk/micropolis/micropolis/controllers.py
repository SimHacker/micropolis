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
import micropolisengine
import micropoliswebutils
import tileengine


########################################################################
# Utilities


def ErrorMessage(
    message):
    
    return {
        'tg_template': 'micropolis.templates.error',
        'message': message,
    }


def StringToElement(s):
    f = StringIO(s)
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


    def before_request_body(self):

        print "STREAMFILTER BEFORE_REQUEST_BODY PATH", cherrypy.request.path
        if cherrypy.request.path == '/micropolisTick':
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


    def __init__(self, *args, **kw):
        super(Root, self).__init__(*args, **kw)

        self.initMicropolis()


    ########################################################################
    # Utilities


    ####################################
    # expectationFailed

    def expectationFailed(
        self,
        message):
        
        self.fatalError(
            417,
            message)

        
    ####################################
    # fatalError

    def fatalError(
        self,
        status,
        message):
        
        raise cherrypy.HTTPError(
            status,
            message)


    ####################################
    # initMicropolis

    def initMicropolis(self):

        self.sessions = {}

        self.game = micropoliswebutils.Game()


    ####################################
    # getSession

    def getSession(self, sessionID):

        sessions = self.sessions
        session = sessions.get(sessionID, None)
        if session:
            session.touch()
            return session

        session = micropoliswebutils.Session(sessionID)
        sessions[sessionID] = session

        session.setGame(self.game)
        session.touch()

        return session


    ####################################
    # checkTileBounds

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


    ####################################
    # index

    @expose(
        template="micropolis.templates.index")
    
    def index(
        self):

        game = self.game
        game.tickSim(1)

        return {
            'game': game,
            'm': game.m,
        }


    ####################################
    # console

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


    ####################################
    # login

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


    ####################################
    # logout

    @expose()
    def logout(
        self):
        
        identity.current.logout()
        redirect("/")


    ####################################
    # micropolisSessionStart

    @expose(
        template="micropolis.templates.micropolisSessionStart",
        content_type="text/xml")
    def micropolisSessionStart(
        self):
        session = self.getSession(micropoliswebutils.UniqueID('SESSION_'))
        return {
            'session': session,
        }


    ####################################
    # micropolisGetTiles

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
        col=0,
        row=0,
        cols=micropolisengine.WORLD_W,
        rows=micropolisengine.WORLD_H,
        code=0,
        **kw):

        self.checkTileBounds(col, row, cols, rows)

        game = self.game

        tengine = game.tengine
        tiles = self.tengine.getTileData(None, None, col, row, cols, rows, code)
        #print "TILES", tiles

        return tiles


    ####################################
    # micropolisTick

    @expose(
        template="micropolis.templates.micropolisTick",
        content_type="text/xml")
    @validate(validators = {
        'ref': validators.Int(),
    })
    def micropolisTick(
        self,
        ref=0,
        sessionID='',
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
        tick = StringToElement(body)
        try:
            tick = StringToElement(body)
        except Exception, e:
            self.expectationFailed("Error parsing XML body: " + str(e))

        tileviews = \
            self.game.handleTick(
                tick,
                session.tileViewCache)

        messages = session.receiveMessages()

        return {
            'ref': ref,
            'tileviews': tileviews,
            'messages': messages,
        }


    ####################################
    # micropolisGetTilesImage

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

        game = self.game
        game.tickSim(ticks)

        tileSize = 16
        width = cols * tileSize
        height = rows * tileSize

        surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
        ctx = cairo.Context(surface)

        game = self.game
        tengine = game.tengine

        alpha = 1.0

        tengine.renderTiles(
            ctx,
            game.tilesSurface,
            game.tilesWidth,
            game.tilesHeight,
            None,
            game.tileMap,
            game.tileSize,
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


    ####################################
    # micropolisView

    @expose(
        template="micropolis.templates.micropolisView")
    @validate(validators = {
        'col': validators.Int(),
        'row': validators.Int(),
        'cols': validators.Int(),
        'rows': validators.Int(),
        'ticks': validators.Int(),
    })
    def micropolisView(
        self,
        col=0,
        row=0,
        cols=micropolisengine.WORLD_W / 4,
        rows=micropolisengine.WORLD_H / 4,
        ticks=1,
        **kw):

        if ((col < 0) or
            (row < 0) or
            (cols <= 0) or
            (rows <= 0) or
            ((col + cols) > micropolisengine.WORLD_W) or
            ((row + rows) > micropolisengine.WORLD_H) or
            (ticks < 0) or
            (ticks > 1000000)):
            self.expectationFailed("Invalid parameter.");

        game = self.game
        
        game.handleCommands(kw)

        game.tickSim(ticks)

        return {
            'col': col,
            'row': row,
            'cols': cols,
            'rows': rows,
            'ticks': ticks,
            'game': game,
            'm': game.m,
            'CityNames': micropoliswebutils.CityNames,
        }


########################################################################
