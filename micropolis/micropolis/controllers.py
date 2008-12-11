########################################################################
# micropolis controllers.py
# By Don Hopkins


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

import logging
log = logging.getLogger("micropolis.controllers")

import cairo, pango
import micropolisengine
import micropolisutils
import tileengine


########################################################################
# Global


ServerVersion = "0.9"
ServerWelcome = "Welcome to the Micropolis  web server!"

DefaultTicks = 1

MicropolisCorePath = 'micropolis/MicropolisCore'

MicropolisTileSize = 16
MicropolisTilesPath = 'micropolis/htdocs/static/images/micropolis_tiles.png'

CityNames = {
    'about': MicropolisCorePath + '/cities/about.cty',
    'badnews': MicropolisCorePath + '/cities/badnews.cty',
    'bluebird': MicropolisCorePath + '/cities/bluebird.cty',
    'bruce': MicropolisCorePath + '/cities/bruce.cty',
    'deadwood': MicropolisCorePath + '/cities/deadwood.cty',
    'finnigan': MicropolisCorePath + '/cities/finnigan.cty',
    'freds': MicropolisCorePath + '/cities/freds.cty',
    'haight': MicropolisCorePath + '/cities/haight.cty',
    'happisle': MicropolisCorePath + '/cities/happisle.cty',
    'joffburg': MicropolisCorePath + '/cities/joffburg.cty',
    'kamakura': MicropolisCorePath + '/cities/kamakura.cty',
    'kobe': MicropolisCorePath + '/cities/kobe.cty',
    'kowloon': MicropolisCorePath + '/cities/kowloon.cty',
    'kyoto': MicropolisCorePath + '/cities/kyoto.cty',
    'linecity': MicropolisCorePath + '/cities/linecity.cty',
    'med_isle': MicropolisCorePath + '/cities/med_isle.cty',
    'ndulls': MicropolisCorePath + '/cities/ndulls.cty',
    'neatmap': MicropolisCorePath + '/cities/neatmap.cty',
    'radial': MicropolisCorePath + '/cities/radial.cty',
    'senri': MicropolisCorePath + '/cities/senri.cty',
    'southpac': MicropolisCorePath + '/cities/southpac.cty',
    'splats': MicropolisCorePath + '/cities/splats.cty',
    'wetcity': MicropolisCorePath + '/cities/wetcity.cty',
    'yokohama': MicropolisCorePath + '/cities/yokohama.cty',
}


