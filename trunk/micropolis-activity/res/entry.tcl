# entry.tcl --
#
# This file contains Tcl procedures used to manage Tk entries.
#
# $Header: /user6/ouster/wish/scripts/RCS/entry.tcl,v 1.2 92/05/23 16:40:57 ouster Exp $ SPRITE (Berkeley)
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

# The procedure below is invoked to backspace over one character
# in an entry widget.  The name of the widget is passed as argument.

proc tk_entryBackspace w {
    set x [expr {[$w index cursor] - 1}]
    if {$x != -1} {$w delete $x}
}

# The procedure below is invoked to backspace over one word in an
# entry widget.  The name of the widget is passed as argument.

proc tk_entryBackword w {
    set string [$w get]
    set curs [expr [$w index cursor]-1]
    if {$curs < 0} return
    for {set x $curs} {$x > 0} {incr x -1} {
	if {([string first [string index $string $x] " \t"] < 0)
		&& ([string first [string index $string [expr $x-1]] " \t"]
		>= 0)} {
	    break
	}
    }
    $w delete $x $curs
}

# The procedure below is invoked after insertions.  If the caret is not
# visible in the window then the procedure adjusts the entry's view to
# bring the caret back into the window again.

proc tk_entrySeeCaret w {
    set c [$w index cursor]
    set left [$w index @0]
    if {$left > $c} {
	$w view $c
	return
    }
    while {[$w index @[expr [winfo width $w]-5]] < $c} {
	set left [expr $left+1]
	$w view $left
    }
}

proc tk_entryCopyPress {w} {
  set sel ""
  catch {set sel [selection -window $w get]}
  $w insert cursor $sel
  tk_entrySeeCaret $w
}

proc tk_entryCutPress {w} {
  catch {$w delete sel.first sel.last}
  tk_entrySeeCaret $w
}

proc tk_entryDelLine {w} {
  $w delete 0 end
}

proc tk_entryDelPress {w} {
  tk_entryBackspace $w
  tk_entrySeeCaret $w
}
