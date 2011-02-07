#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Start script for the micropolis TurboGears project.

This script is only needed during development for running from the project
directory. When the project is installed, easy_install will create a
proper start script.
"""

import sys, os
from micropolis.commands import start, ConfigurationError

#print "Redirecting stderr to /dev/null ..."
#sys.stderr = open('/dev/null', 'w')

import os, sys

MicropolisDir = os.path.normpath(
    os.path.join(
        os.getcwd(),
        '../MicropolisCore/src'))
if MicropolisDir not in sys.path:
    sys.path.append(MicropolisDir)

if __name__ == "__main__":
    try:
        start()
    except ConfigurationError, exc:
        sys.stderr.write(str(exc))
        sys.exit(1)
