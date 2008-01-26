#
# buildidx.tcl --
#
# Code to build Tcl package library. Defines the proc `buildpackageindex'.
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
# $Id: buildidx.tcl,v 2.0 1992/10/16 04:51:38 markd Rel $
#------------------------------------------------------------------------------
#

proc TCHSH:PutLibLine {outfp package where endwhere autoprocs} {
    puts $outfp [concat $package $where [expr {$endwhere - $where - 1}] \
                        $autoprocs]
}

proc TCLSH:CreateLibIndex {libName} {

    if {[file extension $libName] != ".tlb"} {
        error "Package library `$libName' does not have the extension `.tlb'"}
    set idxName "[file root $libName].tndx"

    unlink -nocomplain $idxName
    set libFH [open $libName r]
    set idxFH [open $idxName w]

    set contectHdl [scancontext create]

    scanmatch $contectHdl "^#@package: " {
        set size [llength $matchInfo(line)]
        if {$size < 2} {
            error [format "invalid package header \"%s\"" $matchInfo(line)]
        }
        if $inPackage {
            TCHSH:PutLibLine $idxFH $pkgDefName $pkgDefWhere \
                             $matchInfo(offset) $pkgDefProcs
        }
        set pkgDefName   [lindex $matchInfo(line) 1]
        set pkgDefWhere  [tell $matchInfo(handle)]
        set pkgDefProcs  [lrange $matchInfo(line) 2 end]
        set inPackage 1
    }

    scanmatch $contectHdl "^#@packend" {
        if !$inPackage {
            error "#@packend without #@package in $libName
        }
        TCHSH:PutLibLine $idxFH $pkgDefName $pkgDefWhere $matchInfo(offset) \
                         $pkgDefProcs
        set inPackage 0
    }

    set inPackage 0
    if {[catch {
        scanfile $contectHdl $libFH
       } msg] != 0} {
       global errorInfo errorCode
       close libFH
       close idxFH
       error $msg $errorInfo $errorCode
    }
    if {![info exists pkgDefName]} {
        error "No #@package definitions found in $libName"
    }
    if $inPackage {
        TCHSH:PutLibLine $idxFH $pkgDefName $pkgDefWhere [tell $libFH] \
                         $pkgDefProcs
    }
    close $libFH
    close $idxFH
    
    scancontext delete $contectHdl

    # Set mode and ownership of the index to be the same as the library.

    file stat $libName statInfo
    chmod $statInfo(mode) $idxName
    chown [list $statInfo(uid) $statInfo(gid)] $idxName

}

proc buildpackageindex {libfile} {

    set status [catch {TCLSH:CreateLibIndex $libfile} errmsg]
    if {$status != 0} {
        global errorInfo errorCode
        error "building package index for `$libfile' failed: $errmsg" \
              $errorInfo $errorCode
    }
}

