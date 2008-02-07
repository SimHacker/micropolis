#-----------------------------------------------------------------------------
# TclInit.tcl -- Extended Tcl initialization.
#-----------------------------------------------------------------------------
# $Id: TclInit.tcl,v 2.0 1992/10/16 04:51:37 markd Rel $
#-----------------------------------------------------------------------------

global env TCLENV
set TCLENV(inUnknown) 0

#
# Unknown command trap handler.
#
proc unknown {cmdName args} {
    global TCLENV
    if $TCLENV(inUnknown) {
        error "recursive unknown command trap: \"$cmdName\""}
    set TCLENV(inUnknown) 1
    
    set stat [catch {demand_load $cmdName} ret]
    if {$stat == 0 && $ret} {
        set TCLENV(inUnknown) 0
        return [uplevel 1 [list eval $cmdName $args]]
    }

    if {$stat != 0} {
        global errorInfo errorCode
        set TCLENV(inUnknown) 0
        error $ret $errorInfo $errorCode
    }

    global env interactiveSession noAutoExec

    if {$interactiveSession && ([info level] == 1) && ([info script] == "") &&
            (!([info exists noAutoExec] && [set noAutoExec]))} {
        if {[file rootname $cmdName] == "$cmdName"} {
            if [info exists env(PATH)] {
                set binpath [searchpath [split $env(PATH) :] $cmdName]
            } else {
                set binpath [searchpath "." $cmdName]
            }
        } else {
            set binpath $cmdName
        }
        if {[file executable $binpath]} {
            set TCLENV(inUnknown) 0
            uplevel 1 [list system [concat $cmdName $args]]
            return
        }
    }
    set TCLENV(inUnknown) 0
    error "invalid command name: \"$cmdName\""
}

#
# Search a path list for a file. (catch is for bad ~user)
#
proc searchpath {pathlist file} {
    foreach dir $pathlist {
        if {$dir == ""} {set dir .}
        if {[catch {file exists $dir/$file} result] == 0 && $result}  {
            return $dir/$file
        }
    }
    return {}
}

#
# Define a proc to be available for demand_load.
#
proc autoload {filenam args} {
    global TCLENV
    foreach i $args {
        set TCLENV(PROC:$i) [list F $filenam]
    }
}

#
# Search TCLPATH for a file to source.
#
proc load {name} {
    global TCLPATH errorCode
    if {[string first / $name] >= 0} {
        return  [uplevel #0 source $name]
    }
    set where [searchpath $TCLPATH $name]
    if [lempty $where] {
        error "couldn't find $name in Tcl search path" "" "TCLSH FILE_NOT_FOUND"
    }
    uplevel #0 source $where
}

autoload buildidx.tcl buildpackageindex

# == Put any code you want all Tcl programs to include here. ==

if !$interactiveSession return

# == Interactive Tcl session initialization ==

set TCLENV(topLevelPromptHook) {global programName; concat "$programName>" }
set TCLENV(downLevelPromptHook) {concat "=>"}

if [file readable ~/.tclrc] {source ~/.tclrc}

