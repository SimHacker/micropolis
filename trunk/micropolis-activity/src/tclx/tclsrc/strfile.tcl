#
# string_file --
#
# Functions to read and write strings from a file that has not been opened.
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
# $Id: stringfile.tcl,v 2.0 1992/10/16 04:52:13 markd Rel $
#------------------------------------------------------------------------------
#

#@package: TclX-stringfile_functions read_file write_file

proc read_file {fileName {numBytes {}}} {
    set fp [open $fileName]
    if {$numBytes != ""} {
        set result [read $fp $numBytes]
    } else {
        set result [read $fp]
    }
    close $fp
    return $result
} 

proc write_file {fileName args} {
    set fp [open $fileName w]
    foreach string $args {
        puts $fp $string
    }
    close $fp
}

