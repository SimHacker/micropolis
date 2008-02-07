#
# Config.mk --
#
#   Master configuration file for Extended Tcl.  This should be the only
# file you have to modify to get Extended Tcl to work.
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
# $Id: Config.mk,v 2.3 1992/11/09 07:33:02 markd Exp $
#------------------------------------------------------------------------------
#

#==============================================================================
# Configuration file specification.  Set the macro TCL_CONFIG_FILE to the 
# name of the file to use in the config directory (don't include the directory
# name).  If you find problems with these files or have new onces please send
# them to us (tcl-project@neosoft.com).  At the end of this file is a
# description of all the flags that can be set in the config file.
#
TCL_CONFIG_FILE=linux

#==============================================================================
#
# Configuration section.  Modify this section to set some general options and
# selecting a config file for a specific Unix implementation.
#
#==============================================================================

#------------------------------------------------------------------------------
# Location of the UCB Tcl distribution relative to this directory.  TclX works
# with Tcl 6.3 and Tcl 6.4.
#
TCL_UCB_DIR=../tcl

#------------------------------------------------------------------------------
# If you are a Tk user and would like to build a version "wish", the Tk shell,
# that includes the TclX command set, define TCL_TK_SHELL=wish and the
# location of your Tk directory in TCL_TK_DIR relative to this directory. If
# you do not want a "wish" compiled, don't define TCL_TK_SHELL. The libraries
# required to link Tk are defined in the system specific sections below.
# TK_LIBRARY must be set to the same value specified in the compile of the Tk
# source.  If TCL_TK_SHELL is define, Tk manual pages will be installed by the
# install script.
#
TCL_TK_SHELL=wish
TCL_TK_DIR=../tk
TK_LIBRARY=/usr/local/lib/tk
XPM_LIBS=-L/usr/X11R6/lib -lXpm 

#------------------------------------------------------------------------------
# Compiler debug/optimization/profiling flag to use.  Not that if debugging or
# profiling is enabled, the DO_STRIPPING option below must be disabled.
#

CC=gcc

OPTIMIZE_FLAG=-O3 -DIS_LINUX
#OPTIMIZE_FLAG=-g -DIS_LINUX

#------------------------------------------------------------------------------
# Stripping of the final tclshell binary.  Specify `true' if the binary is to
# be stripped (optimized case) or specify `false' if the binary is not to be
# stripped (debugging case).
#
DO_STRIPPING=false
#DO_STRIPPING=true

#------------------------------------------------------------------------------
# Definition of the compiler you want to use, as well as extra flags for the
# compiler and linker.  Also the yacc program you wish to use.
#
AR=ar
XCFLAGS=
XLDFLAGS=
YACC=yacc
#YACC=bison -b y

#------------------------------------------------------------------------------
# If C++ is to be used these should be used.  Specifying CPLUSOBJS includes the
# C++ support code in the Tcl library. CCPLUS is the command to run your C++
# compiler. CPLUSINCL is the location of your standard C++  include files.
#
#CPLUSOBJS=tcl++.o
CCPLUS=CC
CPLUSINCL=/usr/include/CC

#------------------------------------------------------------------------------
# Enable or disable Tcl history in the Tcl flag.  If this macro is set to
# '-DTCL_NOHISTORY', the `history' command will not be available in the Tcl
# shell.  Many people do not find the history command useful and its rather
# large, so this way it can be excluded from the standard shell.  It will
# still be in the library and will be available in other applications if they
# use `Tcl_RecordAndEval'.
#
HISTORY_FLAG=

