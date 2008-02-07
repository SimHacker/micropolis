# tk.tcl --
#
# Initialization script normally executed in the interpreter for each
# Tk-based application.  Arranges class bindings for widgets.
#
# $Header: /user6/ouster/wish/scripts/RCS/tk.tcl,v 1.19 92/08/08 14:50:08 ouster Exp $ SPRITE (Berkeley)
#
# Copyright 1992 Regents of the University of California
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and without
# fee is hereby granted, provided that this copyright
# notice appears in all copies.  The University of California
# makes no representations about the suitability of this
# software for any purpose.  It is provided "as is" without
# express or implied warranty.

# Insist on running with a compatible version of Tcl.

if "[info tclversion] != {6.4}" {
    error "wrong version of Tcl loaded ([info tclversion]): need 6.4"
}

# Initialize the auto-load path to include Tk's directory as well as
# Tcl's directory:

set auto_path "$tk_library [info library]"

# Turn off strict Motif look and feel as a default.

set tk_strictMotif 0

# ----------------------------------------------------------------------
# Class bindings for various flavors of button widgets.  $tk_priv(window)
# keeps track of the button containing the mouse, and $tk_priv(relief)
# saves the original relief of the button so it can be restored when
# the mouse button is released.
# ----------------------------------------------------------------------

bind Button <Any-Enter> {tk_butEnter %W}
bind Button <Any-Leave> {tk_butLeave %W}
bind Button <1> {tk_butDown %W}
bind Button <ButtonRelease-1> {tk_butUp %W}

bind CheckButton <Any-Enter> {tk_butEnter %W}
bind CheckButton <Any-Leave> {tk_butLeave %W}
bind CheckButton <1> {tk_butDown %W}
bind CheckButton <ButtonRelease-1> {tk_butUp %W}

bind RadioButton <Any-Enter> {tk_butEnter %W}
bind RadioButton <Any-Leave> {tk_butLeave %W}
bind RadioButton <1> {tk_butDown %W}
bind RadioButton <ButtonRelease-1> {tk_butUp %W}

# ----------------------------------------------------------------------
# Class bindings for entry widgets.
# ----------------------------------------------------------------------

bind Entry <1> {
    %W cursor @%x
    %W select from @%x
    if {[lindex [%W config -state] 4] == "normal"} {focus %W}
}
bind Entry <B1-Motion> {%W select to @%x}
bind Entry <Shift-1> {%W select adjust @%x}
bind Entry <Shift-B1-Motion> {%W select to @%x}
bind Entry <2> {%W scan mark %x}
bind Entry <B2-Motion> {%W scan dragto %x}
bind Entry <Any-KeyPress> {
    if {"%A" != ""} {
	%W insert cursor %A
	tk_entrySeeCaret %W
    }
}
bind Entry <Delete> {tk_entryBackspace %W; tk_entrySeeCaret %W}
bind Entry <BackSpace> {tk_entryBackspace %W; tk_entrySeeCaret %W}
bind Entry <Control-h> {tk_entryBackspace %W; tk_entrySeeCaret %W}
bind Entry <Control-d> {%W delete sel.first sel.last; tk_entrySeeCaret %W}
bind Entry <Control-u> {%W delete 0 end}
bind Entry <Control-v> {%W insert cursor [selection get]; tk_entrySeeCaret %W}
bind Entry <Control-w> {tk_entryBackword %W; tk_entrySeeCaret %W}
tk_bindForTraversal Entry

# ----------------------------------------------------------------------
# Class bindings for listbox widgets.
# ----------------------------------------------------------------------

bind Listbox <1> {%W select from [%W nearest %y]}
bind Listbox <B1-Motion> {%W select to [%W nearest %y]}
bind Listbox <Shift-1> {%W select adjust [%W nearest %y]}
bind Listbox <Shift-B1-Motion> {%W select to [%W nearest %y]}
bind Listbox <2> {%W scan mark %x %y}
bind Listbox <B2-Motion> {%W scan dragto %x %y}