AniTiles = (
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31, 
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,
    # Fire
                                            57,  58,  59,  60,  61,  62,  63,  56,
    # No Traffic
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    # Light Traffic
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
    96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    # Heavy Traffic
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    # Wires & Rails
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    # Residential
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
    256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271,
    272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287,
    288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303,
    304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319,
    320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335,
    336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351,
    352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367,
    368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383,
    384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399,
    400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415,
    416, 417, 418, 419, 420, 421, 422,
    # Commercial
                                       423, 424, 425, 426, 427, 428, 429, 430, 431,
    432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447,
    448, 449, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463,
    464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479,
    480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493, 494, 495,
    496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511,
    512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527,
    528, 529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 540, 541, 542, 543,
    544, 545, 546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559,
    560, 561, 562, 563, 564, 565, 566, 567, 568, 569, 570, 571, 572, 573, 574, 575,
    576, 577, 578, 579, 580, 581, 582, 583, 584, 585, 586, 587, 588, 589, 590, 591,
    592, 593, 594, 595, 596, 597, 598, 599, 600, 601, 602, 603, 604, 605, 606, 607,
    608, 609, 610, 611,
    # Industrial
                        612, 613, 614, 615, 616, 617, 618, 619, 852, 621, 622, 623,
    624, 625, 626, 627, 628, 629, 630, 631, 632, 633, 634, 635, 636, 637, 638, 639,
    640, 884, 642, 643, 888, 645, 646, 647, 648, 892, 896, 651, 652, 653, 654, 655,
    656, 657, 658, 659, 660, 661, 662, 663, 664, 665, 666, 667, 668, 669, 670, 671,
    672, 673, 674, 675, 900, 904, 678, 679, 680, 681, 682, 683, 684, 685, 908, 687,
    688, 912, 690, 691, 692,
    # SeaPort
                             693, 694, 695, 696, 697, 698, 699, 700, 701, 702, 703,
    704, 705, 706, 707, 708,
    # AirPort
                             709, 710, 832, 712, 713, 714, 715, 716, 717, 718, 719,
    720, 721, 722, 723, 724, 725, 726, 727, 728, 729, 730, 731, 732, 733, 734, 735,
    736, 737, 738, 739, 740, 741, 742, 743, 744,
    # Coal power
                                                 745, 746, 916, 920, 749, 750, 924,
    928, 753, 754, 755, 756, 757, 758, 759, 760,
    # Fire Dept
                                                 761, 762, 763, 764, 765, 766, 767,
    768, 769, 770, 771, 772, 773, 774, 775, 776, 777, 778,
    # Stadium
                                                           779, 780, 781, 782, 783,
    784, 785, 786, 787, 788, 789, 790, 791, 792, 793, 794,
    # Stadium Anims
                                                           795, 796, 797, 798, 799,
    800, 801, 802, 803, 804, 805, 806, 807, 808, 809, 810,
    # Nuclear Power
                                                           811, 812, 813, 814, 815,
    816, 817, 818, 819, 952, 821, 822, 823, 824, 825, 826,
    # Power out + Bridges
                                                           827, 828, 829, 830, 831,
    # Radar dish
    833, 834, 835, 836, 837, 838, 839, 832,
    # Fountain / Flag
                                            841, 842, 843, 840, 845, 846, 847, 848,
    849, 850, 851, 844, 853, 854, 855, 856, 857, 858, 859, 852,
    # zone destruct & rubblize
                                                                861, 862, 863, 864,
    865, 866, 867, 867,
    # totally unsure
                        868, 869, 870, 871, 872, 873, 874, 875, 876, 877, 878, 879,
    880, 881, 882, 883,
    # Smoke stacks
                        885, 886, 887, 884, 889, 890, 891, 888, 893, 894, 895, 892,
    897, 898, 899, 896, 901, 902, 903, 900, 905, 906, 907, 904, 909, 910, 911, 908,
    913, 914, 915, 912, 917, 918, 919, 916, 921, 922, 923, 920, 925, 926, 927, 924,
    929, 930, 931, 928,
    # Stadium Playfield
                        933, 934, 935, 936, 937, 938, 939, 932, 941, 942, 943, 944,
    945, 946, 947, 940,
    # Bridge up chars
                        948, 949, 950, 951,
    # Nuclear swirl
                                            953, 954, 955, 952,
)


AniTileGroupMap = {}
for tileIndex in range(0, len(AniTiles)):

    #print "tileindex", tileIndex, (tileIndex in AniTileGroupMap)
    if tileIndex in AniTileGroupMap:
        continue

    nextTileIndex = AniTiles[tileIndex]

    if nextTileIndex == tileIndex:
        continue

    curGroup = [tileIndex]
    AniTileGroupMap[tileIndex] = curGroup
    #print "start", curGroup

    while True:
        if nextTileIndex in AniTileGroupMap:
            group = AniTileGroupMap[nextTileIndex]
            if group == curGroup:
                #print "looped"
                break
            #print "merge", curGroup, group
            # Merge curGroup into group.
            group.extend(curGroup)
            for i in curGroup:
                AniTileGroupMap[i] = group
            break

        tileIndex = nextTileIndex
        curGroup.append(tileIndex)
        #print "accumulate", tileIndex, curGroup
        AniTileGroupMap[nextTileIndex] = curGroup
        nextTileIndex = AniTiles[tileIndex]
        #print "... tileIndex", tileIndex, "nextTileindex", nextTileIndex


