# init.tcl --
#
# Default system startup file for Tcl-based applications.  Defines
# "unknown" procedure and auto-load facilities.
#
# $Header: /user6/ouster/tcl/scripts/RCS/init.tcl,v 1.7 92/07/25 16:29:36 ouster Exp $ SPRITE (Berkeley)
#
# Copyright 1991-1992 Regents of the University of California
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and without
# fee is hereby granted, provided that this copyright
# notice appears in all copies.  The University of California
# makes no representations about the suitability of this
# software for any purpose.  It is provided "as is" without
# express or implied warranty.
#

# unknown:
# Invoked when a Tcl command is invoked that doesn't exist in the
# interpreter:
#
#	1. See if the autoload facility can locate the command in a
#	   Tcl script file.  If so, load it and execute it.
#	2. See if the command exists as an executable UNIX program.
#	   If so, "exec" the command.
#	3. See if the command is a valid abbreviation for another command.
#	   if so, invoke the command.  However, only permit abbreviations
#	   at top-level.

proc unknown args {
    global auto_noexec auto_noload env unknown_active

    if [info exists unknown_active] {
	unset unknown_active
	error "unexpected recursion in \"unknown\" command"
    }
    set unknown_active 1
    set name [lindex $args 0]
    if ![info exists auto_noload] {
	if [auto_load $name] {
	    unset unknown_active
	    return [uplevel $args]
	}
    }
    if ![info exists auto_noexec] {
	if [auto_execok $name] {
	    unset unknown_active
	    return [uplevel exec $args]
	}
    }
    if {([info level] == 1) && ([info script] == "")} {
	set cmds [info commands $name*]
	if {[llength $cmds] == 1} {
	    unset unknown_active
	    return [uplevel [lreplace $args 0 0 $cmds]]
	}
	if {[llength $cmds] != 0} {
	    unset unknown_active
	    if {$name == ""} {
		error "empty command name \"\""
	    } else {
		error "ambiguous command name \"$name\": [lsort $cmds]"
	    }
	}
    }
    unset unknown_active
    error "invalid command name \"$name\""
}

# auto_load:
# Checks a collection of library directories to see if a procedure
# is defined in one of them.  If so, it sources the appropriate
# library file to create the procedure.  Returns 1 if it successfully
# loaded the procedure, 0 otherwise.

proc auto_load cmd {
    global auto_index auto_oldpath auto_path env
    if [info exists auto_index($cmd)] {
	uplevel #0 source $auto_index($cmd)
	return 1
    }
    if [catch {set path $auto_path}] {
	if [catch {set path $env(TCLLIBPATH)}] {
	    if [catch {set path [info library]}] {
		return 0
	    }
	}
    }
    if [info exists auto_oldpath] {
	if {$auto_oldpath == $path} {
	    return 0
	}
    }
    set auto_oldpath $path
    catch {unset auto_index}
    foreach dir $path {
	set f ""
	catch {
	    set f [open $dir/tclindex]
	    if {[gets $f] != "# Tcl autoload index file: each line identifies a Tcl"} {
		puts stdout "Bad id line in file $dir/tclindex"
		error done
	    }
	    while {[gets $f line] >= 0} {
		if {([string index $line 0] == "#") || ([llength $line] != 2)} {
		    continue
		}
		set name [lindex $line 0]
		if {![info exists auto_index($name)]} {
		    set auto_index($name) $dir/[lindex $line 1]
		}
	    }
	}
	if {$f != ""} {
	    close $f
	}
    }
    if [info exists auto_index($cmd)] {
	uplevel #0 source $auto_index($cmd)
	return 1
    }
    return 0
}

# auto_execok:
# Returns 1 if there's an executable in the current path for the
# given name, 0 otherwise.  Builds an associative array auto_execs
# that caches information about previous checks, for speed.

proc auto_execok name {
    global auto_execs env

    if [info exists auto_execs($name)] {
	return $auto_execs($name)
    }
    set auto_execs($name) 0
    foreach dir [split $env(PATH) :] {
	if {[file executable $dir/$name] && ![file isdirectory $dir/$name]} {
	    set auto_execs($name) 1
	    return 1
	}
    }
    return 0
}

# auto_reset:
# Destroy all cached information for auto-loading and auto-execution,
# so that the information gets recomputed the next time it's needed.

proc auto_reset {} {
    global auto_execs auto_index
    unset auto_execs auto_index
}