# ----------------------------------------------------------------------
# Class bindings for scrollbar widgets.  When strict Motif is requested,
# the bindings use $tk_priv(buttons) and $tk_priv(activeFg) to set the
# -activeforeground color to -foreground when the mouse is in the window
# and restore it when the mouse leaves.
# ----------------------------------------------------------------------

bind Scrollbar <Any-Enter> {
    if $tk_strictMotif {
	set tk_priv(activeFg) [lindex [%W config -activeforeground] 4]
	%W config -activeforeground [lindex [%W config -foreground] 4]
    }
}
bind Scrollbar <Any-Leave> {
    if {$tk_strictMotif && ($tk_priv(buttons) == 0)} {
	%W config -activeforeground $tk_priv(activeFg)
    }
}
bind Scrollbar <Any-ButtonPress> {incr tk_priv(buttons)}
bind Scrollbar <Any-ButtonRelease> {incr tk_priv(buttons) -1}

# ----------------------------------------------------------------------
# Class bindings for scale widgets.  When strict Motif is requested,
# the bindings use $tk_priv(buttons) and $tk_priv(activeFg) to set the
# -activeforeground color to -foreground when the mouse is in the window
# and restore it when the mouse leaves.
# ----------------------------------------------------------------------

bind Scale <Any-Enter> {
    if $tk_strictMotif {
	set tk_priv(activeFg) [lindex [%W config -activeforeground] 4]
	%W config -activeforeground [lindex [%W config -sliderforeground] 4]
    }
}
bind Scale <Any-Leave> {
    if {$tk_strictMotif && ($tk_priv(buttons) == 0)} {
	%W config -activeforeground $tk_priv(activeFg)
    }
}
bind Scale <Any-ButtonPress> {incr tk_priv(buttons)}
bind Scale <Any-ButtonRelease> {incr tk_priv(buttons) -1}

# ----------------------------------------------------------------------
# Class bindings for menubutton widgets.  Variables used:
# $tk_priv(posted) -		keeps track of the menubutton whose menu is
#				currently posted (or empty string, if none).
# $tk_priv(inMenuButton)-	if non-null, identifies menu button
#				containing mouse pointer.
# $tk_priv(relief) -		keeps track of original relief of posted
#				menu button, so it can be restored later.
# $tk_priv(dragging) -		if non-null, identifies menu button whose
#				menu is currently being dragged in a tear-off
#				operation.
# $tk_priv(focus) -		records old focus window so focus can be
#				returned there after keyboard traversal
#				to menu.
# ----------------------------------------------------------------------

bind Menubutton <Enter> {
    set tk_priv(inMenuButton) %W
    if {[lindex [%W config -state] 4] != "disabled"} {
	if {!$tk_strictMotif} {
	    %W config -state active
	}
    }
}
bind Menubutton <Any-Leave> {
    set tk_priv(inMenuButton) {}
    if {[lindex [%W config -state] 4] != "disabled"} {
	if {!$tk_strictMotif} {
	    %W config -state normal
	}
    }
}
bind Menubutton <1> {tk_mbButtonDown %W}
bind Menubutton <Any-ButtonRelease-1> {
    if {($tk_priv(inMenuButton) != "") && ($tk_priv(posted) != "")} {
	[lindex [$tk_priv(posted) config -menu] 4] activate 0
    } else {
	tk_mbUnpost
    }
}

# In the binding below, it's important to ignore grab-related entries
# and exits because they lag reality and can cause menus to chase
# their own tail, repeatedly posting and unposting.

bind Menubutton <B1-Enter> {
    set tk_priv(inMenuButton) %W
    if {([lindex [%W config -state] 4] != "disabled")
	    && ("%m" != "NotifyGrab") && ("%m" != "NotifyUngrab")} {
	if {!$tk_strictMotif} {
	    %W config -state active
	}
	tk_mbPost %W
    }
}
bind Menubutton <2> {
    if {($tk_priv(posted) == "")
	    && ([lindex [%W config -state] 4] != "disabled")} {
	set tk_priv(dragging) %W
	[lindex [$tk_priv(dragging) config -menu] 4] post %X %Y
    }
}
bind Menubutton <B2-Motion> {
    if {$tk_priv(dragging) != ""} {
	[lindex [$tk_priv(dragging) config -menu] 4] post %X %Y
    }
}
bind Menubutton <ButtonRelease-2> {set tk_priv(dragging) ""}

