#
# forrecur.tcl --
#
# Proc to execute code on every file in a recursive directory glob.
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
# $Id: forrecur.tcl,v 2.0 1992/10/16 04:52:00 markd Rel $
#------------------------------------------------------------------------------
#

#@package: TclX-forrecur for_recursive_glob

proc for_recursive_glob {var globlist code {depth 1}} {
    upvar $depth $var myVar
    foreach globpat $globlist {
        foreach file [glob -nocomplain $globpat] {
            if [file isdirectory $file] {
                for_recursive_glob $var $file/* $code [expr {$depth + 1}]
	    }
	    set myVar $file
	    uplevel $depth $code
        }
    }
}
