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
import re, os, sys, zipfile, time, math, tempfile, array, random, types
import genshi
from genshi import XML
import xml.etree.ElementTree as ElementTree
#import xml.etree.cElementTree as ElementTree
import xml.dom.minidom
from StringIO import StringIO
import pyamf
from pyamf import remoting, amf0, amf3
from pyamf.remoting import gateway

import logging
log = logging.getLogger("micropolis.controllers")

import cairo, pango
from pyMicropolis.simEngine import micropolisengine, micropolisturbogearsengine
from pyMicropolis.tileEngine import tileengine


########################################################################
# Globals


DefaultLanguage = 'en-US'
DataDir = 'micropolis/htdocs/static/data'


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

        #print "STREAMFILTER BEFORE_REQUEST_BODY PATH", cherrypy.request.path
        if cherrypy.request.path == '/micropolisPoll':
            # If you don't check that it is a post method the server might lock up
            # we also check to make sure something was submitted
            if not 'Content-Length' in cherrypy.request.headerMap or \
                   (cherrypy.request.method != 'POST'):
                raise cherrypy.HTTPRedirect('/')
            else:
                # Tell CherryPy not to parse the POST data itself for this URL
                #print "Don't process request body!", cherrypy.request.processRequestBody
                cherrypy.request.processRequestBody = False


########################################################################
# AMF <=> TurboGears Gateway


