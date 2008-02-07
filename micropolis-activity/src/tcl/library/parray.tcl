# parray:
# Print the contents of a global array on stdout.
#
# $Header: /sprite/src/lib/tcl/scripts/RCS/parray.tcl,v 1.1 91/09/26 09:55:03 ouster Exp $ SPRITE (Berkeley)
#
# Copyright 1991 Regents of the University of California
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and without
# fee is hereby granted, provided that this copyright
# notice appears in all copies.  The University of California
# makes no representations about the suitability of this
# software for any purpose.  It is provided "as is" without
# express or implied warranty.
#

proc parray a {
    global $a
    set maxl 0
    foreach name [lsort [array names $a]] {
	if {[string length $name] > $maxl} {
	    set maxl [string length $name]
	}
    }
    set maxl [expr {$maxl + [string length $a] + 2}]
    foreach name [lsort [array names $a]] {
	set nameString [format %s(%s) $a $name]
	puts stdout [format "%-*s = %s" $maxl $nameString [set ${a}($name)]]
    }
}
