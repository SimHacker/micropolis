# -*- mode: python; tab-width: 4 -*-
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

import gtk
import os
import signal
import tempfile
import logging
import sys
import time
import subprocess
import thread
import fcntl

from sugar.activity import activity
from sugar.activity.activity import get_bundle_path
from sugar import profile
from gettext import gettext as _
from glob import glob

try:
    import pygame.mixer
    pygame.mixer.init()
except: pass

try:
    from sugar.presence import presenceservice
except ImportError:
    from sugar.presence import PresenceService as presenceservice


def QuoteTCL(s):
    return s.replace('"', '\\"')


class MicropolisActivity(activity.Activity):

    def __init__(self, handle):

        activity.Activity.__init__(self, handle)
    
        self.set_title(_('Micropolis Activity'))
        self.connect('destroy', self._destroy_cb)
        self.connect('focus-in-event', self._focus_in_cb)
        self.connect('focus-out-event', self._focus_out_cb)

        signal.signal(signal.SIGCHLD, self._sigchild_handler)

        self._bundle_path = get_bundle_path()

        if False:
            # FIXME: Plug Micropolis's window into a gtk socket.
            # Doesn't work yet, but it would be cool if it did. 
            socket = gtk.Socket()
            try:
                self.set_canvas(socket)
            except AttributeError:
                self.add(socket)
            socket.show()
            socket.connect('plug-added', self._plug_added_cb)
            socket.connect('plug-removed', self._plug_removed_cb)

            win = socket.get_id()

        command = os.path.join(
            self._bundle_path,
            'Micropolis')

        args = [
            command,
            #'-R', str(win), # Set root window to socket window id
            '-t', # Interactive tty mode, so we can send it commands.
        ]

        logging.debug("CWD: " + self._bundle_path)
        logging.debug("Micropolis ARGS: " + repr(args))

        self._process = subprocess.Popen(
            args,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            close_fds=True,
            cwd=self._bundle_path,
            preexec_fn=lambda: os.chdir(self._bundle_path))

        logging.debug("STARTING THREAD... " + str(self._stdout_thread_function))
        t = None
        try:
            t = thread.start_new(
                self._stdout_thread_function,
                ())
        except Exception, e:
            logging.debug("EXCEPTION " + str(e))
        self._stdout_thread = t
        logging.debug("STARTED THREAD. " + str(t))

        uri = handle.uri or ''
        logging.debug("Micropolis SUGARSTARTUP URI " + repr(uri))
        self.send_process(
            'SugarStartUp "' + QuoteTCL(uri) + '"\n')

        nick = profile.get_nick_name() or ''
        logging.debug("Micropolis SUGARNICKNAME NICK " + repr(nick))
        self.send_process(
            'SugarNickName "' + QuoteTCL(nick) + '"\n')

        #logging.debug("started Micropolis, pid " + repr(self._pid))

        ps = presenceservice.get_instance()

        for buddy in ps.get_buddies():
            self._buddy_appeared_cb(ps, buddy)

        ps.connect("buddy-appeared", self._buddy_appeared_cb)
        ps.connect("buddy-disappeared", self._buddy_disappeared_cb)


    def _stdout_thread_function(self, *args, **keys):
        logging.debug("_stdout_thread_function BEGIN " + repr(args) + " " + repr(keys))
        f = self._process.stdout
        fcntl.fcntl(f.fileno(), fcntl.F_SETFD, 0)
        while True:
            line = 'XXX'
            try:
                line = f.readline()
            except Exception, e:
                logging.debug("READLINE EXCEPTION " + str(e))
                break
            logging.debug("LINE: " + repr(line))
            line = line.strip()
            if not line:
                continue
            words = line.strip().split(' ')
            command = words[0]
            if command == 'PlaySound':
                logging.debug("PLAYSOUND " + " ".join(words[1:]))
                self.play_sound(words[1])
            else:
                pass # logging.debug(">>> " + line)
        logging.debug("_stdout_thread_function END")


    def play_sound(self, name):
        fileName = os.path.join(
            self._bundle_path,
            'res/sounds',
            name.lower() + '.wav')
        print "PLAY_SOUND " + fileName
        try:
            sound = pygame.mixer.Sound(fileName)
            sound.play()
        except Exception, e:
            print "Can't play sound: " + fileName + " " + str(e)
            pass


    def send_process(self, message):
        logging.debug("SEND_PROCESS " + message)
        self._process.stdin.write(message)


    def share(self):
        logging.debug("SHARE")
        Activity.share(self)
        self.send_process(
            'SugarShare\n')


    def quit_process(self):
        logging.debug("QUIT_PROCESS")
        self.send_process(
            'SugarQuit\n')
        time.sleep(10)


    def _plug_added_cb(self, sock):
        logging.debug("Micropolis window opened")
        return False


    def _plug_removed_cb(self, sock):
        logging.debug("Micropolis window closed")
        self.destroy()
        return False

    
    def _destroy_cb(self, window):
        logging.debug("Micropolis activity destroyed %r" % window)
        self.quit_process()

        
    def _focus_in_cb(self, window, event):
        logging.debug("Micropolis activated %r %r" % (window, event))
        self.send_process(
            'SugarActivate\n')


    def _focus_out_cb(self, window, event):
        logging.debug("Micropolis deactivated %r %r" % (window, event))
        self.send_process(
            'SugarDeactivate\n')


    def _buddy_appeared_cb(self, ps, buddy):

        try:
            key = buddy.props.key or ''
            nick = buddy.props.nick or ''
            color = buddy.props.color or ''
            address = buddy.props.ip4_address or ''
        except AttributeError:
            key = buddy.get_name() or ''
            nick = buddy.get_name() or ''
            color = buddy.get_color() or ''
            address = buddy.get_ip4_address() or ''

        logging.debug("Micropolis _BUDDY_APPEARED_CB KEY " + repr(key) + " NICK " + repr(nick) + " COLOR " + repr(color) + " ADDRESS " + repr(address))

        logging.debug("Buddy appeared " + repr(buddy.props.nick))

        self.send_process(
            'SugarBuddyAdd "' +
            QuoteTCL(key) + '" "' +
            QuoteTCL(nick) + '" "' +
            QuoteTCL(color) + '" "' +
            QuoteTCL(address) + '"\n')

    def _buddy_disappeared_cb(self, ps, buddy):

        try:
            key = buddy.props.key or ''
            nick = buddy.props.nick or ''
            color = buddy.props.color or ''
            address = buddy.props.ip4_address or ''
        except AttributeError:
            key = buddy.get_name() or ''
            nick = buddy.get_name() or ''
            color = buddy.get_color() or ''
            address = buddy.get_ip4_address() or ''

        logging.debug("Micropolis _BUDDY_DISAPPEARED_CB KEY " + repr(key) + " NICK " + repr(nick) + " COLOR " + repr(color) + " ADDRESS " + repr(address))

        logging.debug("Buddy disappeared " + repr(buddy.props.nick))

        self.send_process(
            'SugarBuddyDel "' +
            QuoteTCL(key) + '" "' +
            QuoteTCL(nick) + '" "' +
            QuoteTCL(color) + '" "' +
            QuoteTCL(address) + '"\n')

    def _sigchild_handler(self, signum, frame):
        logging.debug("got signal %i %r %r" % (signum, frame, self._process))
        sys.exit(0)

