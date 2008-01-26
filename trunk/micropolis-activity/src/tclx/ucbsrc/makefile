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

tclbasic.c: ../$(TCL_UCB_DIR)/tclbasic.c
	rm -f tclbasic.c
	sed -f tclbasic.sed <../$(TCL_UCB_DIR)/tclbasic.c >tclbasic.c

tclexpr.c: ../$(TCL_UCB_DIR)/tclexpr.c
	rm -f tclexpr.c
	sed -f tclexpr.sed <../$(TCL_UCB_DIR)/tclexpr.c >tclexpr.c

#------------------------------------------------------------------------------

clean:
	touch junk~
	-rm -f *~ made.tmp tclbasic.c tclexpr.c
	-rm -f $(OBJS)
