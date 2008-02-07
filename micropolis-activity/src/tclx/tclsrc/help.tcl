#
# help.tcl --
#
# Tcl help command. (see TclX manual)
# 
#------------------------------------------------------------------------------
# Copyright 1992 Karl Lehenbauer and Mark Diekhans.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted, provided
# that the above copyright notice appear in all copies.  Karl Lehenbauer and
# Mark Diekhans make no representations about the suitability of this
# software for any purpose.  It is provided "as is" without express or
# implied warranty.
#------------------------------------------------------------------------------
# $Id: help.tcl,v 2.0 1992/10/16 04:52:01 markd Rel $
#------------------------------------------------------------------------------
#

#@package: TclX-help help helpcd helppwd apropos

#------------------------------------------------------------------------------
# Take a path name which might have . and .. elements and flatten them out.

proc help:flattenPath {pathName} {
    set newPath {}
    foreach element [split $pathName /] {
        if {"$element" == "."} {
           continue
        }
        if {"$element" == ".."} {
            if {[llength [join $newPath /]] == 0} {
                error "Help: name goes above subject directory root"}
            lvarpop newPath [expr [llength $newPath]-1]
            continue
        }
        lappend newPath $element
    }
    set newPath [join $newPath /]
    
    # Take care of the case where we started with something line "/" or "/."

    if {("$newPath" == "") && [string match "/*" $pathName]} {
        set newPath "/"}
        
    return $newPath
}

#------------------------------------------------------------------------------
# Take the help current directory and a path and evaluate it into a help root-
# based path name.

proc help:EvalPath {pathName} {
    global TCLENV

    if {![string match "/*" $pathName]} {
        if {"$pathName" == ""} {
            return $TCLENV(help:curDir)}
        if {"$TCLENV(help:curDir)" == "/"} {
            set pathName "/$pathName"
        } else {
            set pathName "$TCLENV(help:curDir)/$pathName"
        }
    }
    set pathName [help:flattenPath $pathName]
    if {[string match "*/" $pathName] && ($pathName != "/")} {
        set pathName [csubstr $pathName 0 [expr [length $pathName]-1]]}

    return $pathName    
}

#------------------------------------------------------------------------------
# Display a line of output, pausing waiting for input before displaying if the
# screen size has been reached.  Return 1 if output is to continue, return
# 0 if no more should be outputed, indicated by input other than return.
#

proc help:Display {line} {
    global TCLENV
    if {$TCLENV(help:lineCnt) >= 23} {
        set TCLENV(help:lineCnt) 0
        puts stdout ":" nonewline
        flush stdout
        gets stdin response
        if {![lempty $response]} {
            return 0}
    }
    puts stdout $line
    incr TCLENV(help:lineCnt)
}

#------------------------------------------------------------------------------
# Display a file.

proc help:DisplayFile {filepath} {

    set inFH [open $filepath r]
    while {[gets $inFH fileBuf] >= 0} {
        if {![help:Display $fileBuf]} {
            break}
    }
    close $inFH

}    

#------------------------------------------------------------------------------
# Procedure to return contents of a directory.  A list is returned, consisting
# of two lists.  The first list are all the directories (subjects) in the
# specified directory.  The second is all of the help files.  Eash sub-list
# is sorted in alphabetical order.
#

proc help:ListDir {dirPath} {
    set dirList {}
    set fileList {}
    if {[catch {set dirFiles [glob $dirPath/*]}] != 0} {
        error "No files in subject directory: $dirPath"}
    foreach fileName $dirFiles {
        if [file isdirectory $fileName] {
            lappend dirList "[file tail $fileName]/"
        } else {
            lappend fileList [file tail $fileName]
        }
    }
   return [list [lsort $dirList] [lsort $fileList]]
}

#------------------------------------------------------------------------------
# Display a list of file names in a column format. This use columns of 14 
# characters 3 blanks.

proc help:DisplayColumns {nameList} {
    set count 0
    set outLine ""
    foreach name $nameList {
        if {$count == 0} {
            append outLine "   "}
        append outLine $name
        if {[incr count] < 4} {
            set padLen [expr 17-[clength $name]]
            if {$padLen < 3} {
               set padLen 3}
            append outLine [replicate " " $padLen]
        } else {
           if {![help:Display $outLine]} {
               return}
           set outLine ""
           set count 0
        }
    }
    if {$count != 0} {
        help:Display $outLine}
    return
}


#------------------------------------------------------------------------------
# Help command main.

proc help {{subject {}}} {
    global TCLENV

    set TCLENV(help:lineCnt) 0

    # Special case "help help", so we can get it at any level.

    if {($subject == "help") || ($subject == "?")} {
        help:DisplayFile "$TCLENV(help:root)/help"
        return
    }

    set request [help:EvalPath $subject]
    set requestPath "$TCLENV(help:root)$request"

    if {![file exists $requestPath]} {
        error "Help:\"$request\" does not exist"}
    
    if [file isdirectory $requestPath] {
        set dirList [help:ListDir $requestPath]
        set subList  [lindex $dirList 0]
        set fileList [lindex $dirList 1]
        if {[llength $subList] != 0} {
            help:Display "\nSubjects available in $request:"
            help:DisplayColumns $subList
        }
        if {[llength $fileList] != 0} {
            help:Display "\nHelp files available in $request:"
            help:DisplayColumns $fileList
        }
    } else {
        help:DisplayFile $requestPath
    }
    return
}


#------------------------------------------------------------------------------
# Helpcd main.
#   
# The name of the new current directory is assembled from the current 
# directory and the argument.  The name will be flatten and any trailing
# "/" will be removed, unless the name is just "/".

proc helpcd {{dir /}} {
    global TCLENV

    set request [help:EvalPath $dir]
    set requestPath "$TCLENV(help:root)$request"

    if {![file exists $requestPath]} {
        error "Helpcd: \"$request\" does not exist"}
    
    if {![file isdirectory $requestPath]} {
        error "Helpcd: \"$request\" is not a directory"}

    set TCLENV(help:curDir) $request
    return    
}

#------------------------------------------------------------------------------
# Helpcd main.

proc helppwd {} {
        global TCLENV
        echo "Current help subject directory: $TCLENV(help:curDir)"
}

#==============================================================================
#     Tcl apropos command.  (see Tcl shell manual)
#------------------------------------------------------------------------------

proc apropos {name} {
    global TCLENV

    set TCLENV(help:lineCnt) 0

    set aproposCT [scancontext create]
    scanmatch -nocase $aproposCT $name {
        set path [lindex $matchInfo(line) 0]
        set desc [lrange $matchInfo(line) 1 end]
        if {![help:Display [format "%s - %s" $path $desc]]} {
            return}
    }
    foreach brief [glob -nocomplain $TCLENV(help:root)/*.brf] {
        set briefFH [open $brief]
        scanfile $aproposCT $briefFH
        close $briefFH
    }
    scancontext delete $aproposCT
}

#------------------------------------------------------------------------------
# One time initialization done when the file is sourced.
#
global TCLENV TCLPATH

set TCLENV(help:root) [searchpath $TCLPATH help]
set TCLENV(help:curDir) "/"
set TCLENV(help:outBuf) {}