#------------------------------------------------------------------------------
# Memory debugging defines.  These are only of interest if you are adding C
# code to Tcl or debugging Tcl C code. You probably don't need this unless it
# seems like you have memory problems. They help find memory overwrites and
# leaks.  One or more of the following flags may be specified (in the form
# -DFLAGNAME).
#
#    o TCL_MEM_DEBUG - Turn on memory debugging. 
#    o TCL_SHELL_MEM_LEAK - Dump a list of active memory blocks when the
#      shell exits an eof (requires TCL_MEM_DEBUG).
#
# NOTE: If TCL_MEM_DEBUG is enabled, the Berkeley Tcl distribution must be 
# recompiled with this option as well, or it will not link or may fail
# with some mysterious memory problems.  Same goes for Tk if you are using
# Tk.  If this option is to be used, all code being tested MUST be compiled
# with TCL_MEM_DEBUG and use ckalloc and ckfree for all memory passed between
# the application and Tcl.
#
# An addition a flag MEM_VALIDATE may be specified in the Berkeley Tcl
# compilation to do validation of all memory blocks on each allocation or
# deallocation (very slow).
#
MEM_DEBUG_FLAGS=

#==============================================================================
# Install options sections.  This sections describes the installation options.
# Your might want to change some of these values before installing.
#..............................................................................
#
# o TCL_OWNER - The user that will own all Tcl files after installation.
# o TCL_GROUP - The group that all Tcl files will belong to after installation.
#
TCL_OWNER=bin
TCL_GROUP=bin

# o TCL_DEFAULT - Base name of Tcl default file.  This name will have a
#   version number appended.
#
#TCL_DEFAULT=/etc/default/tcl
TCL_DEFAULT=/usr/local/lib/tcldefault

#
# The directory to install Tcl binary into.
#
TCL_BINDIR=/usr/local/bin

#
# The directory tcl.a library goes into.
#
TCL_LIBDIR=/usr/local/lib

#
# The directory the Tcl .h files go into.
#
TCL_INCLUDEDIR=/usr/local/include

#
# The directory .tcl files and the .tlib library goes into.
#
TCL_TCLDIR=/usr/local/lib/tcl

#==============================================================================
# These defines specify where and how the manual pages are to be installed.
# They are actually defined in the system specific configuration file in the
# config directory.  Install manual pages is somewhat problematic, so a global
# option not to install manual pages is provided. Since there are so many
# manual pages provided, they are placed together in one Tcl manual page
# directory, rather than splitting into the standard manual pages directories.
# The actual definitions of these variables are set for in the system
# dependent file.  You might want to modify these values.
#..............................................................................

#
# Set to 1 to install manual files, to 0 to not install manual files.
#

TCL_MAN_INSTALL=1

#
# o TCL_MAN_BASEDIR - Base manual directory where all of the man.* and cat.*
#   directories live.
#
TCL_MAN_BASEDIR=/usr/local/man

#
# o TCL_MAN_CMD_SECTION - Section for Tcl command  manual pages. Normal `1' or
#   `C'.  You might perfer TCL since there are some many.
#
# o TCL_MAN_FUNC_SECTION - Section for Tcl C level function manual pages.
#   In some cases it might be desirable install all manual pages in one
#   section, in this case, the value should be the same as TCL_MAN_CMD_SECTION.
#
#TCL_MAN_CMD_SECTION=TCL
#TCL_MAN_FUNC_SECTION=TCL
TCL_MAN_CMD_SECTION=1
TCL_MAN_FUNC_SECTION=3

#
# o TK_MAN_CMD_SECTION - Section for Tk command  manual pages. Normal `1' or
#   `C'.  You might perfer TK since there are some many.
#
# o TK_MAN_FUNC_SECTION - Section for Tk C level function manual pages.
#   In some cases it might be desirable install all manual pages in one
#   section, in this case, the value should be the same as TK_MAN_CMD_SECTION.
#
#TK_MAN_CMD_SECTION=TK
#TK_MAN_FUNC_SECTION=TK
TK_MAN_CMD_SECTION=1
TK_MAN_FUNC_SECTION=3


#..............................................................................
# The rest of the manual page install options are set in the system dependent
# configuration file (config/*)
#

