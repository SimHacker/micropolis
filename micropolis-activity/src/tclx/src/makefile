#
# Makefile  --
#
# Makefile for Extended Tcl C sources.  This will compile all of Extended Tcl
# and add it to the libtcl.a in the parent directory.  Generates a Tcl shell
# in the parent directory.
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
# $Id: Makefile,v 2.0 1992/10/16 04:51:22 markd Rel $
#------------------------------------------------------------------------------
#

include ../config.mk
include ../config/$(TCL_CONFIG_FILE)
SHELL=/bin/sh

#------------------------------------------------------------------------------

CFLAGS= $(OPTIMIZE_FLAG) $(XCFLAGS) -I../$(TCL_UCB_DIR) $(MEM_DEBUG_FLAGS) \
        $(SYS_DEP_FLAGS)

#------------------------------------------------------------------------------

OBJS= 	main.o     tclxbsrc.o tclxcclk.o tclxchmd.o \
	tclxclck.o tclxclp.o  tclxcret.o tclxdebg.o \
	tclxdup.o  tclxfcmd.o tclxfctl.o tclxfmat.o \
	tclxfsca.o tclxgdat.o tclxgenl.o tclxhndl.o \
	tclxid.o   tclxklst.o tclxlib.o  tclxlist.o \
	tclxmath.o tclxmerr.o tclxmsgc.o tclxproc.o \
	tclxprof.o tclxrexp.o tclxsel.o  tclxsig.o  \
	tclxstr.o  tclxstup.o tclxunix.o tclxutil.o
 
#------------------------------------------------------------------------------

all: made.tmp ../tcl

made.tmp: $(OBJS) $(CPLUSOBJS)
	$(AR) cr ../libtcl.a $(OBJS) $(CPLUSOBJS)
	touch made.tmp

tclxstup.o: patchlvl.h tclxstup.c
	$(CC) -c $(CFLAGS) -DTCL_DEFAULT=\"$(TCL_DEFAULT)\" tclxstup.c

main.o: patchlvl.h main.c
	$(CC) -c $(CFLAGS) $(HISTORY_FLAG) main.c

tclxgdat.c: tclxgdat.y
	$(YACC) tclxgdat.y
	mv y.tab.c tclxgdat.c

../tcl: ../libtcl.a made.tmp 
	$(RANLIB_CMD) ../libtcl.a
	$(CC) $(CFLAGS) main.o ../libtcl.a $(LIBS) $(XLDFLAGS) -o ../tcl
	if $(DO_STRIPPING) ; then \
	    strip ../tcl; \
	    $(MCS_CMD) ../tcl; fi


#------------------------------------------------------------------------------
# This is a painful situation.  The file tcl++.h requires string.h, which is
# redefined in the UCB Tcl directory.  We force the compiler to search the
# C++ directories first.

tclplus.o: tclplus.c tclplus.h
	$(CCPLUS) -c -I $(CCPLUSINCL) $(CFLAGS) $(HISTORY_FLAG) tclplus.cc

#------------------------------------------------------------------------------
# This is just to test if it compiles.

tclplus: ../tclplus

../tclplus: mainplus.o
	$(CCPLUS) $(CFLAGS) main++.o ../libtcl.a $(LIBS) $(XLDFLAGS) \
	-o ../tcl++

mainplus.o: tclplus.h mainplus.c
	$(CCPLUS) -c -I /usr/include/CC $(CFLAGS) $(HISTORY_FLAG) mainplus.C


#------------------------------------------------------------------------------

clean:
	touch junk~
	-rm -f made.tmp tclxgdat.c
	-rm -f *~ *.o ../tcl ../tclplus

