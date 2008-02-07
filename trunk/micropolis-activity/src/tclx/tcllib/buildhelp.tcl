#
# buildhelp.tcl --
#
# Program to extract help files from TCL manual pages or TCL script files.
# The help directories are built as a hierarchical tree of subjects and help
# files.  
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
# $Id: buildhelp.tcl,v 2.1 1992/10/25 17:07:40 markd Exp $
#------------------------------------------------------------------------------
#
# For nroff man pages, the areas of text to extract are delimited with:
#
#     '@help: subjectdir/helpfile
#     '@endhelp
#
# start in column one. The text between these markers is extracted and stored
# in help/subjectdir/help.  The file must not exists, this is done to enforced 
# cleaning out the directories before help file generation is started, thus
# removing any stale files.  The extracted text is run through:
#
#     nroff -man|col -xb   {col -b on BSD derived systems}
#
# If there is other text to include in the helpfile, but not in the manual 
# page, the text, along with nroff formatting commands, may be included using:
#
#     '@:Other text to include in the help page.
#
# A entry in the brief file, used by apropos my be included by:
#
#     '@brief: Short, one line description
#
# These brief request must occur with in the bounds of a help section.
#
# If some header text, such as nroff macros, need to be preappended to the
# text streem before it is run through nroff, then that text can be bracketed
# with:
#
#     '@header
#     '@endheader
#
# If multiple header blocks are encountered, they will all be preappended.
#
# For TCL script files, which are indentified because they end in ".tcl",
# the text to be extracted is delimited by:
#
#    #@help: subjectdir/helpfile
#    #@endhelp
#
# And brief lines are in the form:
#
#     #@brief: Short, one line description
#
# The only processing done on text extracted from .tcl files it to replace
# the # in column one with a space.
#
#
#-----------------------------------------------------------------------------
# 
# To run this program:
#
#   tcl buildhelp.tcl [-m mergeTree] -b brief.brf helpDir file-1 file-2 ...
#
# o -m mergeTree is a tree of help code, plus a brief file to merge with the
#   help files that are to be extracted.  This will become part of the new
#   help tree.  Used to merge in the documentation from UCB Tcl.
# o -b specified the name of the brief file to create form the @brief entries.
#   It must have an extension of ".brf".
# o helpDir is the help tree root directory.  helpDir should  exists, but any
#   subdirectories that don't exists will be created.  helpDir should be
#   cleaned up before the start of manual page generation, as this program
#   will not overwrite existing files.
# o file-n are the nroff manual pages (.man) or .tcl or .tlib files to extract
#   the help files from.
#
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Truncate a file name of a help file if the system does not support long
# file names.  If the name starts with `Tcl_', then this prefix is removed.
# If the name is then over 14 characters, it is truncated to 14 charactes
#  
proc TruncFileName {pathName} {
    global G_truncFileNames

    if {!$G_truncFileNames} {
        return $pathName}
    set fileName [file tail $pathName]
    if {"[crange $fileName 0 3]" == "Tcl_"} {
        set fileName [crange $fileName 4 end]}
    set fileName [crange $fileName 0 13]
    return "[file dirname $pathName]/$fileName"
}

#-----------------------------------------------------------------------------
# Proc to ensure that all directories for the specified file path exists,
# and if they don't create them.  Don't use -path so we can set the
# permissions.

proc EnsureDirs {filePath} {
    set dirPath [file dirname $filePath]
    if [file exists $dirPath] return
    foreach dir [split $dirPath /] {
        lappend dirList $dir
        set partPath [join $dirList /]
        if [file exists $partPath] continue

        mkdir $partPath
        chmod u=rwx,go=rx $partPath
    }
}


#-----------------------------------------------------------------------------
#
# Proc to extract nroff text to use as a header to all pass to nroff when
# processing a help file.
#    manPageFH - The file handle of the manual page.
#

proc ExtractNroffHeader {manPageFH} {
    global nroffHeader
    while {[gets $manPageFH manLine] >= 0} {
        if {[string first "'@endheader" $manLine] == 0} {
            break;
            }
        if {[string first "'@:" $manLine] == 0} {
            set manLine [csubstr manLine 3 end]
            }
        append nroffHeader "$manLine\n"
        }
}

#-----------------------------------------------------------------------------
#
# Proc to extract a nroff help file when it is located in the text.
#    manPageFH - The file handle of the manual page.
#    manLine - The '@help: line starting the data to extract.
#

