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

GraphAPIURL = 'https://graph.facebook.com'
FacebookCanvasName = config.get('micropolis.facebook_canvas_name', 'XXX')
FacebookAppID = config.get('micropolis.facebook_app_id', 'XXX')
FacebookAppKey = config.get('micropolis.facebook_app_key', 'XXX')
FacebookAppSecret = config.get('micropolis.facebook_app_secret', 'XXX')
FacebookPermissions = config.get('micropolis.facebook_permissions', 'XXX')
GoogleAPIIdentityKey = config.get('micropolis.google_api_identity_key', 'XXX')


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
    #print "FACEBOOKAPI", path, params
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


def GetFacebookAccessToken():
    url = tg.url(
        GraphAPIURL + '/oauth/access_token',
        client_id=FacebookAppID,
        client_secret=FacebookAppSecret,
        grant_type='client_credentials')
    #print "URL", url

    token = FetchURL(url)
    #print "TOKEN", token
    prefix = 'access_token='
    if token.startswith(prefix):
        token = token[len(prefix):]

    return token


def GetFacebookUser(userID, fields=u'picture,education'):
    access_token = GetFacebookAccessToken()
    me = None

    try:
        params = {
            u'fields': fields,
            u'access_token': access_token,
        }
        me = FacebookAPI(
            u'/' + str(userID),
            params)
    except Exception, e:
        print "ERROR CALLING FacebookAPI", "/me", params, e

    return me


