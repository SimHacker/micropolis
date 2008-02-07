# text.tcl --
#
# This file contains Tcl procedures used to manage Tk entries.
#
# $Header: /user6/ouster/wish/scripts/RCS/text.tcl,v 1.2 92/07/16 16:26:33 ouster Exp $ SPRITE (Berkeley)
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

# The procedure below is invoked when dragging one end of the selection.
# The arguments are the text window name and the index of the character
# that is to be the new end of the selection.

proc tk_textSelectTo {w index} {
    global tk_priv

    case $tk_priv(selectMode) {
	char {
	    if [$w compare $index < anchor] {
		set first $index
		set last anchor
	    } else {
		set first anchor
		set last [$w index $index+1c]
	    }
	}
	word {
	    if [$w compare $index < anchor] {
		set first [$w index "$index wordstart"]
		set last [$w index "anchor wordend"]
	    } else {
		set first [$w index "anchor wordstart"]
		set last [$w index "$index wordend"]
	    }
	}
	line {
	    if [$w compare $index < anchor] {
		set first [$w index "$index linestart"]
		set last [$w index "anchor lineend + 1c"]
	    } else {
		set first [$w index "anchor linestart"]
		set last [$w index "$index lineend + 1c"]
	    }
	}
    }
    $w tag remove sel 0.0 $first
    $w tag add sel $first $last
    $w tag remove sel $last end
}

# The procedure below is invoked to backspace over one character in
# a text widget.  The name of the widget is passed as argument.

proc tk_textBackspace w {
    $w delete insert-1c insert
}

# The procedure below compares three indices, a, b, and c.  Index b must
# be less than c.  The procedure returns 1 if a is closer to b than to c,
# and 0 otherwise.  The "w" argument is the name of the text widget in
# which to do the comparison.

proc tk_textIndexCloser {w a b c} {
    set a [$w index $a]
    set b [$w index $b]
    set c [$w index $c]
    if [$w compare $a <= $b] {
	return 1
    }
    if [$w compare $a >= $c] {
	return 0
    }
    scan $a "%d.%d" lineA chA
    scan $b "%d.%d" lineB chB
    scan $c "%d.%d" lineC chC
    if {$chC == 0} {
	incr lineC -1
	set chC [string length [$w get $lineC.0 $lineC.end]]
    }
    if {$lineB != $lineC} {
	return [expr {($lineA-$lineB) < ($lineC-$lineA)}]
    }
    return [expr {($chA-$chB) < ($chC-$chA)}]
}

# The procedure below is called to reset the selection anchor to
# whichever end is FARTHEST from the index argument.

proc tk_textResetAnchor {w index} {
    global tk_priv
    if {[$w tag ranges sel] == ""} {
	set tk_priv(selectMode) char
	$w mark set anchor $index
	return
    }
    if [tk_textIndexCloser $w $index sel.first sel.last] {
	if {$tk_priv(selectMode) == "char"} {
	    $w mark set anchor sel.last
	} else {
	    $w mark set anchor sel.last-1c
	}
    } else {
	$w mark set anchor sel.first
    }
}