# ----------------------------------------------------------------------
# Class bindings for menu widgets.  $tk_priv(x) and $tk_priv(y) are used
# to keep track of the position of the mouse cursor in the menu window
# during dragging of tear-off menus.  $tk_priv(window) keeps track of
# the menu containing the mouse, if any.
# ----------------------------------------------------------------------

bind Menu <Any-Enter> {set tk_priv(window) %W; %W activate @%y}
bind Menu <Any-Leave> {set tk_priv(window) {}; %W activate none}
bind Menu <Any-Motion> {
    if {$tk_priv(window) != ""} {
	%W activate @%y
    }
}
bind Menu <ButtonRelease-1> {tk_invokeMenu %W}
bind Menu <2> {set tk_priv(x) %x; set tk_priv(y) %y}
bind Menu <B2-Motion> {
    if {$tk_priv(posted) == ""} {
	%W post [expr %X-$tk_priv(x)] [expr %Y-$tk_priv(y)]
    }
}
bind Menu <B2-Leave> { }
bind Menu <B2-Enter> { }
bind Menu <Escape> {tk_mbUnpost}
bind Menu <Any-KeyPress> {tk_traverseWithinMenu %W %A}
bind Menu <Left> {tk_nextMenu -1}
bind Menu <Right> {tk_nextMenu 1}
bind Menu <Up> {tk_nextMenuEntry -1}
bind Menu <Down> {tk_nextMenuEntry 1}
bind Menu <Return> {tk_invokeMenu %W}

# ----------------------------------------------------------------------
# Class bindings for text widgets. $tk_priv(selectMode) holds one of
# "char", "word", or "line" to indicate which selection mode is active.
# ----------------------------------------------------------------------

bind Text <1> {
    set tk_priv(selectMode) char
    %W mark set insert @%x,%y
    %W mark set anchor insert
    if {[lindex [%W config -state] 4] == "normal"} {focus %W}
}
bind Text <Double-1> {
    set tk_priv(selectMode) word
    %W mark set insert "@%x,%y wordstart"
    tk_textSelectTo %W insert
}
bind Text <Triple-1> {
    set tk_priv(selectMode) line
    %W mark set insert "@%x,%y linestart"
    tk_textSelectTo %W insert
}
bind Text <B1-Motion> {tk_textSelectTo %W @%x,%y}
bind Text <Shift-1> {
    tk_textResetAnchor %W @%x,%y
    tk_textSelectTo %W @%x,%y
}
bind Text <Shift-B1-Motion> {tk_textSelectTo %W @%x,%y}
bind Text <2> {%W scan mark %y}
bind Text <B2-Motion> {%W scan dragto %y}
bind Text <Any-KeyPress> {
    if {"%A" != ""} {
	%W insert insert %A
	%W yview -pickplace insert
    }
}
bind Text <Return> {%W insert insert \n; %W yview -pickplace insert}
bind Text <BackSpace> {tk_textBackspace %W; %W yview -pickplace insert}
bind Text <Delete> {tk_textBackspace %W; %W yview -pickplace insert}
bind Text <Control-h> {tk_textBackspace %W; %W yview -pickplace insert}
bind Text <Control-d> {%W delete sel.first sel.last}
bind Text <Control-v> {
    %W insert insert [selection get]
    %W yview -pickplace insert
}
tk_bindForTraversal Text

# Initialize the elements of tk_priv that require initialization.

set tk_priv(buttons) 0
set tk_priv(dragging) {}
set tk_priv(focus) {}
set tk_priv(inMenuButton) {}
set tk_priv(posted) {}
set tk_priv(selectMode) char
set tk_priv(window) {}