# o TCL_MAN_SEPARATOR - The separator character used in the directory name
#   of the cat* and man* manual directories.  This is usually empty or 
#   a period.
#
# o TCL_MAN_STYLE - The style of manual management the system has.It is
#   a string with one of the following values:
#      o SHORT - Short file name installation (an index can be generated).
#      o LONG - Long file name installation, a link will be made for each
#        name the manual page is to be available under.
#   This flag is optional, if omitted LONG is assumed.
#
# o TCL_MAN_INDEX - If 1, then a manual page index will be build in the manual
#   base directory named index.TCL. Not useful if TCL_MAN_STYLE is LONG. Each
#   line in the file has the form:
#       mansubject manfile section
#   This flag is optional, if omitted 0 is assumed.
#

#==============================================================================
# System specific configuration.  A system configuration file in the config
# directory defines the following mactos required for your version of Unix.
# In addition to the options defined in the Berkeley source the following
# options can be defined here.  This information will help you build your own
# system configuration if one is not supplied here.  The configuration file
# name is specified an the end of this section.
#
#    o SYS_DEP_FLAGS - The system dependency flags.  The following options are
#      available, these should be defined using -Dflag.
#
#      o TCL_HAVE_SETLINEBUF - Define if the `setlinebuf' is available as part
#        of stdio.
#
#      o TCL_32_BIT_RANDOM - Define if the `rand' function returns a value in
#        the range 0..(2^31)-1, leave undefined if `rand' returns a value
#        in the range 0..(2^15)-1.
#
#      o TCL_NO_SELECT - The select call is not available.
#
#      o TCL_NEED_SYS_SELECT_H - Define if <sys/select.h> is required. May not
#        need it, even if it is there.
#
#      o TCL_USE_BZERO_MACRO - Use a macro to define bzero for the select
#        FD_ZERO macro.
#
#      o TCL_POSIX_SIG - Set if posix signals are available (sigaction, etc).
#
#      o TCL_HAVE_CATGETS - Set if XPG/3 message catalogs are available
#        (catopen, catgets, etc).
#
#      o TCL_TM_GMTOFF - Set if the seconds east of GMT field in struct tm is
#        names 'tm_gmtoff'.  Not set if its is names 'tm_tzadj'.
#
#      o TCL_TIMEZONE_VAR - If the timezone varaible is used in place of 
#        one of the fields from struct tm.
#
#      o TCL_NEED_TIME_H - Set if time.h is required.
#
#      o TCL_SIG_PROC_INT - Set if signal functions return int rather than
#        void.
#
#      o TCL_NO_ITIMER - Set if setitimer is not available.
#
#      o TCL_IEEE_FP_MATH - Set if IEEE 745-1985 error reporting via the value
#        a function returns is to be used instead of the standard matherr
#        mechanism.
#
#      o TCL_NO_FILE_LOCKING - Set if the fcntl system call does not support
#        file locking.
#
#      o TCL_DUP_CLK_TCK - On some systems that have both time.h and
#        sys/time.h, tclUnix.h defines CLK_TCK then tclExtdInt.h includes
#        time.h and you get a redefiniton warning.  Set this flag to prevent
#        the warning (its not safe to just unset it).
#
#    o LIBS - The flags to specify when linking the tclshell.
#
#    o TCL_TK_LIBS - The libraries to link the TK wish program.  This should
#      also include libraries specified for LIBS, as both values may not be
#      used together due to library ordering constraints.
#
#    o RANLIB_CMD - Either `ranlib' if ranlib is required or `true' if ranlib
#      should not be used.
#
#    o MCS_CMD - Command to delete comments from the object file comment
#      section, if available.  The command `true' if it's not available.  This
#      makes the object file even smaller after its stipped.
#
#    o SUPPORT_FLAGS - The flags for SUPPORT_OBJS code.  The following options
#      are available, these should be defined using -Dflag.
#
#      o TCL_HAS_TM_ZONE - If if 'struct tm' has the 'tm_zone' field.   Used
#        by strftime.
#
#    o SUPPORT_OBJS - The object files to compile to implement library 
#      functions that are not available on this particular version of Unix or 
#      do not function correctly.  The following are available:
#         o strftime.o
#
#..............................................................................
