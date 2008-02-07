# menu.tcl --
#
# This file contains Tcl procedures used to manage Tk menus and
# menubuttons.  Most of the code here is dedicated to support for
# menu traversal via the keyboard.
#
# $Header: /user6/ouster/wish/scripts/RCS/menu.tcl,v 1.11 92/08/08 14:49:55 ouster Exp $ SPRITE (Berkeley)
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

# The procedure below is publically available.  It is used to indicate
# the menus associated with a particular top-level window, for purposes
# of keyboard menu traversal.  Its first argument is the path name of
# a top-level window, and any additional arguments are the path names of
# the menu buttons associated with that top-level window, in the order
# they should be traversed.  If no menu buttons are named, the procedure
# returns the current list of menus for w.  If a single empty string is
# supplied, then the menu list for w is cancelled.  Otherwise, tk_menus
# sets the menu list for w to the menu buttons.

# Variables used by menu buttons:
# $tk_priv(posted@$screen) -	keeps track of the menubutton whose menu is
#				currently posted (or empty string, if none).
# $tk_priv(inMenuButton@$screen)-
#				if non-null, identifies menu button
#				containing mouse pointer.
# $tk_priv(relief@$screen) -	keeps track of original relief of posted
#				menu button, so it can be restored later.
# $tk_priv(dragging@$screen) -	if non-null, identifies menu button whose
#				menu is currently being dragged in a tear-off
#				operation.
# $tk_priv(focus@$screen) -	records old focus window so focus can be
#				returned there after keyboard traversal
#				to menu.
#
# Variables used by menus:
#     $tk_priv(x@$screen) and $tk_priv(y@$screen) are used to keep
#     track of the position of the mouse cursor in the menu window
#     during dragging of tear-off menus.  $tk_priv(window) keeps track
#     of the menu containing the mouse, if any.

proc tk_menus {w args} {
    global tk_priv

    if {$args == ""} {
	if [catch {set result [set tk_priv(menusFor$w)]}] {
	    return ""
	}
	return $result
    }

    if {$args == "{}"} {
	catch {unset tk_priv(menusFor$w)}
	return ""
    }

    append tk_priv(menusFor$w) " $args"
}

# The procedure below is publically available.  It takes any number of
# arguments taht are names of widgets or classes.  It sets up bindings
# for the widgets or classes so that keyboard menu traversal is possible
# when the input focus is in those widgets or classes.

proc tk_bindForTraversal args {
    foreach w $args {
	bind $w <Alt-KeyPress> {tk_traverseToMenu %W %A}
	bind $w <F10> {tk_firstMenu %W}
    }
}

# The procedure below does all of the work of posting a menu (including
# unposting any other menu that might currently be posted).  The "w"
# argument is the name of the menubutton for the menu to be posted.
# Note:  if $w is disabled then the procedure does nothing.

proc tk_mbPost {w} {
    global tk_priv
    if {[lindex [$w config -state] 4] == "disabled"} {
	return
    }
    set screen [winfo screen $w]
    if {![info exists tk_priv(posted@$screen)]} {
	set tk_priv(posted@$screen) {}
    }
    if {![info exists tk_priv(focus@$screen)]} {
	set tk_priv(focus@$screen) {}
    }
    set cur $tk_priv(posted@$screen)
    if {$cur == $w} {
	return
    }
    if {$cur != ""} {tk_mbUnpost $w}
    set tk_priv(relief@$screen) [lindex [$w config -relief] 4]
    $w config -relief raised
    set tk_priv(cursor@$screen) [lindex [$w config -cursor] 4]
    $w config -cursor arrow
    $w post
    catch {grab -global $w}
    set tk_priv(posted@$screen) $w
    if {$tk_priv(focus@$screen) == ""} {
	set tk_priv(focus@$screen) [focus -query $w]
    }
    set menu [lindex [$w config -menu] 4]
    focus $menu
}

# The procedure below does all the work of unposting the menubutton that's
# currently posted.  It takes no arguments.

proc tk_mbUnpost {w} {
    global tk_priv
    set screen [winfo screen $w]
    if {![info exists tk_priv(posted@$screen)]} {
	set tk_priv(posted@$screen) {}
    }
    if {![info exists tk_priv(focus@$screen)]} {
	set tk_priv(focus@$screen) {}
    }
    set mb $tk_priv(posted@$screen)
    if {$mb != ""} {
	$mb config -relief $tk_priv(relief@$screen)
	$mb config -cursor $tk_priv(cursor@$screen)
	$mb unpost
	catch {grab -off $mb}
	set menu [lindex [$mb config -menu] 4]
	focus $tk_priv(focus@$screen)
	set tk_priv(focus@$screen) ""
	set tk_priv(posted@$screen) {}
    }
}

# The procedure below is invoked to implement keyboard traversal to
# a menu button.  It takes two arguments:  the name of a window where
# a keystroke originated, and the ascii character that was typed.
# This procedure finds a menu bar by looking upward for a top-level
# window, then looking for a window underneath that named "menu".
# Then it searches through all the subwindows of "menu" for a menubutton
# with an underlined character matching char.  If one is found, it
# posts that menu.

