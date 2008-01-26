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

# $tk_priv(window@$screen) keeps track of the button containing the mouse, 
# and $tk_priv(relief@$screen) saves the original relief of the button so 
# it can be restored when the mouse button is released.

# The procedure below is invoked when the mouse pointer enters a
# button widget.  It records the button we're in and changes the
# state of the button to active unless the button is disabled.

proc tk_butEnter w {
    global tk_priv
    set screen [winfo screen $w]
    if {[lindex [$w config -state] 4] != "disabled"} {
	$w config -state active
	set tk_priv(window@$screen) $w
    } else {
	set tk_priv(window@$screen) ""
    }
}

# The procedure below is invoked when the mouse pointer leaves a
# button widget.  It changes the state of the button back to
# inactive.

proc tk_butLeave w {
    global tk_priv
    if {[lindex [$w config -state] 4] != "disabled"} {
	$w config -state normal
    }
    set screen [winfo screen $w]
    set tk_priv(window@$screen) ""
}

# The procedure below is invoked when the mouse button is pressed in
# a button/radiobutton/checkbutton widget.  It records information
# (a) to indicate that the mouse is in the button, and
# (b) to save the button's relief so it can be restored later.

proc tk_butDown w {
    global tk_priv
    set screen [winfo screen $w]
    set tk_priv(relief@$screen) [lindex [$w config -relief] 4]
    if {[lindex [$w config -state] 4] != "disabled"} {
	$w config -relief sunken
	update idletasks
    }
}

# The procedure below is invoked when the mouse button is released
# for a button/radiobutton/checkbutton widget.  It restores the
# button's relief and invokes the command as long as the mouse
# hasn't left the button.

proc tk_butUp w {
    global tk_priv
    set screen [winfo screen $w]
    $w config -relief $tk_priv(relief@$screen)
    update idletasks
    if {($w == $tk_priv(window@$screen))
	    && ([lindex [$w config -state] 4] != "disabled")} {
	uplevel #0 [list $w invoke]
    }
}