def GetFacebookPayments(startTime=0, endTime=0, status='settled'):
    #print "GetPayments", startTime, endTime, status
    access_token = GetFacebookAccessToken()

    day = 24 * 60 * 60
    since = startTime
    results = []
    while since < endTime:
        #print "since", since, "endTime", endTime
        
        duration = min(day, endTime - since)
        #print "duration", duration
        
        until = since + duration
        #print "since", since, time.asctime(time.gmtime(since))
        #print "until", until, time.asctime(time.gmtime(until))
        #print "duration", until - since

        url = tg.url(
            GraphAPIURL + '/' + FacebookAppID + '/payments',
            status=status,
            since=since,
            until=until,
            access_token=access_token)
        print "URL", url

        result = FetchURL(url)
        #print "RESULT", result
        dict = simplejson.loads(result)
        #print "DICT", dict
        payments = dict['data']
        #print "PAYMENTS", payments

        results += payments
        since += duration

    print "TOTAL PAYMENT COUNT", len(results)
    return results


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
    #heartBeatInterval = 10
    heartBeatInterval = 60


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
                'link': '/server/adminUser',
                'description': 'List users.',
            },
            {
                'name': 'Cities',
                'link': '/server/adminCity',
                'description': 'List cities.',
            },
            {
                'name': 'Products',
                'link': '/server/adminProduct',
                'description': 'List products.',
            },
            {
                'name': 'Orders',
                'link': '/server/adminOrder',
                'description': 'List orders.',
            },
            {
                'name': 'Facebook Payments',
                'link': '/server/adminFacebookPayment',
                'description': 'List facebook payments.',
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
    # adminUser
    #
    # Administrative command console, user page.
    #
    @expose(
        template="micropolis.templates.adminUser")
    @identity.require(
        identity.in_group("admin"))
    def adminUser(
        self,
        user_id=''):

        if user_id:
            user = User.query.filter_by(user_id=user_id).first()
            if user:
                users = [user]
            else:
                users = []
        else:
            users = User.query().all()

        return {
            'user_id': user_id,
            'users': users,
        }


    ########################################################################
    # adminCity
    #
    # Administrative command console, city page.
    #
    @expose(
        template="micropolis.templates.adminCity")
    @identity.require(
        identity.in_group("admin"))
    def adminCity(
        self,
        city_id='',
        user_id=''):

        if city_id:
            city = City.query.filter_by(city_id=city_id).first()
            if city:
                cities = [city]
            else:
                cities = []
        elif user_id:
            cities = City.query.filter_by(user_id=user_id).all()
        else:
            cities = City.query().all()

        return {
            'city_id': city_id,
            'user_id': user_id,
            'cities': cities,
        }


    ########################################################################
    # adminProduct
    #
    # Administrative command console, product page.
    #
    @expose(
        template="micropolis.templates.adminProduct")
    @identity.require(
        identity.in_group("admin"))
    def adminProduct(
        self,
        product_id=''):

        if product_id:
            produce = City.query.filter_by(city_id=city_id).first()
            if product:
                products = [product]
            else:
                products = []
        else:
            products = Product.query().order_by(Product.sequence).all()

        return {
            'product_id': product_id,
            'products': products,
        }


    ########################################################################
    # adminOrder
    #
    # Administrative command console, order page.
    #
    @expose(
        template="micropolis.templates.adminOrder")
    @identity.require(
        identity.in_group("admin"))
    def adminOrder(
        self,
        order_id='',
        user_id=''):

        if order_id:
            order = Order.query.filter_by(order_id=order_id).first()
            if order:
                orders = [order]
            else:
                orders = []
        elif user_id:
            orders = Order.query.filter_by(user_id=user_id).all()
        else:
            orders = Order.query().all()

        return {
            'order_id': order_id,
            'user_id': user_id,
            'orders': orders,
        }


    ########################################################################
    # adminFacebookPayment
    #
    # Administrative command console, payment page.
    #
    @expose(
        template="micropolis.templates.adminFacebookPayment")
    @identity.require(
        identity.in_group("admin"))
    @validate(validators = {
        'startTime': validators.Int(),
        'endTime': validators.Int(),
        'daysAgo': validators.Int(),
    })
    def adminFacebookPayment(
        self,
        payment_id='',
        startTime=0,
        endTime=0,
        daysAgo=14):

        now = int(time.time())
        day = 24 * 60 * 60

        if daysAgo:
            startTime = now - (daysAgo * day)
            endTime = now

        payments = GetFacebookPayments(startTime, endTime)
        print "PAYMENTS", payments

        return {
            'payment_id': payment_id,
            'startTime': startTime,
            'endTime': endTime,
            'daysAgo': daysAgo,
            'payments': payments,
        }


    ########################################################################
    # cityIcon
    #
    # Return city icon image stored in database.
    #
    @expose(
        content_type="image/png")
    def cityIcon(
        self,
        cookie='',
        city_id=''):

        city = None
        if cookie:
            city = City.query.filter_by(cookie=cookie).first()
        elif city_id:
            city = City.query.filter_by(city_id=city_id).first()

        if not city:
            return ""

        icon = city.icon
        if not icon:
            return ""
        
        return str(icon)


    ########################################################################
    # cityThumbnail
    #
    # Return city thumbname image stored in database.
    #
    @expose(
        content_type="image/png")
    def cityThumbnail(
        self,
        cookie='',
        city_id=''):

        city = None
        if cookie:
            city = City.query.filter_by(cookie=cookie).first()
        elif city_id:
            city = City.query.filter_by(city_id=city_id).first()

        if not city:
            return ""

        thumbnail = city.thumbnail
        if not thumbnail:
            return ""
        
        return str(thumbnail)


    ########################################################################
    # product
    #
    # Product info.
    #
    @expose(
        template="micropolis.templates.product")
    def product(
        self,
        item_id=None):

        produce = None
        if item_id:
            product = Product.query.filter_by(item_id=item_id).first()

        if product:
            products = [product]
            print "ONE PRODUCT", products
        else:
            products = list(Product.query.filter_by(active=True).order_by(Product.sequence).all())
            item_id = None
            print "MANY PRODUCTS", products

        user_id = ''
        if not turbogears.identity.current.anonymous:
            user_id = str(turbogears.identity.current.user.facebook_user_id)
            print "Got user id", user_id

        return {
            'item_id': item_id,
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

        savedCity = cityCookie and City.query.filter_by(cookie=cityCookie).first()
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

        #print "ERRORS", tg_errors
        #print "KW", kw

        debugging = int(kw.get('debugging', 0) or 0)
        in_tab = int(kw.get('in_tab_flag', 0) or 0)
        page_admin = int(kw.get('page_admin', 0) or 0)
        fb_page_id = kw.get('fb_page_id', None) or ''
        locale = kw.get('locale', DefaultLanguage) or DefaultLanguage
        
        #print "\n\nFACEBOOKCANVAS", "method", request.method, "debugging", debugging, "in_tab", in_tab, "page_admin", page_admin, "fb_page_id", fb_page_id, "locale", locale, "args", args, "kw", kw

        if in_tab:
            pass#print "FACEBOOKCANVAS Displaying in tab! How cool is that?"

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

            #print "data", data
            #print "user_id", user_id
            #print "access_token", access_token

        if access_token:
            #print "Getting me...", "access_token", access_token
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
                pass#print "Got me:", me.get('name', '???'), me.get('picture', '???')
            else:
                print "FACEBOOKCANVAS ERROR: Got access_token but I can't find out who me is! data", data

        if me and user_id:

            user = User.by_facebook_user_id(user_id)

            if not user:
                print "FACEBOOKCANVAS Creating new user_id", user_id, "me", me
                user = User(
                    facebook_user_id=user_id,
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
            user.facebook_user_name = me.get('username', '')
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
            #print "FACEBOOKCANVAS Page", page

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

        #print "\n\nFACEBOOKCREDITS", 'test_mode', test_mode, 'order_id', order_id, 'order_info', order_info, 'signed_request', signed_request, 'receiver', receiver, 'buyer', buyer, 'method', method, 'args', args, 'kw', kw

        data = None
        if signed_request:
            data = ParseSignedRequest(signed_request)
        #print "DATA", data

        #print "METHOD", method

        if method == 'payments_get_items':

            credits = data.get('credits')
            if not credits:
                print "FACEBOOKCREDITS ERROR: payments_get_items signed request missing credits"
                return "{}"
            #print "CREDITS", credits
            
            user = data.get('user')
            #print "USER", user

            buyer = credits.get('buyer')
            order_id = str(credits.get('order_id'))
            test_mode = credits.get('test_mode')
            order_info = credits.get('order_info', '""')
            item_id = simplejson.loads(order_info)
            receiver = credits.get('receiver')
            #print "FACEBOOKCREDITS buyer', buyer, 'order_id', order_id, 'test_mode', test_mode, 'item_id', item_id, 'receiver', receiver, 'order_info', order_info, 'user', user

            product = Product.query.filter_by(item_id=item_id).first()
            if not product:
                print "Unknown Product item_id:", item_id
                return "{}"
            #print "PRODUCT", product

            price = product.price
            title = product.title
            description = product.description
            image_url = product.image_url
            product_url = product.product_url

            me = GetFacebookUser(buyer)
            if me:
                print "ME", me

            order = Order.query.filter_by(facebook_order_id=order_id).first()
            if order:
                print "FOUND EXISTING ORDER", order_id, order
            else:
                print "MAKING NEW ORDER"
                order = Order(
                    facebook_order_id=order_id.decode('utf8'),
                    item_id=item_id.decode('utf8'),
                    order_details=u'',
                    order_info=order_info.decode('utf8'),
                    price=price,
                    title=title.decode('utf8'),
                    description=description.decode('utf8'),
                    image_url=image_url.decode('utf8'),
                    product_url=product_url.decode('utf8'),
                    from_user_name=u'',
                    from_user_id=u'',
                    to_user_name=u'',
                    to_user_id=u'',
                    user_id=None,
                    product_id=product.product_id,
                    amount=product.price,
                    status=u'new',
                    confirmed_settled=False,
                    application_name=u'',
                    application_id=u'',
                    country=u'',
                    created=datetime.now(),
                    updated=datetime.now(),
                    refund_code=u'',
                    refund_message=u'',
                    refund_funding_source=False,
                    refund_params=u'',
                    comments=u'')
                print "CREATED ORDER", order_id, order

            response = {
                'content': [
                    {
                        'title': title,
                        'description': description,
                        'price': price,
                        'image_url': ServerURL + image_url,
                        'product_url': ServerURL + product_url,
                        'data': item_id,
                    },
                ],
                'method': 'payments_get_items',
            }
            #print "RESPONSE", response

            return simplejson.dumps(response)

        elif method == 'payments_status_update':
            
            # data = {
            #   u'user_id': u'502061754',
            #   u'algorithm': u'HMAC-SHA256',
            #   u'oauth_token': u'185197118179045|2.zSGjJDEh1p1a870_Cf_uIw__.3600.1298505600-502061754|ags7Ut1-GUpVHWSDxlc2fzsJ0g4',
            #   u'issued_at': 1298499531
            #   u'expires': 1298505600,
            #   u'user': {
            #     u'locale': u'en_US',
            #     u'country': u'nl',
            #     u'age': {u'min': 21}
            #   },
            #   u'credits': {
            #     u'status': u'placed',
            #     u'test_mode': 1
            #     u'order_id': 131169043619058,
            #     u'order_details': u'{ # NOTE: This is a string of JSON that needs to be parsed!
            #       "order_id":131169043619058,
            #       "buyer":502061754,
            #       "app":185197118179045,
            #       "receiver":502061754,
            #       "amount":1,
            #       "update_time":1298499531,
            #       "time_placed":1298499525,
            #       "data":"",
            #       "status":"placed",
            #       "items": [
            #         {
            #           "item_id":"0",
            #           "title":"One Thousand Micropoleans",
            #           "description":"That\'s a grand idea! You can build a lot of stuff with one thousand Micropolians! Credited to your current city. As cheap as could be!",
            #           "image_url":"http:\\/\\/www.MicropolisOnline.com\\/static\\/images\\/product_micropoleans_1000_icon.png",
            #           "product_url":"http:\\/\\/www.MicropolisOnline.com\\/server\\/product\\/micropoleans_1000",
            #           "price":1,
            #           "data":"micropoleans_1000"
            #         }
            #       ]
            #     }'
            #   }
            # }

            # Get the user_id from the data.
            user_id = data.get('user_id')
            if not user_id:
                print "FACEBOOKCREDITS ERROR: MISSING user_id from data", data
                return "{}"
            #print "USER_ID", user_id

            # Get the user from the database.
            user = User.query.filter_by(facebook_user_id=user_id).first()
            if not user:
                print "FACEBOOKCREDITS ERROR: MISSING user buyer", buyer, "order_details", order_details
                return "{}"
            #print "USER", user

            # Get the credits from the data.
            credits = data['credits']
            if not credits:
                print "FACEBOOKCREDITS ERROR: MISSING credits from data", data
                return "{}"
            #print "CREDITS", credits

            # Get the status from the credits.
            status = credits.get('status')
            if not status:
                print "FACEBOOKCREDITS ERROR: MISSING status from credits", credits
                return "{}"
            #print "STATUS", status

            # Get the test_mode from the credits.
            test_mode = credits.get('test_mode')
            #print "TEST_MODE", test_mode

            # Get the order_id from the credits.
            order_id = credits.get('order_id')
            if not order_id:
                print "FACEBOOKCREDITS ERROR: MISSING order_id from credits", credits
                return "{}"
            #print "ORDER_ID", order_id

            # Get the order form the database.
            order = Order.query.filter_by(facebook_order_id=order_id).first()
            if not order:
                print "FACEBOOKCREDITS ERROR: MISSING order", order_id
                return "{}"
            #print "ORDER", order_id, order

            # Get the order_details from the credits.
            order_details_json = credits.get('order_details')
            if not order_details_json:
                print "FACEBOOKCREDITS ERROR: MISSING order_details from credits", credits
                return "{}"
            order_details = simplejson.loads(order_details_json)
            #print "ORDER_DETAILS", order_details

            # Get the item from the order_details.
            items = order_details.get('items')
            if (not items):
                print "FACEBOOKCREDITS ERROR: MISSING items from order_details", order_details
                return "{}"
            if len(items) != 1:
                print "FACEBOOKCREDITS ERROR: WRONG number of items from order_details", items
                return "{}"
            item = items[0]
            #print "ITEM", item

            # Get the product item id from the item.
            product_item_id = item.get('data')
            if not product_item_id:
                print "FACEBOOKCREDITS ERROR: MISSING product_item_id data from item", item
                return "{}"
            #print "PRODUCT_ITEM_ID", product_item_id

            # Get the product from the database.
            product = Product.query.filter_by(item_id=product_item_id).first()
            if not product:
                print "FACEBOOKCREDITS ERROR: MISSING Product product_item_id", product_item_id
                return "{}"
            #print "PRODUCT", product

            # Get the product_data from the product.
            if not product.data:
                print "FACEBOOKCREDITS ERROR: EMPTY product.data from product", product
                return "{}"
            product_data = simplejson.loads(product.data)
            if not product_data:
                print "FACEBOOKCREDITS ERROR: NULL product_data", product.data
                return "{}"
            #print "PRODUCT_DATA", product_data

            if status == 'placed':

                # The order was placed, so take action on it, and change to settled.
                
                new_status = user.purchaseProduct(product, product_data, order, True)

                #print "NEW_STATUS FROM user.purchaseProduct", new_status

                order.status = new_status.decode('utf8')
                order.order_details = order_details_json.decode('utf8')
                order.test_mode = test_mode
                order.updated = datetime.now()

                print "FACEBOOKCREDITS SUCCESS status placed", order

            elif status == 'settled':

                # The confirmation came back that the order was settled, so remember that it's a done deal.

                order.updated = datetime.now()
                order.confirmed_settled = True
                new_status = status

                print "FACEBOOKCREDITS SUCCESS status settled", order

            else:

                print "FACEBOOKCREDITS ERROR: WRONG status", status, "credits", credits
                return "{}"

            response = {
                'content': {
                    'order_id': order_id,
                    'status': new_status,
                },
                'method': 'payments_status_update',
            }

            print "RESPONSE", response

            return simplejson.dumps(response)

        else:

            print "FACEBOOKCREDITS ERROR: unexpected method", method

        return {}


########################################################################