proc tk_traverseToMenu {w char} {
    if {$char == ""} {
	return
    }
    set char [string tolower $char]

    foreach mb [tk_getMenuButtons $w] {
	if {[winfo class $mb] == "Menubutton"} {
	    set char2 [string index [lindex [$mb config -text] 4] \
		    [lindex [$mb config -underline] 4]]
	    if {[string compare $char [string tolower $char2]] == 0} {
		tk_mbPost $mb
		[lindex [$mb config -menu] 4] activate 0
		return
	    }
	}
    }
}

# The procedure below is used to implement keyboard traversal within
# the posted menu.  It takes two arguments:  the name of the menu to
# be traversed within, and an ASCII character.  It searches for an
# entry in the menu that has that character underlined.  If such an
# entry is found, it is invoked and the menu is unposted.

proc tk_traverseWithinMenu {w char} {
    if {$char == ""} {
	return
    }
    set char [string tolower $char]
    set last [$w index last]
    for {set i 0} {$i <= $last} {incr i} {
	if [catch {set char2 [string index \
		[lindex [$w entryconfig $i -label] 4] \
		[lindex [$w entryconfig $i -underline] 4]]}] {
	    continue
	}
	if {[string compare $char [string tolower $char2]] == 0} {
	    tk_mbUnpost $w
	    $w invoke $i
	    return
	}
    }
}

# The procedure below takes a single argument, which is the name of
# a window.  It returns a list containing path names for all of the
# menu buttons associated with that window's top-level window, or an
# empty list if there are none.

proc tk_getMenuButtons {w} {
    global tk_priv
    set top [winfo toplevel $w]
    if [catch {set buttons [set tk_priv(menusFor$top)]}] {
	return ""
    }
    return $buttons
}

# The procedure below is used to traverse to the next or previous
# menu in a menu bar.  It takes one argument, which is a count of
# how many menu buttons forward or backward (if negative) to move.
# If there is no posted menu then this procedure has no effect.

proc tk_nextMenu {w count} {
    global tk_priv
    set screen [winfo screen $w]
    if {![info exists tk_priv(posted@$screen)]} {
	set tk_priv(posted@$screen) {}
    }
    if {$tk_priv(posted@$screen) == ""} {
	return
    }
    set buttons [tk_getMenuButtons $tk_priv(posted@$screen)]
    set length [llength $buttons]
    for {set i 0} 1 {incr i} {
	if {$i >= $length} {
	    return
	}
	if {[lindex $buttons $i] == $tk_priv(posted@$screen)} {
	    break
	}
    }
    incr i $count
    while 1 {
	while {$i < 0} {
	    incr i $length
	}
	while {$i >= $length} {
	    incr i -$length
	}
	set mb [lindex $buttons $i]
	if {[lindex [$mb configure -state] 4] != "disabled"} {
	    break
	}
	incr i $count
    }
    tk_mbUnpost $w
    tk_mbPost $mb
    [lindex [$mb config -menu] 4] activate 0
}

# The procedure below is used to traverse to the next or previous entry
# in the posted menu.  It takes one argument, which is 1 to go to the
# next entry or -1 to go to the previous entry.  Disabled entries are
# skipped in this process.

proc tk_nextMenuEntry {w count} {
    global tk_priv
    set screen [winfo screen $w]
    if {![info exists tk_priv(posted@$screen)]} {
	set tk_priv(posted@$screen) {}
    }
    if {$tk_priv(posted@$screen) == ""} {
	return
    }
    set menu [lindex [$tk_priv(posted@$screen) config -menu] 4]
    set length [expr [$menu index last]+1]
    set i [$menu index active]
    if {$i == "none"} {
	set i 0
    } else {
	incr i $count
    }
    while 1 {
	while {$i < 0} {
	    incr i $length
	}
	while {$i >= $length} {
	    incr i -$length
	}
	if {[catch {$menu entryconfigure $i -state} state] == 0} {
	    if {[lindex $state 4] != "disabled"} {
		break
	    }
	}
	incr i $count
    }
    $menu activate $i
}

# The procedure below invokes the active entry in the posted menu,
# if there is one.  Otherwise it does nothing.

proc tk_invokeMenu {w} {
    set i [$w index active]
    if {$i != "none"} {
	tk_mbUnpost $w
	update idletasks
	$w invoke $i
    }
}

# The procedure below is invoked to keyboard-traverse to the first
# menu for a given source window.  The source window is passed as
# parameter.

proc tk_firstMenu {w} {
    set mb [lindex [tk_getMenuButtons $w] 0]
    if {$mb != ""} {
	tk_mbPost $mb
	[lindex [$mb config -menu] 4] activate 0
    }
}

# The procedure below is invoked when a button-1-down event is
# received by a menu button.  If the mouse is in the menu button
# then it posts the button's menu.  If the mouse isn't in the
# button's menu, then it deactivates any active entry in the menu.
# Remember, event-sharing can cause this procedure to be invoked
# for two different menu buttons on the same event.

