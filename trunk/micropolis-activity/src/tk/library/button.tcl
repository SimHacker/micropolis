# button.tcl --
#
# This file contains Tcl procedures used to manage Tk buttons.
#
# $Header: /user6/ouster/wish/scripts/RCS/button.tcl,v 1.7 92/07/28 15:41:13 ouster Exp $ SPRITE (Berkeley)
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

# The procedure below is invoked when the mouse pointer enters a
# button widget.  It records the button we're in and changes the
# state of the button to active unless the button is disabled.

proc tk_butEnter w {
    global tk_priv tk_strictMotif
    if {[lindex [$w config -state] 4] != "disabled"} {
	if {!$tk_strictMotif} {
	    $w config -state active
	}
	set tk_priv(window) $w
    }
}

# The procedure below is invoked when the mouse pointer leaves a
# button widget.  It changes the state of the button back to
# inactive.

proc tk_butLeave w {
    global tk_priv tk_strictMotif
    if {[lindex [$w config -state] 4] != "disabled"} {
	if {!$tk_strictMotif} {
	    $w config -state normal
	}
    }
    set tk_priv(window) ""
}

# The procedure below is invoked when the mouse button is pressed in
# a button/radiobutton/checkbutton widget.  It records information
# (a) to indicate that the mouse is in the button, and
# (b) to save the button's relief so it can be restored later.

proc tk_butDown w {
    global tk_priv
    set tk_priv(relief) [lindex [$w config -relief] 4]
    if {[lindex [$w config -state] 4] != "disabled"} {
	$w config -relief sunken
    }
}

# The procedure below is invoked when the mouse button is released
# for a button/radiobutton/checkbutton widget.  It restores the
# button's relief and invokes the command as long as the mouse
# hasn't left the button.

proc tk_butUp w {
    global tk_priv
    $w config -relief $tk_priv(relief)
    if {($w == $tk_priv(window))
	    && ([lindex [$w config -state] 4] != "disabled")} {
	uplevel #0 [list $w invoke]
    }
}
