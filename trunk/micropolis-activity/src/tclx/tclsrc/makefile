#
# Makefile --
#
# Makefile for Extended Tcl package library.
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
# $Id: Makefile,v 2.0 1992/10/16 04:51:50 markd Rel $
#------------------------------------------------------------------------------
#

include ../config.mk
include ../config/$(TCL_CONFIG_FILE)
SHELL=/bin/sh

#------------------------------------------------------------------------------

TCL.TLIB=../tcllib/tcl.tlb
TCL.TNDX=../tcllib/tcl.tdx

#------------------------------------------------------------------------------

TCL_SRCS=array.tcl assign.tcl edprocs.tcl forfile.tcl  \
	forrec.tcl globrec.tcl help.tcl packages.tcl \
	pushd.tcl setfuncs.tcl showproc.tcl strfile.tcl \
	compat.tcl convlib.tcl profrep.tcl

#------------------------------------------------------------------------------

all: made.tmp

made.tmp: $(TCL.TLIB)
	touch made.tmp

$(TCL.TLIB): $(TCL_SRCS)
	 cat $(TCL_SRCS) | egrep -v '^#$$|^#  *|^#-|^#=|^  *#' > $@

#------------------------------------------------------------------------------

clean:
	touch junk~
	-rm -f *~ made.tmp $(TCL.TLIB) $(TCL.TNDX)

