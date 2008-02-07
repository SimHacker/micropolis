#
# globrecur.tcl --
#
#  Build up a directory list recursively.
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
# $Id: globrecur.tcl,v 2.0 1992/10/16 04:52:04 markd Rel $
#------------------------------------------------------------------------------
#

#@package: TclX-globrecur recursive_glob

proc recursive_glob {globlist} {
    set result ""
    foreach pattern $globlist {
        foreach file [glob -nocomplain $pattern] {
            lappend result $file
            if [file isdirectory $file] {
                set result [concat $result [recursive_glob $file/*]]
            }
        }
    }
    return $result
}
