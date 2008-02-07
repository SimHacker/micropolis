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

# ----------------------------------------------------------------------
# Class bindings for various flavors of button widgets.  
# ----------------------------------------------------------------------

bind Button <Any-Enter> {tk_butEnter %W}
bind Button <Any-Leave> {tk_butLeave %W}
bind Button <1> {tk_butDown %W}
bind Button <ButtonRelease-1> {tk_butUp %W}
bind Button <2> {tk_butDown %W}
bind Button <ButtonRelease-2> {tk_butUp %W}
bind Button <3> {tk_butDown %W}
bind Button <ButtonRelease-3> {tk_butUp %W}

bind CheckButton <Any-Enter> {tk_butEnter %W}
bind CheckButton <Any-Leave> {tk_butLeave %W}
bind CheckButton <1> {tk_butDown %W}
bind CheckButton <ButtonRelease-1> {tk_butUp %W}
bind CheckButton <2> {tk_butDown %W}
bind CheckButton <ButtonRelease-2> {tk_butUp %W}
bind CheckButton <3> {tk_butDown %W}
bind CheckButton <ButtonRelease-3> {tk_butUp %W}

bind RadioButton <Any-Enter> {tk_butEnter %W}
bind RadioButton <Any-Leave> {tk_butLeave %W}
bind RadioButton <1> {tk_butDown %W}
bind RadioButton <ButtonRelease-1> {tk_butUp %W}
bind RadioButton <2> {tk_butDown %W}
bind RadioButton <ButtonRelease-2> {tk_butUp %W}
bind RadioButton <3> {tk_butDown %W}
bind RadioButton <ButtonRelease-3> {tk_butUp %W}

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
    tk_entryCutPress %W
    if {"%A" != ""} {
	%W insert cursor %A
	tk_entrySeeCaret %W
    }
}
bind Entry <Delete> {tk_entryDelPress %W}
bind Entry <BackSpace> {tk_entryDelPress %W}
bind Entry <Control-h> {tk_entryDelPress %W}
bind Entry <Control-d> {tk_entryCutPress %W}
bind Entry <Control-u> {tk_entryDelLine %W}
bind Entry <Control-x> {tk_entryCutPress %W}
bind Entry <Control-y> {tk_entryCopyPress %W}
bind Entry <Control-v> {tk_entryCopyPress %W}
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
bind Listbox <3> {%W select from [%W nearest %y]}
bind Listbox <B3-Motion> {%W select to [%W nearest %y]}
bind Listbox <Shift-3> {%W select adjust [%W nearest %y]}
bind Listbox <Shift-B3-Motion> {%W select to [%W nearest %y]}

# ----------------------------------------------------------------------
# Class bindings for scrollbar widgets.  
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
# Class bindings for scale widgets.  
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
# Class bindings for menubutton widgets.  
# ----------------------------------------------------------------------

bind Menubutton <Enter>			{tk_mbButtonEnter %W %m}
bind Menubutton <Any-Leave>		{tk_mbButtonLeave %W}
bind Menubutton <1>			{tk_mbButtonDown %W %X %Y}
bind Menubutton <Any-ButtonRelease-1>	{tk_mbButtonUp %W %X %Y}
bind Menubutton <B1-Enter>		{tk_mbButton1Enter %W %m}
bind Menubutton <2>			{tk_mbButtonDown %W %X %Y}
bind Menubutton <ButtonRelease-2>	{tk_mbButtonUp %W %X %Y}
bind Menubutton <3>			{tk_mbButtonDown %W %X %Y}
bind Menubutton <ButtonRelease-3>	{tk_mbButtonUp %W %X %Y}

# ----------------------------------------------------------------------
# Class bindings for menu widgets.
# ----------------------------------------------------------------------

bind Menu <Any-Enter> {tk_menuEnter %W %y}
bind Menu <Any-Leave> {tk_menuLeave %W}
bind Menu <Any-Motion> {tk_menuMotion %W %y}
bind Menu <ButtonRelease-1> {tk_menuUp %W %y}
bind Menu <ButtonRelease-2> {tk_menuUp %W %y}
bind Menu <ButtonRelease-3> {tk_menuUp %W %y}

bind Menu <Escape> {tk_mbUnpost %W}
bind Menu <Any-KeyPress> {tk_traverseWithinMenu %W %A}
bind Menu <Left> {tk_nextMenu %W -1}
bind Menu <Right> {tk_nextMenu %W 1}
bind Menu <Up> {tk_nextMenuEntry %W -1}
bind Menu <Down> {tk_nextMenuEntry %W 1}
bind Menu <Return> {tk_invokeMenu %W}

# ----------------------------------------------------------------------
# Class bindings for text widgets. 
# ----------------------------------------------------------------------

#bind Text <1> {tk_textDown %W %x %y}
#bind Text <Double-1> {tk_textDoubleDown %W %x %y}
#bind Text <Triple-1> {tk_textTripleDown %W %x %y}
#bind Text <B1-Motion> {tk_textSelectTo %W %x %y}
#bind Text <Shift-1> {tk_textAdjustTo %W %x %y}
#bind Text <Shift-B1-Motion> {tk_textSelectTo %W %x %y}
bind Text <2> {%W scan mark %y}
bind Text <B2-Motion> {%W scan dragto %y}
bind Text <Any-KeyPress> {tk_textKeyPress %W %A}
bind Text <Return> {tk_textReturnPress %W}
bind Text <BackSpace> {tk_textDelPress %W}
bind Text <Delete> {tk_textDelPress %W}
bind Text <Control-h> {tk_textDelPress %W}
bind Text <Control-d> {tk_textCutPress %W}
bind Text <Control-v> {tk_textCopyPress %W}
tk_bindForTraversal Text

# Initialize the elements of tk_priv that require initialization.
