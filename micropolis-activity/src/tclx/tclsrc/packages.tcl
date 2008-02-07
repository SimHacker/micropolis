#
# packages.tcl --
#
# Command to retrieve a list of packages or information about the packages.
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
# $Id: packages.tcl,v 2.0 1992/10/16 04:52:02 markd Rel $
#------------------------------------------------------------------------------
#

#@package: TclX-packages packages autoprocs

proc packages {{option {}}} {
    global TCLENV
    set packList {}
    foreach key [array names TCLENV] {
        if {[string match "PKG:*" $key]} {
            lappend packList [string range $key 4 end]
        }
    }
    if [lempty $option] {
        return $packList
    } else {
        if {$option != "-location"} {
            error "Unknow option \"$option\", expected \"-location\""
        }
        set locList {}
        foreach pack $packList {
            set fileId [lindex $TCLENV(PKG:$pack) 0]
            
            lappend locList [list $pack [concat $TCLENV($fileId) \
                                             [lrange $TCLENV(PKG:$pack) 1 2]]]
        }
        return $locList
    }
}

proc autoprocs {} {
    global TCLENV
    set procList {}
    foreach key [array names TCLENV] {
        if {[string match "PROC:*" $key]} {
            lappend procList [string range $key 5 end]
        }
    }
    return $procList
}
