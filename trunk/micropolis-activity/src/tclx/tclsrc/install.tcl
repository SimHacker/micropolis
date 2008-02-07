#
# installTcl.tcl -- 
#
# Tcl program to install Tcl onto the system.
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
# $Id: installTcl.tcl,v 2.0 1992/10/16 04:52:08 markd Rel $
#------------------------------------------------------------------------------
#
# It is run in the following manner:
#
#     tcl installTcl.tcl
#
# This script reads the Extended Tcl Makefile confiugation file (Config.mk)
# and converts the Makefile macros in Tcl variables that control the
# installation.  The following variables are currently used:
#
#   TCL_UCB_DIR             TCL_DEFAULT             TCL_OWNER
#   TCL_GROUP               TCL_BINDIR              TCL_LIBDIR
#   TCL_INCLUDEDIR          TCL_TCLDIR              TCL_MAN_INSTALL
#   TCL_MAN_BASEDIR         TCL_MAN_CMD_SECTION     TCL_MAN_FUNC_SECTION
#   TK_MAN_CMD_SECTION      TK_MAN_FUNC_SECTION     TCL_MAN_STYLE*
#   TCL_MAN_INDEX*          TCL_TK_SHELL*
#
# (ones marked with * are optional)
#
# Notes:
#   o Must be run in the Extended Tcl top level directory.
#   o The routine InstallManPages has code to determine if a manual page
#     belongs to a command or function.  For Tcl the commands are assumed
#     to be in "Tcl.man",  for TclX functions are in TclX.man.  All others
#     are assumed to be functions.  For Tk, all manuals starting with Tk_
#     are assumed to be functions, all others are assumed to be commands.
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#------------------------------------------------------------------------------
# ParseConfigFile --
#
#  Parse a configure file in the current directory and convert all make
#  macros to global Tcl variables.

proc ParseConfigFile {configFile} {
   set cfgFH [open $configFile]
 
   while {[gets $cfgFH line] >= 0} {
      if {[string match {[A-Za-z]*} $line]} {
          set idx [string first "=" $line]
          if {$idx < 0} {
              error "no `=' in: $line"}
          set name  [string trim [csubstr $line 0 $idx]]
          set value [string trim [crange  $line [expr $idx+1] end]]
          global $name
          set $name $value
      }
   }
   close $cfgFH

}

#------------------------------------------------------------------------------
# GiveAwayFile --
#   Give away a file to the Tcl owner and group and set its permissions.
#
# Globals:
#    TCL_OWNER - Owner name for Tcl files.
#    TCL_GROUP - Group nmae for Tcl file.
#------------------------------------------------------------------------------

proc GiveAwayFile {file} {
    global TCL_OWNER TCL_GROUP

    if {[file isdirectory $file]} {
        chmod a+rx,go-w $file
    } else {
        chmod a+r,go-w $file
    }    
    chown [list $TCL_OWNER $TCL_GROUP] $file

} ;# GiveAwayFile

#------------------------------------------------------------------------------
# MakePath --
#
# Make sure all directories in a directory path exists, if not, create them.
#------------------------------------------------------------------------------
proc MakePath {pathlist} {
    foreach path $pathlist {
        set exploded_path [split $path /]
        set thisdir {}
        foreach element $exploded_path {
            append thisdir $element
            if {![file isdirectory $thisdir]} {
                mkdir $thisdir
                GiveAwayFile $thisdir
            }
            append thisdir /
        }
    }
}

#------------------------------------------------------------------------------
# CopyFile -- 
#
# Copy the specified file and change the ownership.  If target is a directory,
# then the file is copied to it, otherwise target is a new file name.
#------------------------------------------------------------------------------

proc CopyFile {sourceFile target} {

    if {[file isdirectory $target]} {
        set targetFile "$target/[file tail $sourceFile]"
    } else {
        set targetFile $target
    }

    unlink -nocomplain $targetFile
    set sourceFH [open $sourceFile r]
    set targetFH [open $targetFile w]
    copyfile $sourceFH $targetFH
    close $sourceFH
    close $targetFH
    GiveAwayFile $targetFile

} ;# CopyFile