proc ExtractNroffHelp {manPageFH manLine} {
    global G_helpDir nroffHeader G_briefHelpFH G_colArgs

    set helpName [string trim [csubstr $manLine 7 end]]
    set helpFile [TruncFileName "$G_helpDir/$helpName"]
    if {[file exists $helpFile]} {
        error "Help file already exists: $helpFile"}
    EnsureDirs $helpFile
    set helpFH [open "| nroff -man | col $G_colArgs > $helpFile" w]
    echo "    creating help file $helpName"

    # Nroff commands from .TH macro to get the formatting right.  The `\n'
    # are newline separators to output, the `\\n' become `\n' in the text.
        
    puts $helpFH ".ad b\n.PD\n.nrIN \\n()Mu\n.nr)R 0\n.nr)I \\n()Mu"
    puts $helpFH ".nr)R 0\n.\}E\n.DT\n.na\n.nh"
    puts $helpFH $nroffHeader
    set foundBrief 0
    while {[gets $manPageFH manLine] >= 0} {
        if {[string first "'@endhelp" $manLine] == 0} {
            break;
        }
        if {[string first "'@brief:" $manLine] == 0} {
            if $foundBrief {
                error {Duplicate "'@brief" entry"}
            }
            set foundBrief 1
	    puts $G_briefHelpFH "$helpName\t[csubstr $manLine 8 end]"
            continue;
        }
        if {[string first "'@:" $manLine] == 0} {
            set manLine [csubstr $manLine 3 end]
        }
        if {[string first "'@help" $manLine] == 0} {
            error {"'@help" found within another help section"}
        }
        puts $helpFH $manLine
        }
    close $helpFH
    chmod a-w,a+r $helpFile
}

#-----------------------------------------------------------------------------
#
# Proc to extract a tcl script help file when it is located in the text.
#    ScriptPageFH - The file handle of the .tcl file.
#    ScriptLine - The #@help: line starting the data to extract.
#

proc ExtractScriptHelp {ScriptPageFH ScriptLine} {
    global G_helpDir G_briefHelpFH
    set helpName [string trim [csubstr $ScriptLine 7 end]]
    set helpFile "$G_helpDir/$helpName"
    if {[file exists $helpFile]} {
        error "Help file already exists: $helpFile"}
    EnsureDirs $helpFile
    set helpFH [open $helpFile w]
    echo "    creating help file $helpName"
    set foundBrief 0
    while {[gets $ScriptPageFH ScriptLine] >= 0} {
        if {[string first "#@endhelp" $ScriptLine] == 0} {
            break;
        }
        if {[string first "#@brief:" $ScriptLine] == 0} {
            if $foundBrief {
                error {Duplicate "#@brief" entry"}
            }
            set foundBrief 1
	    puts $G_briefHelpFH "$helpName\t[csubstr $ScriptLine 8 end]"
            continue;
        }
        if {[string first "#@help" $ScriptLine] == 0} {
            error {"#@help" found within another help section"}
        }
        if {[clength $ScriptLine] > 1} {
            set ScriptLine " [csubstr $ScriptLine 1 end]"
        } else {
            set ScriptLine ""
        }
        puts $helpFH $ScriptLine
        }
    close $helpFH
    chmod a-w,a+r $helpFile
}

#-----------------------------------------------------------------------------
#
# Proc to scan a nroff manual file looking for the start of a help text
# sections and extracting those sections.
#    pathName - Full path name of file to extract documentation from.
#

proc ProcessNroffFile {pathName} {
   global G_nroffScanCT G_scriptScanCT nroffHeader

   set fileName [file tail $pathName]

   set nroffHeader {}
   set manPageFH [open $pathName r]
   echo "    scanning $pathName"
   set matchInfo(fileName) [file tail $pathName]
   scanfile $G_nroffScanCT $manPageFH
   close $manPageFH
}

#-----------------------------------------------------------------------------
#
# Proc to scan a Tcl script file looking for the start of a
# help text sections and extracting those sections.
#    pathName - Full path name of file to extract documentation from.
#

proc ProcessTclScript {pathName} {
   global G_scriptScanCT nroffHeader

   set scriptFH [open "$pathName" r]

   echo "    scanning $pathName"
   set matchInfo(fileName) [file tail $pathName]
   scanfile $G_scriptScanCT $scriptFH
   close $scriptFH
}

#-----------------------------------------------------------------------------
# Proc to copy the help merge tree, excluding the brief file and RCS files
# 

proc CopyMergeTree {helpDirPath mergeTree} {
    if {"[cindex $helpDirPath 0]" != "/"} {
        set helpDirPath "[pwd]/$helpDirPath"
    }
    set oldDir [pwd]
    cd $mergeTree

    set curHelpDir "."

    for_recursive_glob mergeFile {.} {
        if [string match "*/RCS/*" $mergeFile] continue

        set helpFile "$helpDirPath/$mergeFile"
        if [file isdirectory $mergeFile] continue

        if {[file exists $helpFile]} {
            error "Help file already exists: $helpFile"}
        EnsureDirs $helpFile
        set inFH [open $mergeFile r]
        set outFH [open $helpFile w]
        copyfile $inFH $outFH
        close $outFH
        close $inFH
        chmod a-w,a+r $helpFile
    }
    cd $oldDir
}

#-----------------------------------------------------------------------------
# GenerateHelp: main procedure.  Generates help from specified files.
#    helpDirPath - Directory were the help files go.
#    mergeTree - Help file tree to merge with the extracted help files.
#    briefFile - The name of the brief file to create.
#    sourceFiles - List of files to extract help files from.

proc GenerateHelp {helpDirPath briefFile mergeTree sourceFiles} {
    global G_helpDir G_truncFileNames G_nroffScanCT
    global G_scriptScanCT G_briefHelpFH G_colArgs

    echo ""
    echo "Begin building help tree"

    # Determine version of col command to use (no -x on BSD)
    if {[system {col -bx </dev/null >/dev/null 2>&1}] != 0} {
        set G_colArgs {-b}
    } else {
        set G_colArgs {-bx}
    }
    set G_helpDir [glob $helpDirPath]

    if {![file isdirectory $G_helpDir]} {
        error [concat "$G_helpDir is not a directory or does not exist. "  
                      "This should be the help root directory"]
    }
        
    set status [catch {set tmpFH [open $G_helpDir/AVeryVeryBigFileName w]}]
    if {$status != 0} {
        set G_truncFileNames 1
    } else {
        close $tmpFH
        unlink $G_helpDir/AVeryVeryBigFileName
        set G_truncFileNames 0
    }

    set G_nroffScanCT [scancontext create]

    scanmatch $G_nroffScanCT "^'@help:" {
        ExtractNroffHelp $matchInfo(handle) $matchInfo(line)
        continue
    }

    scanmatch $G_nroffScanCT "^'@header" {
        ExtractNroffHeader $matchInfo(handle)
        continue
    }
    scanmatch $G_nroffScanCT "^'@endhelp" {
        error [concat {"'@endhelp" without corresponding "'@help:"} \
                 ", offset = $matchInfo(offset)"]
    }
    scanmatch $G_nroffScanCT "^'@brief" {
        error [concat {"'@brief" without corresponding "'@help:"}
                 ", offset = $matchInfo(offset)"]
    }

    set G_scriptScanCT [scancontext create]
    scanmatch $G_scriptScanCT "^#@help:" {
        ExtractScriptHelp $matchInfo(handle) $matchInfo(line)
    }

    if ![lempty $mergeTree] {
        echo "    Merging tree: $mergeTree"
        CopyMergeTree $helpDirPath $mergeTree
    }


    if {[file extension $briefFile] != ".brf"} {
        puts stderr "Brief file \"$briefFile\" must have an extension \".brf\""
        exit 1
    }
    if [file exists $G_helpDir/$briefFile] {
        puts stderr "Brief file \"$G_helpDir/$briefFile\" already exists"
        exit 1
    }
    set G_briefHelpFH [open "|sort > $G_helpDir/$briefFile" w]

    foreach manFile $sourceFiles {
        set manFile [glob $manFile]
        set ext [file extension $manFile]
        if {"$ext" == ".man"} {
            set status [catch {ProcessNroffFile $manFile} msg]
        } else {
            set status [catch {ProcessTclScript $manFile} msg]
        }
        if {$status != 0} {
            echo "Error extracting help from: $manFile"
            echo $msg
            global errorInfo interactiveSession
            if {!$interactiveSession} {
                echo $errorInfo
                exit 1
            }
        }
    }

    close $G_briefHelpFH
    chmod a-w,a+r $G_helpDir/$briefFile
    echo "*** completed extraction of all help files"
}

#-----------------------------------------------------------------------------
# Print a usage message and exit the program
proc Usage {} {
    puts stderr {Wrong args: [-m mergetree] -b briefFile helpdir manfile1 [manfile2..]}
    exit 1
}

#-----------------------------------------------------------------------------
# Main program body, decides if help is interactive or batch.

if {$interactiveSession} {
    echo "To extract help, use the command:"
    echo {GenerateHelp helpdir -m mergetree file-1 file-2 ...}
} else {
    set mergeTree {}
    set briefFile {}
    while {[string match "-*" [lindex $argv 0]]} {
        set flag [lvarpop argv 0]
        case $flag in {
            "-m" {set mergeTree [lvarpop argv]}
            "-b" {set briefFile [lvarpop argv]}
            default Usage
        }
    }
    if {[llength $argv] < 2} {
        Usage
    }
    if [lempty $briefFile] {
       puts stderr {must specify -b argument}
       Usage 
    }
    GenerateHelp [lindex $argv 0] $briefFile $mergeTree [lrange $argv 1 end]
   
}
