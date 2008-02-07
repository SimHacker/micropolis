#
# convlib.tcl --
#
#     Convert Ousterhout style tclIndex files and associated libraries to a
# package library.
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
# $Id: convlib.tcl,v 2.0 1992/10/16 04:51:53 markd Rel $
#------------------------------------------------------------------------------
#

#@package: TclX-convertlib convert_lib

proc convert_lib {tclIndex packageLib {ignore {}}} {
    if {[file tail $tclIndex] != "tclindex"} {
        error "Tail file name numt be `tclindex': $tclIndex"}
    set srcDir [file dirname $tclIndex]

    if {[file extension $packageLib] != ".tlib"} {
        append packageLib ".tlib"}

    # Build an array addressed by file name containing all of the procs
    # defined in that file.

    set tclIndexFH [open $tclIndex r]
    while {[gets $tclIndexFH line] >= 0} {
        if {([cindex $line 0] == "#") || ([llength $line] != 2)} {
            continue}
        if {[lsearch $ignore [lindex $line 1]] >= 0} {
            continue}
        lappend entryTable([lindex $line 1]) [lindex $line 0]
    }
    close $tclIndexFH

    set libFH [open $packageLib w]
    foreach srcFile [array names entryTable] {
        set srcFH [open $srcDir/$srcFile r]
        puts $libFH "#@package: $srcFile $entryTable($srcFile)\n"
        copyfile $srcFH $libFH
        close $srcFH
    }
    close $libFH
    buildpackageindex $packageLib
}