class TurboGearsGateway(gateway.BaseGateway):


    strict = False
    #objectEncoding = pyamf.AMF3
    objectEncoding = pyamf.AMF0
    controller = None


    def __init__(
        self, 
        controller=None,
        *args, 
        **kw):

        gateway.BaseGateway.__init__(self, *args, **kw);

        self.controller = controller


    def getResponse(self, request, rq):
        #print "TurboGearsGateway getResponse making response"
        response = remoting.Envelope(rq.amfVersion, rq.clientType)

        for name, message in rq:
            #print "Getting processor for message", message
            processor = self.getProcessor(message)
            #print "Got processor", processor
            #print "Calling processor with message", message, "request", request
            result = processor(message)
            #print "Got result", result
            #print "Store result name", name, "for message"
            response[name] = result

        return response


    def __call__(self, request):

        body = request.body.read()
        stream = None

        context = pyamf.get_context(self.objectEncoding)
        #print "CONTEXT", context

        # Decode the request
        if True:#try:
            rq = remoting.decode(body, context, strict=self.strict)
        #except (pyamf.DecodeError, EOFError):
        #    self.controller.fatalError(
        #        400,
        #        "Bad Request\n\nThe request body was unable to " \
        #        "be successfully decoded.")
        #except (KeyboardInterrupt, SystemExit):
        #    raise
        #except Exception, e:
        #    self.controller.fatalError(
        #        500,
        #        "Internal server error decoding request: " + str(e))

        # Process the request
        if True: #try:
            response = self.getResponse(request, rq)
        #except (KeyboardInterrupt, SystemExit):
        #    raise
        #except Exception, e:
        #    self.controller.fatalError(
        #        500,
        #        "Internal server error getting response.")

        # Encode the response
        if True:#try:
            stream = remoting.encode(response, context, strict=self.strict)
        #except:
        #    self.controller.fatalError(
        #        500,
        #        "Internal server error encoding response.")

        response = stream.getvalue()

        return response


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

        gateway = TurboGearsGateway(self)
        self.gateway = gateway

        gateway.addService(
            self.startSessionService,
            name='micropolis.startSession',
            description='Start a Micropolis session.',
            expose_request=True)

        gateway.addService(
            self.pollService,
            name='micropolis.poll',
            description='Poll the simulation.',
            expose_request=True)

        gateway.addService(
            self.echoService,
            name='micropolis.echo',
            description='Echo the parameter.',
            expose_request=True)

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

        session = micropolisturbogearsengine.Session(self, sessionID)
        sessions[sessionID] = session

        session.createEngine()
        session.touch()

        return session


    ########################################################################
    # getSessions
    #
    # Return a list of sessions.
    #
    def getSessions(self):
        self.expireSessions()
        return self.sessions.values()


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
    # micropolisGetMapImage
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

        #print "MAP IMAGE"

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
    # getLanguages
    #
    def getLanguages(
        self):
        languages = []
        for fileName in os.listdir(DataDir):
            if (fileName.startswith('strings_') and
                fileName.endswith('.xml')):
                languages.append(fileName[8:-4])
        return languages


    ########################################################################
    # readStrings
    #
    def readStrings(
        self,
        language):

        stringsPath = os.path.join(
            DataDir, 'strings_' + language + '.xml')

        try:
            doc = xml.dom.minidom.parse(stringsPath)
        except Exception, e:
            print "Error parsing xml file", stringsPath, e
            return [], None, None, None

        stringsEl = doc.firstChild
        if (stringsEl.nodeType != 1) or (stringsEl.nodeName != u'strings'):
            print "Strings file should contain top level <strings> element.", stringsPath
            return [], None, None, None

        strings = []
        stringEls = {}
        el = stringsEl.firstChild
        while el:
            if (el.nodeType == 1) and (el.nodeName == u'string'):
                if not el.hasAttribute('id'):
                    print "Expected id attribute on string element:", el.toxml().encode('utf-8')
                else:
                    id = el.getAttribute("id")
                    if el.hasAttribute('comment'):
                        comment = el.getAttribute('comment')
                    else:
                        comment = None
                    text = u''
                    subEl = el.firstChild
                    while subEl:
                        if subEl.nodeType == 3:
                            text += subEl.data
                        subEl = subEl.nextSibling
                    if id in stringEls:
                        print "Duplicate string id:", language, id, text, comment
                    else:
                        strings.append((id, text, comment))
                        stringEls[id] = el

            el = el.nextSibling

        return strings, doc, stringEls, stringsPath


    ########################################################################
    # getTranslations
    #
    @expose(
        template="micropolis.templates.getTranslations")
    def getTranslations(
        self,
        language=DefaultLanguage,
        command='',
        **kw):

        message = ''
        languages = self.getLanguages()
        #print "languages", languages
        strings, doc, stringEls, stringsPath = self.readStrings(language)
        #print "strings", strings

        defaultStrings, defaultDoc, defaultStringEls, defaultStringsPath = self.readStrings(DefaultLanguage)

        if command == 'update':
            stringMap = {}
            for t in strings:
                stringMap[t[0]] = t
            stringMapKeys = set(stringMap.keys())

            defaultStringMap = {}
            for t in defaultStrings:
                defaultStringMap[t] = t
            defaultStringMapKeys = set(defaultStringMap.keys())

            newStrings = {}
            for key in kw.keys():
                if key[:7] == 'string_':
                    num = int(key[7:])
                    id = strings[num][0]
                    newStrings[id] = kw[key]
                    #print "XXXX", num, repr(id), repr(key), repr(kw[key])

            for idDefault, textDefault, commentDefault in defaultStrings:
                if idDefault in newStrings:
                    textTranslated = newStrings[idDefault]
                    #print "TT1", repr(idDefault), repr(textTranslated)
                    if idDefault in stringMapKeys:
                        stringMapKeys.remove(idDefault)
                elif idDefault in stringMap:
                    textTranslated = stringMap[idDefault][1]
                    #print "TT2", repr(idDefault), repr(textTranslated)
                    stringMapKeys.remove(idDefault)
                else:
                    textTranslated = u'@' + language + '@ ' + textDefault
                    #print "TT3", repr(idDefault), repr(textTranslated)

                el = defaultStringEls[idDefault]

                while el.firstChild:
                    el.removeChild(el.firstChild)

                el.appendChild(defaultDoc.createTextNode(textTranslated))
                
            if stringMapKeys:
                print "Extra strings left over in ", language, "translation"
                for idString in stringMapKeys:
                    print stringMap[idString]

            try:
                os.path.rename(stringsPath, stringsPath + '.bak')
            except: pass

            print "Writing new strings to", stringsPath
            f = open(stringsPath, 'w')
            f.write(defaultDoc.toxml())
            f.close()

            # Read the changes back in.
            strings, doc, stringEls, stringsPath = self.readStrings(language)

        return {
            'message': message,
            'defaultLanguage': DefaultLanguage,
            'language': language,
            'languages': languages,
            'strings': strings,
            'defaultStrings': defaultStrings,
        }


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

        response = self.gateway(request)

        #print "RESPONSE", response

        return response


    def startSessionService(self, ignore):
        session = self.getSession(micropolisturbogearsengine.UniqueID('SESSION_'))
        #print "STARTSESSIONSERVICE", "sessionID", session
        return session.sessionID


    def pollService(self, ignore, pollDict):
        #print "POLLSERVICE", "pollDict", pollDict
        sessionID = str(pollDict['sessionID'])
        ref = pollDict['ref']
        messages = pollDict['messages']

        #print "SESSIONID", sessionID
        if not sessionID:
            self.expectationFailed("Invalid sessionID parameter.");
        session = self.getSession(sessionID)

        #print "Calling session.handlePoll", pollDict
        messages = session.handlePoll(pollDict)
        #print "Called session.handlePoll"

        #print "POLL MESSAGES", messages

        return {
            'ref': ref,
            'sessionID': sessionID,
            'messages': messages,
        }


    def echoService(self, ignore, param):
        #print "ECHOSERVICE", "param", param
        return param


########################################################################