AniTileGroups = []
for group in AniTileGroupMap.values():
    if group not in AniTileGroups:
        AniTileGroups.append(group)

for group in AniTileGroups:
    group.sort()

AniTileGroups.sort()

print len(AniTileGroups), "AniTileGroups"

AniTileMap = array.array('i')
for i in range(0, len(AniTiles)):
    AniTileMap.append(i)

for group in AniTileGroups:
    baseTile = group[0]
    for tile in group:
        AniTileMap[tile] = baseTile

#print AniTileMap


########################################################################
# Utilities


def Now():
    return datetime.now()


def ErrorMessage(
    message):
    
    return {
        'tg_template': 'micropolis.templates.error',
        'message': message,
    }


def UniqueID():
    id = 'SESSION'
    for i in range(0, 4):
        id += "_%06d" % (random.randint(0, 1000000),)
    return id


########################################################################
# Classes


class Session(object):


    def __init__(self, sessionID):
        self.sessionID = sessionID
        self.messages = []
        self.touch()

        tileViewCache = array.array('i')
        self.tileViewCache = tileViewCache
        row = (-1,) * micropolisengine.WORLD_X
        for i in range(0, micropolisengine.WORLD_Y):
            tileViewCache.extend(row)


    def touch(self):
        self.timeTouched = Now()


    def age(self):
        return Now() - self.timeTouched


    def sendMessage(self, message):
        mesages.append(message)


    def receiveMessages(self):
        messages = self.messages
        self.messages = []
        return messages


####################################
# CherryPy Filter
# From Etienne Posthumus:
# http://www.epoz.org/blog/entries/20060630.1/view


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


####################################
# Root controller class.


