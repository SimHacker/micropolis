#
# compat --
#
# This file provides commands compatible with older versions of Extended Tcl.
# 
#------------------------------------------------------------------------------
# Copyright 1992 Karl Lehenbauer and Mark Diekhans.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted, provided
# that the above copyright notice appear in all copies.  Karl Lehenbauer and
# Mark Diekhans make no representations about the suitability of this
# software for any purpose.  It is provided "as is" without express or
# implied warranty.
#------------------------------------------------------------------------------
# $Id: compat.tcl,v 2.0 1992/10/16 04:51:51 markd Rel $
#------------------------------------------------------------------------------
#

#@package: TclX-Compatibility execvp

proc execvp {progname args} {
    execl $progname $args
}
