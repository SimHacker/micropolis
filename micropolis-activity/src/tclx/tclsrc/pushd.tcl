#
# pushd.tcl --
#
# C-shell style directory stack procs.
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
# $Id: pushd.tcl,v 2.0 1992/10/16 04:52:06 markd Rel $
#------------------------------------------------------------------------------
#

#@package: TclX-directory_stack pushd popd dirs

global TCLENV(dirPushList)

set TCLENV(dirPushList) ""

proc pushd {args} {
    global TCLENV

    if {[llength $args] > 1} {
        error "bad # args: pushd [dir_to_cd_to]"
    }
    set TCLENV(dirPushList) [linsert $TCLENV(dirPushList) 0 [pwd]]

    if {[llength $args] != 0} {
        cd [glob $args]
    }
}

proc popd {} {
    global TCLENV

    if [llength $TCLENV(dirPushList)] {
        cd [lvarpop TCLENV(dirPushList)]
        pwd
    } else {
        error "directory stack empty"
    }
}

proc dirs {} { 
    global TCLENV
    echo [pwd] $TCLENV(dirPushList)
}