proc tk_mbButtonDown {w x y} {
    global tk_priv
    set screen [winfo screen $w]
    if {![info exists tk_priv(inMenuButton@$screen)]} {
	set tk_priv(inMenuButton@$screen) {}
    }
    if {![info exists tk_priv(posted@$screen)]} {
	set tk_priv(posted@$screen) {}
    }
    if {[lindex [$w config -state] 4] == "disabled"} {
	return
    } 
    if {$tk_priv(inMenuButton@$screen) == $w} {
	tk_mbPost $w
    }
    if {$tk_priv(posted@$screen) != ""} then {
	set menu [lindex [$tk_priv(posted@$screen) config -menu] 4]
	if {![info exists tk_priv(window@$screen)]} {
	    set tk_priv(window@$screen) {}
	}
	if {$tk_priv(window@$screen) != $menu} {
	    $menu activate none
	}
    }
}

proc tk_mbButtonUp {w x y} {
    global tk_priv
    set screen [winfo screen $w]
    if {![info exists tk_priv(inMenuButton@$screen)]} {
	set tk_priv(inMenuButton@$screen) {}
    }
    if {![info exists tk_priv(posted@$screen)]} {
	set tk_priv(posted@$screen) {}
    }
    if {($tk_priv(inMenuButton@$screen) != "") &&
	($tk_priv(posted@$screen) != "")} {
	[lindex [$tk_priv(posted@$screen) config -menu] 4] activate 0
    } else {
	tk_mbUnpost $w
    }
}

proc tk_mbButtonEnter {w m} {
    global tk_priv
    set screen [winfo screen $w]
    set tk_priv(inMenuButton@$screen) $w
    if {[lindex [$w config -state] 4] != "disabled"} {
	$w config -state active
    }
}

proc tk_mbButtonLeave {w} {
    global tk_priv
    set screen [winfo screen $w]
    set tk_priv(inMenuButton@$screen) {}
    if {[lindex [$w config -state] 4] != "disabled"} {
	$w config -state normal
    }
}

# In the binding below, it's important to ignore grab-related entries
# and exits because they lag reality and can cause menus to chase
# their own tail, repeatedly posting and unposting.

proc tk_mbButton1Enter {w m} {
    global tk_priv
    set screen [winfo screen $w]
    set tk_priv(inMenuButton@$screen) $w
    if {([lindex [$w config -state] 4] != "disabled")
	    && ("$m" != "NotifyGrab") && ("$m" != "NotifyUngrab")} {
	$w config -state active
	tk_mbPost $w
    }
}


proc tk_mbButton2Down {w x y} {
    global tk_priv
    set screen [winfo screen $w]
    if {![info exists tk_priv(inMenuButton@$screen)]} {
	set tk_priv(inMenuButton@$screen) {}
    }
    if {![info exists tk_priv(posted@$screen)]} {
	set tk_priv(posted@$screen) {}
    }
    if {($tk_priv(posted@$screen) == "")
	    && ([lindex [$w config -state] 4] != "disabled")} {
	set tk_priv(dragging@$screen) $w
	[lindex [$w config -menu] 4] post $x $y
    }
}


proc tk_mbButton2Motion {w x y} {
    global tk_priv
    set screen [winfo screen $w]
    if {![info exists tk_priv(dragging@$screen)]} {
	set tk_priv(dragging@$screen) {}
    }
    if {$tk_priv(dragging@$screen) != ""} {
	[lindex [$tk_priv(dragging@$screen) config -menu] 4] post $x $y
    }
}

proc tk_mbButton2Up {w x y} {
    global tk_priv
    set screen [winfo screen $w]
    set tk_priv(dragging@$screen) ""
}


proc tk_menuEnter {w y} {
    global tk_priv
    set screen [winfo screen $w]
    set tk_priv(window@$screen) $w
    $w activate @$y
}

proc tk_menuLeave {w} {
    global tk_priv
    set screen [winfo screen $w]
    set tk_priv(window@$screen) {}
    $w activate none
}

proc tk_menuMotion {w y} {
    global tk_priv
    set screen [winfo screen $w]
    if {![info exists tk_priv(window@$screen)]} {
	set tk_priv(window@$screen) {}
    }
    if {$tk_priv(window@$screen) != ""} {
	$w activate @$y
    }
}

proc tk_menuUp {w y} {
    tk_menuMotion $w $y
    tk_invokeMenu $w
}

proc tk_menu2Down {w x y} {
    global tk_priv
    set screen [winfo screen $w]
    set tk_priv(x@$screen) $x
    set tk_priv(y@$screen) $y
}

proc tk_menu2Motion {w x y} {
    global tk_priv
    set screen [winfo screen $w]
    if {$tk_priv(posted@$screen) == ""} {
	$w post [expr $x-$tk_priv(x@$screen)] [expr $y-$tk_priv(y@$screen)]
    }
}

