#
# Makefile  --
#
# Makefile for Extended Tcl ossupport code.
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
# $Id: Makefile,v 2.1 1992/11/07 22:33:46 markd Exp $
#------------------------------------------------------------------------------
#

include ../config.mk
include ../config/$(TCL_CONFIG_FILE)
SHELL=/bin/sh

#------------------------------------------------------------------------------

CFLAGS=$(OPTIMIZE_FLAG) $(XCFLAGS) -I../$(TCL_UCB_DIR) $(MEM_DEBUG_FLAGS) \
  $(SYS_DEP_FLAGS) $(SUPPORT_FLAGS)

#------------------------------------------------------------------------------

all: made.tmp

made.tmp: $(SUPPORT_OBJS)
	$(AR) cr ../libtcl.a $(SUPPORT_OBJS)
	touch made.tmp

#------------------------------------------------------------------------------

clean:
	touch junk~
	-rm -f *~ made.tmp
	-rm -f *.o