#------------------------------------------------------------------------------
# CopyManPage -- 
#
# Copy the specified manual page and change the ownership.  The manual page
# is edited to remove change bars (.VS and .VE macros). If target is a
# directory, then the file is copied to it, otherwise target is a new file
# name.
#------------------------------------------------------------------------------

proc CopyManPage {sourceFile target} {

    if {[file isdirectory $target]} {
        set targetFile "$target/[file tail $sourceFile]"
    } else {
        set targetFile $target
    }

    unlink -nocomplain $targetFile
    set sourceFH [open $sourceFile r]
    set targetFH [open $targetFile w]
    while {[gets $sourceFH line] >= 0} {
        if [string match {.V[SE]*} $line] continue
        puts $targetFH $line
    }
    close $sourceFH
    close $targetFH
    GiveAwayFile $targetFile

} ;# CopyManPage

#------------------------------------------------------------------------------
# CopySubDir --
#
# Recursively copy part of a directory tree, changing ownership and 
# permissions.  This is a utility routine that actually does the copying.
#------------------------------------------------------------------------------

proc CopySubDir {sourceDir destDir} {
    foreach sourceFile [glob -nocomplain $sourceDir/*] {

        if [file isdirectory $sourceFile] {
            set destFile $destDir/[file tail $sourceFile]
            if {![file exists $destFile]} {
                mkdir $destFile}
            GiveAwayFile $destFile
            CopySubDir $sourceFile $destFile
        } else {
            CopyFile $sourceFile $destDir
        }
    }
} ;# CopySubDir

#------------------------------------------------------------------------------
# CopyDir --
#
# Recurisvely copy a directory tree.
#------------------------------------------------------------------------------

proc CopyDir {sourceDir destDir} {

    set cwd [pwd]
    if ![file exists $sourceDir] {
        error "\"$sourceDir\" does not exist"
    }
    if ![file isdirectory $sourceDir] {
        error "\"$sourceDir\" isn't a directory"
    }
    if {![file exists $destDir]} {
        mkdir $destDir
        GiveAwayFile $destDir
    }
    if ![file isdirectory $destDir] {
        error "\"$destDir\" isn't a directory"
    }
    cd $sourceDir
    set status [catch {CopySubDir . $destDir} msg]
    cd $cwd
    if {$status != 0} {
        global errorInfo errorCode
        error $msg $errorInfo $errorCode
    }
}

#------------------------------------------------------------------------------
# GenDefaultFile -- 
#
# Generate the tcl defaults file.
#------------------------------------------------------------------------------

proc GenDefaultFile {defaultFileBase sourceDir} {

    set defaultFile "$defaultFileBase[infox version]"

    if ![file writable [file dirname $defaultFile]] {
        puts stderr "Can't create $defaultFile -- directory is not writable"
        puts stderr "Please reinstall with correct permissions or rebuild"
        puts stderr "Tcl to select a default file where the directory path"
        puts stderr "you specify is writable by you."
        puts stderr ""
        exit 1
    }

    set fp [open $defaultFile w]

    puts $fp "# Extended Tcl [infox version] default file"
    puts $fp ""
    puts $fp "set TCLINIT $sourceDir/TclInit.tcl"
    puts $fp ""
    puts $fp "set TCLPATH $sourceDir"

    close $fp
    GiveAwayFile $defaultFile

} ;# GenDefaultFile

#------------------------------------------------------------------------------
# GetManNames --
#
#   Search a manual page (nroff source) for the name line.  Parse the name
# line into all of the functions or commands that it references.  This isn't
# comprehensive, but it works for all of the Tcl, TclX and Tk man pages.
#
# Parameters:
#   o manFile (I) - The path to the  manual page file.
# Returns:
#   A list contain the functions or commands or {} if the name line can't be
# found or parsed.
#------------------------------------------------------------------------------

proc GetManNames {manFile} {

   set manFH [open $manFile]

   #
   # Search for name line.  Once found, grab the next line that is not a
   # nroff macro.  If we end up with a blank line, we didn't find it.
   #
   while {[gets $manFH line] >= 0} {
       if [regexp {^.SH NAME.*$} $line] {
           break
       }
   }
   while {[gets $manFH line] >= 0} {
       if {![string match ".*" $line]} break
   }
   close $manFH

   set line [string trim $line]
   if {$line == ""} return

   #
   # Lets try and parse the name list out of the line
   #
   if {![regexp {^(.*)(\\-)} $line {} namePart]} {
       if {![regexp {^(.*)(-)} $line {} namePart]} return
   }

   #
   # This magic converts the name line into a list
   #

   if {[catch {join [split $namePart ,] " "} namePart] != 0} return

   return $namePart

}

#------------------------------------------------------------------------------
# SetUpManIndex --
#   Setup generation of manual page index for short manual pages, if required.
# Globals:
#   o TCL_MAN_INDEX - Boolean indicating if a manual page is to be created.
#     If it does not exists, false is assumed.
#   o TCL_MAN_BASEDIR - Base manual directory where all of the man.* and cat.* 
#     directories live.
# Returns:
#   The manual index file handle, or {} if the manual index is not to be
#  generated.
#------------------------------------------------------------------------------

proc SetUpManIndex {} {
    global TCL_MAN_BASEDIR TCL_MAN_INDEX

    if {!([info exists TCL_MAN_INDEX] && [set TCL_MAN_INDEX])} {
        return {}
    }
    set tclIndexFile $TCL_MAN_BASEDIR/index.TCL
    return [open $tclIndexFile w]
}

#------------------------------------------------------------------------------
# FinishUpManIndex --
#   Finish generation of manual page index for short manual pages, if required.
# Parameters:
#   o indexFileHdl - The file handle returned by SetUpManIndex, maybe {}.
# Globals:
#   o TCL_MAN_BASEDIR - Base manual directory where all of the man.* and cat.* 
#     directories live.
#------------------------------------------------------------------------------

proc FinishUpManIndex {indexFileHdl} {
    global TCL_MAN_BASEDIR TCL_MAN_INDEX_MERGE

    if [lempty $indexFileHdl] return

    set tclIndexFile $TCL_MAN_BASEDIR/index.TCL
    close $indexFileHdl
    GiveAwayFile $tclIndexFile

}

#------------------------------------------------------------------------------
# InstallShortMan --
#   Install a manual page on a system that does not have long file names,
#   optionally adding an entry to the man index.
#
# Parameters:
#   o sourceFile - Manual page source file path.
#   o section - Section to install the manual page in.
#   o indexFileHdl - File handle of the current index file being created, or
#     empty if no index is to be created.
# Globals:
#   o TCL_MAN_BASEDIR - Base manual directory where all of the man.* and cat.* 
#     directories live.
#   o TCL_MAN_SEPARATOR - The name separator between the directory and the
#     section.
#------------------------------------------------------------------------------

proc InstallShortMan {sourceFile section indexFileHdl} {
    global TCL_MAN_BASEDIR TCL_MAN_SEPARATOR

    set manNames [GetManNames $sourceFile]
    if [lempty $manNames] {
        set baseName [file tail [file root $sourceFile]]
        puts stderr "Warning: can't parse NAME line for man page: $sourceFile."
        puts stderr "         Manual page only available as: $baseName"
    }

    set manFileBase [file tail [file root $sourceFile]]
    set manFileName "$manFileBase.$section"

    set destManDir "$TCL_MAN_BASEDIR/man$TCL_MAN_SEPARATOR$section"
    set destCatDir "$TCL_MAN_BASEDIR/cat$TCL_MAN_SEPARATOR$section"

    CopyManPage $sourceFile "$destManDir/$manFileName"
    unlink -nocomplain  "$destCatDir/$manFileName"

    if {![lempty $indexFileHdl]} {
        foreach name $manNames {
            puts $indexFileHdl "$name\t$manFileBase\t$section"
        }
    }    
}

#------------------------------------------------------------------------------
# InstallLongMan --
#   Install a manual page on a system that does have long file names.
#
# Parameters:
#   o sourceFile - Manual page source file path.
#   o section - Section to install the manual page in.
# Globals:
#   o TCL_MAN_BASEDIR - Base manual directory where all of the man.* and cat.* 
#     directories live.
#   o TCL_MAN_SEPARATOR - The name separator between the directory and the
#     section.
#------------------------------------------------------------------------------

proc InstallLongMan {sourceFile section} {
    global TCL_MAN_BASEDIR TCL_MAN_SEPARATOR

    set manNames [GetManNames $sourceFile]
    if [lempty $manNames] {
        set baseName [file tail [file root $sourceFile]]
        puts stderr "Warning: can't parse NAME line for man page: $sourceFile."
        puts stderr "         Manual page only available as: $baseName"
        set manNames $baseName
    }

    set destManDir "$TCL_MAN_BASEDIR/man$TCL_MAN_SEPARATOR$section"
    set destCatDir "$TCL_MAN_BASEDIR/cat$TCL_MAN_SEPARATOR$section"

    # Copy file to the first name in the list.

    set firstFile [lvarpop manNames]
    set firstFilePath "$destManDir/$firstFile.$section"

    CopyManPage $sourceFile $firstFilePath
    unlink -nocomplain   "$destCatDir/$firstFile.$section"

    # Link it to the rest of the names in the list.

    foreach manEntry $manNames {
        set destFilePath "$destManDir/$manEntry.$section"
        unlink -nocomplain  $destFilePath
        if {[catch {
                link $firstFilePath $destFilePath
            } msg] != 0} {
            puts stderr "error from: link $firstFilePath $destFilePath"
            puts stderr "    $msg"
        }
        unlink -nocomplain "$destCatDir/$manEntry.$section"
    }

}

#------------------------------------------------------------------------------
# InstallManPage --
#   Install a manual page on a system.
#
# Parameters:
#   o sourceFile - Manual page source file path.
#   o section - Section to install the manual page in.
#   o indexFileHdl - File handle of the current index file being created, or
#     empty if no index is to be created.
# Globals
#   o TCL_MAN_STYLE - SHORT if short manual page names are being used,
#     LONG if long manual pages are being used.
#------------------------------------------------------------------------------

proc InstallManPage {sourceFile section indexFileHdl} {
    global TCL_MAN_STYLE

    if {"$TCL_MAN_STYLE" == "SHORT"} {
        InstallShortMan $sourceFile $section $indexFileHdl
    } else {
        InstallLongMan $sourceFile $section
    }
}

#------------------------------------------------------------------------------
# InstallManPages --
#   Install the manual pages.
#------------------------------------------------------------------------------

proc InstallManPages {} {
    global TCL_UCB_DIR          TCL_TK_SHELL         TCL_TK_DIR
    global TCL_MAN_BASEDIR      TCL_MAN_SEPARATOR    TCL_MAN_STYLE
    global TCL_MAN_CMD_SECTION  TCL_MAN_FUNC_SECTION
    global TK_MAN_CMD_SECTION   TK_MAN_FUNC_SECTION

    if {![info exists TCL_MAN_STYLE]} {
        set TCL_MAN_STYLE LONG
    }
    set TCL_MAN_STYLE [string toupper $TCL_MAN_STYLE]
    case $TCL_MAN_STYLE in {
        {SHORT} {}
        {LONG}  {}
        default {error "invalid value for TCL_MAN_STYLE: `$TCL_MAN_STYLE'"}
    }

    MakePath $TCL_MAN_BASEDIR 
    MakePath "$TCL_MAN_BASEDIR/man$TCL_MAN_SEPARATOR$TCL_MAN_CMD_SECTION"
    MakePath "$TCL_MAN_BASEDIR/cat$TCL_MAN_SEPARATOR$TCL_MAN_CMD_SECTION"
    MakePath "$TCL_MAN_BASEDIR/man$TCL_MAN_SEPARATOR$TCL_MAN_FUNC_SECTION"
    MakePath "$TCL_MAN_BASEDIR/cat$TCL_MAN_SEPARATOR$TCL_MAN_FUNC_SECTION"

    set indexFileHdl [SetUpManIndex]

    # Install all of the actual files.

    echo "    Installing Tcl [info tclversion] man files"
    foreach fileName [glob $TCL_UCB_DIR/doc/*.man] {
        if {[file root $fileName] == "Tcl.man"} {
            set section $TCL_MAN_CMD_SECTION
        } else {
            set section $TCL_MAN_FUNC_SECTION
        }
        InstallManPage $fileName $section $indexFileHdl
    }

    echo "    Installing Extended Tcl [infox version] man files"

    foreach fileName [glob man/*.man] {
        if {[file root $fileName] == "TclX.man"} {
            set section $TCL_MAN_CMD_SECTION
        } else {
            set section $TCL_MAN_FUNC_SECTION
        }
        InstallManPage $fileName $section $indexFileHdl
    }

    if {![info exists TCL_TK_SHELL]} {
        FinishUpManIndex $indexFileHdl
        return
    }

    MakePath "$TCL_MAN_BASEDIR/man$TCL_MAN_SEPARATOR$TK_MAN_CMD_SECTION"
    MakePath "$TCL_MAN_BASEDIR/cat$TCL_MAN_SEPARATOR$TK_MAN_CMD_SECTION"
    MakePath "$TCL_MAN_BASEDIR/man$TCL_MAN_SEPARATOR$TK_MAN_FUNC_SECTION"
    MakePath "$TCL_MAN_BASEDIR/cat$TCL_MAN_SEPARATOR$TK_MAN_FUNC_SECTION"

    echo "    Installing Tk man files"

    foreach fileName [glob $TCL_TK_DIR/doc/*.man] {
        if {![string match "Tk_*" [file root $fileName]]} {
            set section $TK_MAN_CMD_SECTION
        } else {
            set section $TK_MAN_FUNC_SECTION
        }
        InstallManPage $fileName $section $indexFileHdl
    }

    FinishUpManIndex $indexFileHdl

} ;# InstallLongManPages

#------------------------------------------------------------------------------
# Main program code.
#------------------------------------------------------------------------------

echo ""
echo ">>> Installing Extended Tcl [infox version] <<<"

set argc [llength $argv]
if {$argc != 0} {
    puts stderr "usage: tcl installTcl.tcl"
    exit 1
}

#
# Bring in all of the macros defined bu the configuration file.
#
ParseConfigFile Config.mk
ParseConfigFile config/$TCL_CONFIG_FILE

#
# Make sure all directories exists that we will be installing in.
#

MakePath [list $TCL_TCLDIR [file dirname $TCL_DEFAULT] $TCL_BINDIR]
MakePath [list $TCL_LIBDIR $TCL_INCLUDEDIR $TCL_TCLDIR]

echo "    Creating default file: $TCL_DEFAULT[infox version]"
GenDefaultFile $TCL_DEFAULT $TCL_TCLDIR

echo "    Installing `tcl' program in: $TCL_BINDIR"
CopyFile tcl $TCL_BINDIR
chmod +rx $TCL_BINDIR/tcl

echo "    Installing `libtcl.a' library in: $TCL_LIBDIR"
CopyFile libtcl.a $TCL_LIBDIR

echo "    Installing Tcl .h files in: $TCL_INCLUDEDIR"
CopyFile $TCL_UCB_DIR/tcl.h $TCL_INCLUDEDIR
CopyFile $TCL_UCB_DIR/tclHash.h $TCL_INCLUDEDIR
CopyFile src/tclExtend.h $TCL_INCLUDEDIR
CopyFile src/tcl++.h $TCL_INCLUDEDIR

echo "    Installing Tcl run-time files in: $TCL_TCLDIR"
foreach srcFile [glob tcllib/*] {
    if {![file isdirectory $srcFile]} {
        CopyFile $srcFile $TCL_TCLDIR
    }
}

echo "    Installing Tcl help files in: $TCL_TCLDIR/help"
if [file exists $TCL_TCLDIR/help] {
     echo "       Purging old help tree"
     exec rm -rf $TCL_TCLDIR/help
}
CopyDir tcllib/help          $TCL_TCLDIR/help

if [info exists TCL_TK_SHELL] {
    echo "    Installing `wish' program in: $TCL_BINDIR"
    CopyFile wish $TCL_BINDIR
    chmod +rx $TCL_BINDIR/wish

    echo "    Installing `libtk.a' library in: $TCL_LIBDIR"
    CopyFile libtk.a $TCL_LIBDIR

    echo "    Installing `tk.h' in: $TCL_INCLUDEDIR"
    CopyFile $TCL_TK_DIR/tk.h $TCL_INCLUDEDIR
}

foreach file [glob $TCL_TCLDIR/*.tlib] {
    buildpackageindex $file
}

if {$TCL_MAN_INSTALL} {
    InstallManPages
}

echo "     *** TCL IS NOW INSTALLED ***"

