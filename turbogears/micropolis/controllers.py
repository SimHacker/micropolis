########################################################################
# controllers.py
# TurboGears controller classes.
# Written for Micropolis, by Don Hopkins.
# Licensed under GPLv3.


########################################################################
# Imports


import re, os, sys, zipfile, time, math, tempfile, array, random
import types, urllib, urllib2, code, traceback, signal
import base64, hmac, hashlib, simplejson
from StringIO import StringIO
import pyamf
from pyamf import remoting, amf0, amf3
from pyamf.remoting import gateway
import xml.dom.minidom
import xml.etree.ElementTree as ElementTree
import cairo, pango
import genshi
from genshi import XML

import cherrypy
from cherrypy import request, response

import turbogears as tg
from turbogears import controllers, scheduler, identity, redirect
from turbogears import validate, validators, expose, flash, config
from turbogears.controllers import url
from micropolis import model
from model import *

#import xml.etree.cElementTree as ElementTree
from pyMicropolis.micropolisEngine.xmlutilities import *

from pyMicropolis.micropolisEngine import micropolisengine, micropolisturbogearsengine
from pyMicropolis.tileEngine import tileengine

import logging
log = logging.getLogger("micropolis.controllers")


########################################################################
# Globals


ServerURL = 'http://www.MicropolisOnline.com'
DefaultLanguage = config.get('micropolis.default_language', 'en-US')
DataDir = config.get('micropolis.data_dir', 'micropolis/htdocs/static/data')
LocalServerRoot = config.get('micropolis.local_server_root', 'http://127.0.0.1:8082')
GoogleTranslateAPIURL = 'https://www.googleapis.com/language/translate/v2'

FacebookCanvasName = config.get('micropolis.facebook_canvas_name', 'XXX')
FacebookAppID = config.get('micropolis.facebook_app_id', 'XXX')
FacebookAppKey = config.get('micropolis.facebook_app_key', 'XXX')
FacebookAppSecret = config.get('micropolis.facebook_app_secret', 'XXX')
FacebookPermissions = config.get('micropolis.facebook_permissions', 'XXX')
GoogleAPIIdentityKey = config.get('micropolis.google_api_identity_key', 'XXX')


FacebookProducts = {

    'simoleans_10000': {
        'order_info': 'simoleans_10000',
        'price': 1, # $0.10 (10,000 per dollar)
        'title': 'Ten Thousand Simoleans',
        'description': "You can build a lot of stuff with ten thousand Simolians! Credited to your current city. As cheap as could be!",
        'image_url': '/static/images/product_simoleans_1000_icon.png',
        'product_url': '/server/product/simoleans_1000',
        'sequence': 1,
    },
    
    'simoleans_1000000': {
        'order_info': 'simoleans_1000000',
        'price': 25, # $2.50 (400,000 per dollar)
        'title': 'One Million Simoleans',
        'description': "Did you ever want to be a millionaire? Well here's your chance! Credited to your current city. This is the deal of the century!",
        'image_url': '/static/images/product_simoleans_1000000_icon.png',
        'product_url': '/server/product/simoleans_1000000',
        'sequence': 2,
    },
    
    'save_slot': {
        'order_info': 'save_slot',
        'price': 50, # $5.00
        'title': 'One Save File Slot',
        'description': "This enables you to save more cities! You can save one city per save slot. Don't leave town without it!",
        'image_url': '/static/images/product_save_slot_icon.png',
        'product_url': '/server/product/save_slot',
        'sequence': 3,
    },
    
    'karma_1000': {
        'order_info': 'karma_1000',
        'price': 100, # $10.00
        'title': '1000 Karma',
        'description': "A kilo-karma will put a sparkle into your day. Credited to your wonderful personality. Just as strong and effective as homeopathic medicine!",
        'image_url': '/static/images/product_karma_1000_icon.png',
        'product_url': '/server/product/karma_1000',
        'sequence': 4,
    },
    
    'karma_2000': {
        'order_info': 'karma_2000',
        'price': 200, # $20.00
        'title': '2000 Karma',
        'description': "A ton of karma never killed anyone. Credited to your wonderful personality. Your pets will think you're god!",
        'image_url': '/static/images/product_karma_2000_icon.png',
        'product_url': '/server/product/karma_2000',
        'sequence': 5,
    },
    
    'karma_5000': {
        'order_info': 'karma_5000',
        'price': 500, # $50.00
        'title': '5000 Karma',
        'description': "Five thousand is the largest isogrammic number in the English language, and that's a huge heap of karma. Credited to your wonderful personality. Your horiscope will come true!",
        'image_url': '/static/images/product_karma_5000_icon.png',
        'product_url': '/server/product/karma_5000',
        'sequence': 6,
    },
    
    'karma_9001': {
        'order_info': 'karma_9001',
        'price': 900, # $90.00
        'title': '9001 Karma',
        'description': "It's over nine thousaaaaaaaaaand karma!!! Credited to your wonderful personality. Best deal, by far!",
        'image_url': '/static/images/product_karma_9001_icon.png',
        'product_url': '/server/product/karma_9001',
        'sequence': 7,
    },
    
}


