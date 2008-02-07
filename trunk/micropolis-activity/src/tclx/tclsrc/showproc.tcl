#
# showprocs.tcl --
#
# Display procedure headers and bodies.
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
# $Id: showprocs.tcl,v 2.0 1992/10/16 04:52:11 markd Rel $
#------------------------------------------------------------------------------
#

#@package: TclX-show_procedures showproc showprocs

proc showproc {procname} {
    if [lempty [info procs $procname]] {demand_load $procname}
	set arglist [info args $procname]
	set nargs {}
	while {[llength $arglist] > 0} {
	    set varg [lvarpop arglist 0]
	    if [info default $procname $varg defarg] {
		lappend nargs [list $varg $defarg]
	    } else {
		lappend nargs $varg
	    }
    }
    format "proc %s \{%s\} \{%s\}\n" $procname $nargs [info body $procname]
}

proc showprocs {args} {
    if [lempty $args] { set args [info procs] }
    set out ""

    foreach i $args {
	foreach j $i { append out [showproc $j] "\n"}
    }
    return $out
}

