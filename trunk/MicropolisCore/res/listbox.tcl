# listbox.tcl --
#
# This file contains Tcl procedures used to manage Tk listboxes.
#
# $Header: /user6/ouster/wish/scripts/RCS/listbox.tcl,v 1.2 92/06/03 15:21:28 ouster Exp $ SPRITE (Berkeley)
#
# Copyright 1992 Regents of the University of California
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and without
# fee is hereby granted, provided that this copyright
# notice appears in all copies.  The University of California
# makes no representations about the suitability of this
# software for any purpose.  It is provided "as is" without
# express or implied warranty.
#

# The procedure below may be invoked to change the behavior of
# listboxes so that only a single item may be selected at once.
# The arguments give one or more windows whose behavior should
# be changed;  if one of the arguments is "Listbox" then the default
# behavior is changed for all listboxes.

proc tk_listboxSingleSelect args {
    foreach w $args {
	bind $w <B1-Motion> {%W select from [%W nearest %y]} 
	bind $w <Shift-1> {%W select from [%W nearest %y]}
	bind $w <Shift-B1-Motion> {%W select from [%W nearest %y]}
    }
}