########################################################################
# Utilities


def Base64URLDecode(data):
    data = data.encode(u'ascii')
    data += '=' * (4 - (len(data) % 4))
    return base64.urlsafe_b64decode(data)


def Base64URLEncode(data):
    return base64.urlsafe_b64encode(data).rstrip('=')


def ParseSignedRequest(signed_request):
    data = None
    user_id = None
    access_token = None

    sig, payload = signed_request.split(u'.', 1)
    sig = Base64URLDecode(sig)
    data = simplejson.loads(Base64URLDecode(payload))

    expected_sig = hmac.new(
        FacebookAppSecret, msg=payload, digestmod=hashlib.sha256).digest()

    if sig != expected_sig:
        print "ERROR: ParseSignedRequest sig", sig, "does not equal expected_sig", expected_sig
        return None

    if data['issued_at'] < (time.time() - 86400):
        print "ERROR: ParseSignedRequest issued_at", data['issued_at'], "expired after", time.time()
        return None

    return data


def FacebookAPI(path, params=None, method=u'GET', domain=u'graph', access_token=None):
    print "FACEBOOKAPI", path, params
    if not params:
        params = {}
    params[u'method'] = method
    if u'access_token' not in params and access_token:
        params[u'access_token'] = access_token
    data = FetchURL(
        url=u'https://' + domain + u'.facebook.com' + path,
        payload=urllib.urlencode(params),
        method='POST',
        headers={
            u'Content-Type': u'application/x-www-form-urlencoded',
        })
    result = simplejson.loads(data)
    if isinstance(result, dict) and u'error' in result:
        raise Exception(result)
    return result


def FetchURL(
    url=None,
    payload=None,
    method=None,
    headers=None):

    #print "FETCHURL", url, payload, method, headers

    req = urllib2.Request(
        url=url,
        data=payload)

    if headers:
        for key in headers.keys():
            req.add_header(key, headers[key])

    f = urllib2.urlopen(req)

    data = f.read()

    #print "RESULT:", data
    
    return data

########################################################################
# Getting stack trace from a running Python application
# http://stackoverflow.com/questions/132058/getting-stack-trace-from-a-running-python-application
def debug(sig, frame):
    """Interrupt running process, and provide a python prompt for
    interactive debugging."""
    d={'_frame':frame}         # Allow access to frame object.
    d.update(frame.f_globals)  # Unless shadowed by global
    d.update(frame.f_locals)

    i = code.InteractiveConsole(d)
    message  = "Signal recieved : entering python shell.\nTraceback:\n"
    message += ''.join(traceback.format_stack(frame))
    i.interact(message)


def listen():
    signal.signal(signal.SIGUSR1, debug)  # Register handler
    signal.signal(signal.SIGQUIT, debug)  # Register handler