class Root(controllers.RootController):


    #_cpFilterList = [StreamFilter()]


    def __init__(self, *args, **kw):
        super(Root, self).__init__(*args, **kw)

        self.initMicropolis()


    def renderClippedTilesAsHtml(self):

        out = []

        def onTilesBegin(left, top, right, bottom, w, h):

            out.append(
                """<div style="width: %dpx; height: %dpx">\n""" % (
                    w * 16,
                    h * 16,
                ))

            out.append(
                """<table gap="0" cellpadding="0" cellspacing="0">\n""" % (
                    left, top, right, bottom, w, h,
                ))

        def onRowBegin(y, w, h):
            out.append(
                """<tr height="16">""" % (
                    y,
                ))

        def onTile(x, y, tile, flags):
            out.append(
                """<td class="t" style="background-position: %dpx %dpx"/>""" % (
                    -16 * (tile % 16),
                    -16 * (tile / 16),
                ))

        def onRowEnd(y, w, h):
            out.append(
                """</tr>\n""")

        def onTilesEnd(left, top, right, bottom, w, h):

            out.append(
                """</table>\n""")
        
            out.append(
                """</div>\n""")

        self.applyToClippedTiles(
            onTilesBegin,
            onRowBegin,
            onTile,
            onRowEnd,
            onTilesEnd)
            
        return "".join(out)


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
    # index

    @expose(
        template="micropolis.templates.index")
    
    def index(
        self):

        self.tickSim(DefaultTicks)

        m = self.m
        mview = self.mview

        return {
            'm': self.m,
            'mview': self.mview,
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
                    'name': 'List Maps',
                    'link': '/listMaps',
                    'description': 'List the maps.',
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


    ########################################################################
    # Micropolis Stuff


    def initMicropolis(self):

        self.sessions = {}

        m = micropolisutils.NiceMicropolis()
        self.m = m
        print "Created Micropolis simulator engine:", m

        m.ResourceDir = MicropolisCorePath + '/res'
        m.InitGame()

        # Load a city file.
        cityFileName = MicropolisCorePath + '/cities/haight.cty'
        print "Loading city file:", cityFileName
        m.loadFile(cityFileName)

        # Initialize the simulator engine.

        m.Resume()
        m.setSpeed(2)
        m.setSkips(500)
        m.SetFunds(1000000000)
        m.autoGo = 0
        m.CityTax = 10
        m.NoDisasters = 1

        mview = micropolisutils.MicropolisView(m)
        self.mview = mview

        tengine = tileengine.TileEngine()
        self.tengine = tengine

        tengine.setBuffer(m.getMapBuffer())
        tengine.width = micropolisengine.WORLD_X
        tengine.height = micropolisengine.WORLD_Y

        # Unsigned short tile values, in column major order.
        tengine.typeCode = 'H'
        tengine.colBytes = 2 * micropolisengine.WORLD_Y
        tengine.rowBytes = 2
        tengine.tileMask = micropolisengine.LOMASK

        self.tileCount = 960
        self.tileSize = MicropolisTileSize

        self.tilesSurface = cairo.ImageSurface.create_from_png(MicropolisTilesPath)
        self.tilesWidth = self.tilesSurface.get_width()
        self.tilesHeight = self.tilesSurface.get_height()
        self.tilesCols = int(math.floor(self.tilesWidth / self.tileSize))
        self.tilesRows = int(math.floor(self.tilesHeight / self.tileSize))

        tileMap = array.array('i')
        self.tileMap = tileMap
        for i in range(0, self.tileCount):
            tileMap.append(i)


    def getSession(self, sessionID):
        sessions = self.sessions
        session = sessions.get(sessionID, None)
        if session:
            return session
        session = Session(sessionID)
        sessions[sessionID] = session
        return session


    def draw(
        self,
        widget=None,
        event=None):

        ctxWindow = self.window.cairo_create()

        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        self.loadGraphics(ctxWindow)

        windowBuffer = self.windowBuffer
        windowBufferWidth = self.windowBufferWidth
        windowBufferHeight = self.windowBufferHeight
        
        slop = 4

        if ((not windowBuffer) or
            (windowBufferWidth < winWidth) or
            (windowBufferHeight < winHeight)):

            nativeTarget = ctxWindow.get_target()
            windowBufferWidth = max(windowBufferWidth, winWidth + slop)
            windowBufferHeight = max(windowBufferHeight, winHeight + slop)

            #print "Window Buffer growing from", self.windowBufferWidth, self.windowBufferHeight, "to", windowBufferWidth, windowBufferHeight

            windowBuffer = nativeTarget.create_similar(cairo.CONTENT_COLOR, windowBufferWidth, windowBufferHeight)

            self.windowBuffer = windowBuffer
            self.windowBufferWidth = windowBufferWidth
            self.windowBufferHeight = windowBufferHeight

        ctxWindowBuffer = cairo.Context(windowBuffer)
        ctxWindowBuffer.set_antialias(cairo.ANTIALIAS_NONE)

        panX = self.panX
        panY = self.panY
        worldCols = self.worldCols
        worldRows = self.worldRows
        scale = self.scale
        tileSize = self.tileSize

        worldWidth = worldCols * tileSize
        worldHeight = worldRows * tileSize
        worldRight = panX + worldWidth
        worldBottom = panY + worldHeight

        #print "WORLDRIGHT", worldRight, "WORLDBOTTOM", worldBottom

        worldLeftClipped = max(0, panX)
        worldTopClipped = max(0, panY)
        worldRightClipped = min(worldWidth + panX, winWidth)
        worldBottomClipped = min(worldHeight + panY, winHeight)

        colLeft = \
            int(max(0,
                    math.floor(float(0 - panX) /
                               float(tileSize))))

        colRight = \
            int(min(worldCols,
                    math.ceil(float(winWidth - panX) /
                              float(tileSize))))

        #print "COLLEFT", colLeft, "COLRIGHT", colRight, "winWidth - panX", winWidth - panX

        rowTop = \
            int(max(0,
                    math.floor(float(0 - panY) /
                               float(tileSize))))

        rowBottom = \
            int(min(worldRows,
                    math.ceil(float(winHeight - panY) /
                              float(tileSize))))

        #print "ROWTOP", rowTop, "ROWBOTTOM", rowBottom, "winHeight - panY", winHeight - panY

        renderCols = colRight - colLeft
        renderRows = rowBottom - rowTop

        #print "PANX", panX, "PANY", panY, "TILESIZE", tileSize, "COLS", renderCols, "ROWS", renderRows

        renderX = panX + (colLeft * tileSize)
        renderY = panY + (rowTop * tileSize)
        renderWidth = renderCols * tileSize
        renderHeight = renderRows * tileSize

        self.renderCol = colLeft
        self.renderRow = rowTop
        self.renderCols = renderCols
        self.renderRows = renderRows

        # Make the off-screen buffer to draw the tiles into.

        buffer = self.buffer
        bufferWidth = self.bufferWidth
        bufferHeight = self.bufferHeight
        if ((not buffer) or
            (bufferWidth < renderWidth) or
            (bufferHeight < renderHeight)):

            nativeTarget = ctxWindow.get_target()
            bufferWidth = max(bufferWidth, renderWidth + slop)
            bufferHeight = max(bufferHeight, renderHeight + slop)

            #print "Buffer growing from", self.bufferWidth, self.bufferHeight, "to", bufferWidth, bufferHeight

            buffer = nativeTarget.create_similar(cairo.CONTENT_COLOR, bufferWidth, bufferHeight)

            self.buffer = buffer
            self.bufferWidth = bufferWidth
            self.bufferHeight = bufferHeight

        ctx = cairo.Context(buffer)

        if ((renderCols > 0) and
            (renderRows > 0)):

            #print "renderTilesLazy BEGIN", self.generateTile
            self.tengine.renderTilesLazy(
                ctx,
                self.tileFunction,
                self.tileMap,
                self.tileSize,
                self.renderCol,
                self.renderRow,
                renderCols,
                renderRows,
                1.0,
                self.generateTile,
                self.tileCache,
                self.tileCacheSurfaces,
                self.tileState)
            #print "renderTilesLazy END"

        ctxWindowBuffer.save()

        ctxWindowBuffer.set_source_surface(
            buffer,
            renderX,
            renderY)

        ctxWindowBuffer.rectangle(
            renderX,
            renderY,
            renderWidth,
            renderHeight)

        ctxWindowBuffer.clip()

        ctxWindowBuffer.paint()

        ctxWindowBuffer.restore()

        # Draw the background outside of the tile world.
        
        backgroundVisible = False

        # Left Background
        if panX > 0:
            backgroundVisible = True
            ctxWindowBuffer.rectangle(
                0,
                worldTopClipped,
                panX,
                worldBottomClipped - worldTopClipped)

        # Right Background
        if worldRight < winWidth:
            backgroundVisible = True
            ctxWindowBuffer.rectangle(
                worldRight,
                worldTopClipped,
                winWidth - worldRight,
                worldBottomClipped - worldTopClipped)

        # Top Background
        if panY > 0:
            backgroundVisible = True
            ctxWindowBuffer.rectangle(
                0,
                0,
                winWidth,
                panY)

        # Bottom Background
        if worldBottom < winHeight:
            backgroundVisible = True
            ctxWindowBuffer.rectangle(
                0,
                worldBottom,
                winWidth,
                winHeight - worldBottom)

        if backgroundVisible:
            ctxWindowBuffer.set_source_rgb(*self.outsideBackgroundColor)
            ctxWindowBuffer.fill()

        self.drawOverlays(ctxWindowBuffer)

        ctxWindow.set_source_surface(
            windowBuffer,
            0,
            0)

        ctxWindow.paint()


    @expose(
        template="micropolis.templates.micropolisSessionStart",
        content_type="text/xml")
    def micropolisSessionStart(
        self):
        session = self.getSession(UniqueID())
        return {
            'session': session,
        }


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
        cols=micropolisengine.WORLD_X,
        rows=micropolisengine.WORLD_Y,
        code=0,
        **kw):

        if ((col < 0) or
            (row < 0) or
            (cols <= 0) or
            (rows <= 0) or
            ((col + cols) > micropolisengine.WORLD_X) or
            ((row + rows) > micropolisengine.WORLD_Y)):
            self.expectationFailed("Invalid tile coordinates.");

        self.tickSim(1)

        tiles = self.tengine.getTileData(None, None, col, row, cols, rows, code)
        #print "TILES", tiles

        return tiles


    @expose(
        template="micropolis.templates.micropolisTick",
        content_type="text/xml")
    @validate(validators = {
        'col': validators.Int(),
        'row': validators.Int(),
        'cols': validators.Int(),
        'rows': validators.Int(),
        'ref': validators.Int(),
    })
    def micropolisTick(
        self,
        col=0,
        row=0,
        cols=micropolisengine.WORLD_X,
        rows=micropolisengine.WORLD_Y,
        ref=0,
        sessionID='',
        body='',
        **kw):

        global request

        method = request.method
        if method != 'POST':
            self.expectationFailed("Expected post.");

        def eatTag(s, tag):
            print "EATTAG BEGIN", s, tag
            for kind, data, pos in s:
                print "EAT", kind, data, pos
                if ((kind == s.END) and
                    (data[0] == tag)):
                    return
                if (kind == s.START):
                    eatTag(s, data[0])
            print "EATTAG END"

        print "BODY", body

        try:
            s = XML(body)
        except Exception, e:
            self.expectationFailed("Error parsing XML body: " + str(e))

        for kind, data, pos in s.select('commands/command'):
            if kind == s.START:
                if data[0].localname == 'command':
                    params = {}
                    for attName, attValue in data[1]:
                        params[attName.localname] = attValue
                    self.doCommand(params)

        if ((col < 0) or
            (row < 0) or
            (cols <= 0) or
            (rows <= 0) or
            ((col + cols) > micropolisengine.WORLD_X) or
            ((row + rows) > micropolisengine.WORLD_Y) or
            (not sessionID)):
            self.expectationFailed("Invalid parameters.");

        session = self.getSession(sessionID)

        self.tickSim(1, False)

        code = 3
        tileViewCache = session.tileViewCache
        tiles = self.tengine.getTileData(None, AniTileMap, col, row, cols, rows, code, tileViewCache)
        print "TILES", tiles

        format = 1

        messages = [
            {
                'name': 'test',
                'body': 'Test Body',
            },
        ]

        return {
            'col': col,
            'row': row,
            'cols': cols,
            'rows': rows,
            'ref': ref,
            'format': format,
            'tiles': tiles,
            'messages': messages,
        }


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
        cols=micropolisengine.WORLD_X,
        rows=micropolisengine.WORLD_Y,
        ticks=DefaultTicks,
        **kw):

        if ((col < 0) or
            (row < 0) or
            (cols <= 0) or
            (rows <= 0) or
            ((col + cols) > micropolisengine.WORLD_X) or
            ((row + rows) > micropolisengine.WORLD_Y)):
            self.expectationFailed("Invalid tile coordinates.");

        self.tickSim(ticks, True)

        tileSize = 16
        width = cols * tileSize
        height = rows * tileSize

        surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
        ctx = cairo.Context(surface)

        tengine = self.tengine

        alpha = 1.0

        tengine.renderTiles(
            ctx,
            self.tilesSurface,
            self.tilesWidth,
            self.tilesHeight,
            None,
            self.tileMap,
            self.tileSize,
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
        cols=micropolisengine.WORLD_X / 4,
        rows=micropolisengine.WORLD_Y / 4,
        ticks=DefaultTicks,
        **kw):

        if ((col < 0) or
            (row < 0) or
            (cols <= 0) or
            (rows <= 0) or
            ((col + cols) > micropolisengine.WORLD_X) or
            ((row + rows) > micropolisengine.WORLD_Y) or
            (ticks < 0) or
            (ticks > 1000000)):
            self.expectationFailed("Invalid parameter.");

        self.handleCommands(kw)

        self.tickSim(ticks, True)

        m = self.m
        mview = self.mview

        return {
            'col': col,
            'row': row,
            'cols': cols,
            'rows': rows,
            'ticks': ticks,
            'm': self.m,
            'mview': self.mview,
            'CityNames': CityNames,
        }


    def handleCommands(self, kw):

        m = self.m

        command = kw.get('command', None)
        if not command:
            return None

        print "COMMAND", command
        
        if command == 'LoadCity':

            cityName = kw.get('city', None)
            print "LoadCity", cityName
            if cityName not in CityNames:
                return None
            cityFileName = CityNames[cityName]
            print "cityFileName", cityFileName
            m.loadFile(cityFileName)

        elif command == 'Disaster':

            disaster = kw.get('disaster', None)
            print "DISASTER", disaster
            if disaster == 'Earthquake':
                print "EARTHQUAKE"
                m.MakeEarthquake()
            elif disaster == 'Flood':
                print "FLOOD"
                m.MakeFlood()
            elif disaster == 'Meltdown':
                print "MELTDOWN"
                m.MakeMeltdown()
            elif disaster == 'BailOut':
                print "BailOut"
                m.SetFunds(7000000)
            else:
                return None

        elif command == 'SetTaxRate':

            rateStr = kw.get('rate', None)
            rate = m.CityTax
            try:
                rate = int(rateStr)
            except: pass
            rate = max(0, min(rate, 20))
            m.CityTax = rate


    def tickSim(self, ticks=1, animateTiles=True):

        m = self.m
        
        now = time.time()
        fracTime = now - math.floor(now)

        m.flagBlink = fracTime < 0.5

        m.Resume()
        
        lastSkips = m.sim_skips
        m.setSkips(ticks - 1)
        #print "TICK", ticks
        print "CityTime", m.CityTime, "CityMonth", m.CityMonth, "CityYear", m.CityYear, 
        #print "sim_paused", m.sim_paused, "sim_skips", m.sim_skips, "sim_skip", m.sim_skip
        m.sim_tick()
        m.setSkips(lastSkips)

        if animateTiles:
            m.animateTiles()
        

    def doCommand(self, params):
        m = self.m
        command = params.get('name', None)
        print "DOCOMMAND", command, params

        if command == 'disaster':

            disaster = params.get('disaster', None)
            print "disaster", disaster

            if disaster == 'monster':
                m.MakeMonster()
            elif disaster == 'fire':
                m.SetFire()
            elif disaster == 'flood':
                m.MakeFlood()
            elif disaster == 'meltdown':
                m.MakeMeltdown()
            elif disaster == 'tornado':
                m.MakeTornado()
            elif disaster == 'earthquake':
                m.MakeEarthquake()

        elif command == 'setTaxRate':

            rateStr = params.get('rate', m.CityTax)
            rate = m.CityTax
            print "setTaxRate", rate
            try:
                rate = int(rateStr)
            except: pass
            if (rate >= 0) and (rate <= 20):
                m.CityTax = rate

        elif command == 'loadCity':

            cityName = params.get('city', None)
            print "LoadCity", cityName
            if cityName in CityNames:
                cityFileName = CityNames[cityName]
                print "cityFileName", cityFileName
                m.loadFile(cityFileName)

        elif command == 'loadScenario':

            scenarioStr = params.get('scenario', None)
            print "LoadScenario", scenarioStr
            scenario = 0
            try:
                scenario = int(scenarioStr)
            except: pass
            if scenario:
                m.LoadScenario(scenario)


########################################################################
