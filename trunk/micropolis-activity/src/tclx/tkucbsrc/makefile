#
# Makefile --
#
# Makefile for building special version of tk source to support Extended Tcl.
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
# $Id: Makefile,v 2.0 1992/10/16 04:54:52 markd Rel $
#------------------------------------------------------------------------------
#

include ../config.mk
include ../config/$(TCL_CONFIG_FILE)
SHELL=/bin/sh

#------------------------------------------------------------------------------

CFLAGS= $(OPTIMIZE_FLAG) $(XCFLAGS) -I../src -I../$(TCL_TK_DIR) \
        -I../$(TCL_UCB_DIR) -I$(XHOME)/include \
	$(MEM_DEBUG_FLAGS) $(SYS_DEP_FLAGS) \
        -DTK_VERSION=\"2.2/\"

#------------------------------------------------------------------------------
#
# Targets to build:

LIBOBJS   = tkevent.o
OBJS      = main.o $(LIBOBJS)

#------------------------------------------------------------------------------

all: made.tmp

made.tmp: $(OBJS)
	-$(AR) d ../libtk.a $(LIBOBJS)
	$(AR) cr ../libtk.a $(LIBOBJS)
	touch made.tmp

#------------------------------------------------------------------------------

clean:
	touch junk~
	rm -f *~ $(OBJS) made.tmp