listen()


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


    # Seconds between heart beats.
    heartBeatInterval = 10


    # FIXME: Doesn't seem to work. Should give us the raw post body. 
    #_cpFilterList = [StreamFilter()]


    ########################################################################
    # Initialization.
    #
    # Initialize the web server.
    #
    def __init__(self, *args, **kw):
        super(Root, self).__init__(*args, **kw)

        self.heatBeatCookie = MakeRandomCookie()
        self.sessions = {}

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

        print "GATWAY SERVICES", gateway.services

        scheduler.add_interval_task(
            action=self.makeHeartBeat,
            taskname='micropolis_heartBeat',
            initialdelay=self.heartBeatInterval,
            interval=self.heartBeatInterval)


    ########################################################################
    # Utilities


    ########################################################################
    # heartBeat
    #
    # Perform periodic tasks. Called every self.heartBeatInterval seconds.
    #
    def makeHeartBeat(self):
        #print "HEARTBEAT"
        cookie = MakeRandomCookie()
        self.heartBeatCookie = cookie
        hbUrl = url(
            (LocalServerRoot, 'heartBeat',),
            { 
                'cookie': cookie,
            })
        data = urllib.urlopen(hbUrl).read()
        #print data,
        sys.stdout.flush()


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
    def getSession(self, sessionID, args=None):

        #print "GETSESSION", sessionID, args

        self.expireSessions()

        sessions = self.sessions
        session = sessions.get(sessionID, None)
        if session:
            session.touch()
            return session

        if args == None:
            print "ERROR: controller getSession args is None!", sessionID
            return None

        session = micropolisturbogearsengine.Session(self, sessionID, args)
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
    # adminConsole
    #
    # Administrative command console.
    #
    @expose(
        template="micropolis.templates.adminConsole")
    @identity.require(
        identity.in_group("admin"))
    def adminConsole(
        self):

        commands = [
            {
                'name': 'Users',
                'link': '/server/adminUsers',
                'description': 'List users.',
            },
            {
                'name': 'Payments',
                'link': '/server/adminPayments',
                'description': 'List payments.',
            },
            {
                'name': 'Cities',
                'link': '/server/adminCities',
                'description': 'List cities.',
            },
            {
                'name': 'Translations',
                'link': '/server/translations',
                'description': 'Edit translations.',
            },
        ]

        return {
            'commands': commands,
        }


    ########################################################################
    # product
    #
    # Product info.
    #
    @expose(
        template="micropolis.templates.product")
    def product(
        self,
        order_info=None):

        if ((not order_info) or
            (order_info not in FacebookProducts)):
            products = FacebookProducts.values()
            products.sort(lambda p1, p2: cmp(p1['sequence'], p2['sequence']))
            print "MANY PRODUCTS", products
        else:
            products = [FacebookProducts[order_info]]
            print "ONE PRODUCT", products

        user_id = ''
        if not turbogears.identity.current.anonymous:
            user_id = str(turbogears.identity.current.user.uid)
            print "Got user id", user_id

        return {
            'products': products,
            'app_id': FacebookAppID,
            'app_key': FacebookAppKey,
            'canvas_name': FacebookCanvasName,
            'user_id': user_id,
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
    # getTiles
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
    def getTiles(
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
        code = tileengine.TILE_CODE_RAW_BINARY_16

        tiles = engine.getTileData(col, row, cols, rows, code)

        return tiles


    ########################################################################
    # getMapImage
    #
    # Get a picture of the session's map.
    #
    @expose(
        content_type="image/png")
    @validate(validators = {
        'width': validators.Int(),
        'height': validators.Int(),
    })
    def getMapImage(
        self,
        sessionID='',
        width=120,
        height=100,
        overlay='',
        **kw):

        #print "GETMAPIMAGE", sessionID, width, height, overlay, kw

        session = self.getSession(sessionID)
        engine = session.engine

        surface = engine.getMapImage(
            width,
            height,
            overlay)

        #print "MAP IMAGE", surface

        if not surface:
            self.expectationFailed("Bad parameters.");

        tempFileName = tempfile.mktemp()
        surface.write_to_png(tempFileName)
        surface.finish()
        f = open(tempFileName, 'rb')
        data = f.read()
        f.close()
        os.unlink(tempFileName)

        #print "DATA", len(data), type(data)
        return data


    ########################################################################
    # getCityIcon
    #
    # Get a saved city's icon.
    #
    @expose(
        content_type="image/png")
    def getCityIcon(
        self,
        sessionID='',
        cityCookie=0,
        **kw):

        user = None
        if sessionID:
            session = self.getSession(sessionID)
            user = session.user

        savedCity = cityCookie and model.City.query.filter_by(cookie=cityCookie).first()
        if not savedCity:
            self.expectationFailed("Bad parameters.");

        if ((not savedCity.shared) and 
            ((not user) or
             (user.user_id != savedCity.user_id))):
            self.expectationFailed("Not shared.");

        data = savedCity.icon
        if not data:
            # TODO: Use a stand-in icon, or make one now.
            self.expectationFailed("No icon.");

        return str(data)


    ########################################################################
    # heartBeat
    #
    @expose()
    def heartBeat(
        self,
        cookie=''):
        if cookie != self.heartBeatCookie:
            print "Unexpected heart beat cookie -- got", cookie, "expected", self.heartBeatCookie
            return 'skip'

        #print "CONTROLLER heartBeat"
        self.expireSessions()
        sessions = self.sessions.values()
        #print len(sessions), "sessions"
        engines = set()
        for session in sessions:
            #print "controller heartBeat SESSION", session
            engine = session.engine
            if engine:
                engines.add(engine)
        #print len(engines), "engines"
        for engine in engines:
            engine.heartBeat()

        return '!'


    ########################################################################
    # translations
    #
    @expose(
        template="micropolis.templates.translations")
    @identity.require(
        identity.in_group("admin"))
    def translations(
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

            defaultDoc.firstChild.setAttribute("language", language)

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

            url = ServerURL + '/server/translations?language=' + language
            raise cherrypy.HTTPRedirect(url)

            # Read the changes back in.
            #strings, doc, stringEls, stringsPath = self.readStrings(language)

        return {
            'message': message,
            'defaultLanguage': DefaultLanguage,
            'language': language,
            'languages': languages,
            'strings': strings,
            'defaultStrings': defaultStrings,
            'googleAPIIdentityKey': GoogleAPIIdentityKey,
            'googleTranslateAPIURL': GoogleTranslateAPIURL,
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

        #print "AMFGATEWAY", kw, request
        #print "HEADERS", request.headers
        #print "PARAMS", request.params
        #print "GATEWAY", gateway

        response = self.gateway(request)

        #print "RESPONSE", response

        return response


    def startSessionService(self, ignore, args):
        #print "STARTSESSIONSERVICE", self, "ignore", ignore, "args", args
        session = self.getSession(micropolisturbogearsengine.UniqueID('SESSION_'), args)
        sys.stdout.flush()
        return session.sessionID


    def pollService(self, ignore, pollDict):
        #print "POLLSERVICE", "ignore", ignore, "pollDict", pollDict
        sessionID = str(pollDict['sessionID'])
        ref = pollDict['ref']
        messages = pollDict['messages']

        #print "pollService SESSIONID", sessionID
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


    ########################################################################
    # facebookCanvas
    #
    # Facebook canvas interface.
    #
    @expose()
    @validate(validators = {
        'debugging': validators.Int(),
        #'page_admin': validators.Int(),
    })
    def facebookCanvas(
        self,
        #debugging=0,
        #in_tab=0,
        #page_admin=0,
        #fb_page_id='',
        #locale=DefaultLanguage,
        tg_errors=None,
        *args,
        **kw):

        print "ERRORS", tg_errors

        print "KW", kw
        debugging = int(kw.get('debugging', 0) or 0)
        in_tab = int(kw.get('in_tab_flag', 0) or 0)
        page_admin = int(kw.get('page_admin', 0) or 0)
        fb_page_id = kw.get('fb_page_id', None) or ''
        locale = kw.get('locale', DefaultLanguage) or DefaultLanguage
        
        print "\nFACEBOOKCANVAS", "METHOD", request.method, "DEBUGGING", debugging, "IN_TAB", in_tab, "PAGE_ADMIN", page_admin, "FB_PAGE_ID", fb_page_id, "LOCALE", locale, "ARGS", args, "KW", kw

        if in_tab:
            print "DISPLAYING IN TAB"

        signed_request = None
        data = None
        user_id = ''
        access_token = ''
        me = None
        user = None
        page = None

        if ((request.method == 'POST') and
            ('signed_request' in kw)):

            signed_request = kw['signed_request']

        elif 'u' in cherrypy.request.simple_cookie:

            signed_request = cherrypy.request.simple_cookie['u'].value

        if signed_request:
            data = ParseSignedRequest(signed_request)

        # Allow the signed_request to work for up to 1 day.
        if data:

            user_id = data.get(u'user_id', '')
            access_token = data.get(u'oauth_token', '')

            if user_id:

                payload = Base64URLEncode(simplejson.dumps({
                    u'user_id': user_id,
                    u'issued_at': str(int(time.time())),
                }))

                sig = Base64URLEncode(
                    hmac.new(
                        FacebookAppSecret,
                        msg=payload,
                        digestmod=hashlib.sha256).digest())

                cookie = sig + '.' + payload
                cherrypy.response.simple_cookie['u'] = cookie
                cherrypy.response.simple_cookie['u']['expires'] = 1440 * 60

            print "data", data
            print "user_id", user_id
            print "access_token", access_token

        if access_token:
            print "Getting me...", "access_token", access_token
            try:
                params = {
                    u'fields': u'picture,friends',
                    u'access_token': access_token,
                }
                me = FacebookAPI(
                    u'/me',
                    params)
            except Exception, e:
                print "ERROR CALLING FacebookAPI", "/me", params, e

            if me:
                print "Got me:", me.get('name', '???'), me.get('picture', '???')
            else:
                print "I don't know who me is."

        if me and user_id:

            user = model.User.by_uid(user_id)

            if not user:
                print "Creating new user id", user_id
                user = model.User(
                    uid=user_id,
                    user_name=user_id,
                    password=micropolisturbogearsengine.UniqueID(''),
                    created=datetime.now())

            user.access_token = access_token
            user.first_name = me.get('first_name', '')
            user.middle_name = me.get('middle_name', '')
            user.last_name = me.get('last_name', '')
            user.name = me.get('name', '')
            user.picture = me.get('picture', '')
            user.timezone = me.get('timezone', '')
            user.locale = me.get('locale', DefaultLanguage)
            user.username = me.get('username', '')
            user.email = me.get('email', '')
            user.third_party_id = me.get('third_party_id', '')
            user.email = me.get('email', '')
            user.activity = datetime.now()

            # Log them in.
            user.login()

        if fb_page_id:
            params = {
                u'access_token': access_token,
            }
            page = FacebookAPI(
                u'/page/' + str(fb_page_id),
                params)
            print "PAGE", page

        lzr_param = 'swf10'
        debugging_params_amp = '' if debugging else 'lzt=swf&'
        user_name = (me and me.get('name')) or 'Mayor'

        if page_admin:
            tg_template = 'micropolis.templates.facebookPageAdmin'
        else:
            tg_template = 'micropolis.templates.facebookCanvas'

        return {
            #'me': me,
            #'user': user,
            'app_id': FacebookAppID,
            'app_key': FacebookAppKey,
            'canvas_name': FacebookCanvasName,
            'fb_page_id': fb_page_id,
            'page': page,
            'in_tab': in_tab,
            'page_admin': page_admin,
            'tg_template': tg_template,
            'facebook_permissions': FacebookPermissions,
            'access_token': access_token,
            'user_id': user_id,
            'user_name': user_name,
            'user_locale': locale,
            'lzr_param': lzr_param,
            'debugging': debugging,
            'debugging_params_amp': debugging_params_amp,
        }
    

    ########################################################################
    # facebookPageAdmin
    #
    # Facebook page admin interface.
    #
    @expose(
        template="micropolis.templates.facebookPageAdmin")
    @validate(validators = {
        'debugging': validators.Int(),
    })
    def facebookPageAdmin(
        self,
        debugging=0,
        fb_page_id='',
        *args,
        **kw):

        print "\nFACEBOOKPAGEADMIN", "FB_PAGE_ID", fb_page_id, "METHOD", request.method, "ARGS", args, "KW", kw

        return {
        }


    ########################################################################
    # facebookCredits
    #
    # Facebook credits interface.
    #
    @expose(
        content_type="application/json")
    @validate(validators = {
        'test_mode': validators.Int(),
    })
    def facebookCredits(
        self,
        test_mode=0,
        order_id='',
        order_info='',
        signed_request='',
        receiver='',
        buyer='',
        method='',
        *args,
        **kw):

        print "\nFACEBOOKCREDITS", 'test_mode', test_mode, 'order_id', order_id, 'order_info', order_info, 'signed_request', signed_request, 'receiver', receiver, 'buyer', buyer, 'method', method, 'args', args, 'kw', kw

        if order_info:
            print "ORDER_INFO", type(order_info), repr(order_info)
            order_info = simplejson.loads(order_info)

        data = None
        if signed_request:
            data = ParseSignedRequest(signed_request)

        if method == 'payments_get_items':

            print "METHOD: payments_get_items", "order_info", order_info

            credits = data.get('credits')
            if not credits:
                print "FACEBOOKCREDITS payments_get_items signed request missing credits"
                return "{}"
            #print "CREDITS", credits
            
            user = data.get('user')
            print "USER", user

            buyer = credits.get('buyer')
            order_id = credits.get('order_id')
            test_mode = credits.get('test_mode')
            #order_info = credits.get('order_info')
            receiver = credits.get('receiver')

            print "CREDITS:", 'buyer', buyer, 'order_id', order_id, 'test_mode', test_mode, 'order_info', order_info, 'receiver', receiver, 'user', user

            if order_info not in FacebookProducts:
                print "Unknown order_info:", order_info
                return "{}"

            product = FacebookProducts[order_info]
            price = product['price']
            title = product['title']
            description = product['description']

            # Look up order_info in database.

            image_url = ServerURL + product['image_url']
            product_url = ServerURL + product['product_url']

            response = {
                'content': [
                    {
                        'title': title,
                        'description': description,
                        'price': price,
                        'image_url': image_url,
                        'product_url': product_url,
                        'data': order_info,
                    },
                ],
                'method': 'payments_get_items',
            }

            print "RESPONSE", response

            return simplejson.dumps(response)

        elif method == 'payments_status_update':
            
            print "METHOD: payments_status_update"

            print "DATA", data

            credits = data.get('credits')
            if not credits:
                print "FACEBOOKCREDITS payments_status_update signed request missing credits"
                return "{}"
            print "CREDITS", credits
            
            user = data.get('user')
            print "USER", user

            status = credits.get('status')
            order_id = credits.get('order_id')
            order_details = credits.get('order_details')
            test_mode = credits.get('test_mode')

            print "status", status, "order_id", order_id, "test_mode", test_mode, "order_details", order_details

            status = 'settled'

            response = {
                'content': {
                    'order_id': order_id,
                    'status': status,
                },
                'method': 'payments_status_update',
            }

            print "RESPONSE", response

            return simplejson.dumps(response)

        else:

            print "FACEBOOKCREDITS unexpected method", method

        return {}


########################################################################
