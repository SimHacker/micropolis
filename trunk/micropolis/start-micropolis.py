#!/usr/bin/env python2.5
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

sys.path.append(
    os.path.normpath(
        os.path.join(
            os.getcwd(),
            '../MicropolisCore/src')))

if __name__ == "__main__":
    try:
        start()
    except ConfigurationError, exc:
        sys.stderr.write(str(exc))
        sys.exit(1)
