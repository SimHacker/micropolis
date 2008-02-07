#
# Makefile --
#
# Makefile for Extended Tcl updates to the standard UCB Tcl.
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
# $Id: Makefile,v 2.0 1992/10/16 04:51:44 markd Rel $
#------------------------------------------------------------------------------
#


include ../config.mk
include ../config/$(TCL_CONFIG_FILE)
SHELL=/bin/sh

#------------------------------------------------------------------------------

CFLAGS=$(OPTIMIZE_FLAG) $(XCFLAGS) -I../$(TCL_UCB_DIR) $(MEM_DEBUG_FLAGS) \
  $(SYS_DEP_FLAGS)

#------------------------------------------------------------------------------

OBJS=tclbasic.o tclexpr.o

#------------------------------------------------------------------------------

all: made.tmp

made.tmp: $(OBJS)
	-$(AR) d  ../libtcl.a $(OBJS)
	$(AR)  cr ../libtcl.a $(OBJS)
	touch made.tmp

#------------------------------------------------------------------------------

clean:
	touch junk~
	-rm -f *~ made.tmp
	-rm -f $(OBJS)
