# Makefile.linux
#
# This Makefile is for use when distributing Tcl to the outside world.
# It is normally set up by running the "config" script.  Before modifying
# this file by hand, you should read through the "config" script to see
# what it does.
#
# Some changes you may wish to make here:
#
# 1. To compile for non-UNIX systems (so that only the non-UNIX-specific
# commands are available), change the OBJS line below so it doesn't
# include ${UNIX_OBJS}.  Also, add the switch "-DTCL_GENERIC_ONLY" to
# CFLAGS.  Lastly, you'll have to provide your own replacement for the
# "panic" procedure (see panic.c for what the current one does).
#
# 2. ANSI-C procedure prototypes are turned on by default if supported
# by the compiler.  To turn them off, add "-DNO_PROTOTYPE" to CFLAGS
# below.
#
# 3. If you've put the Tcl script library in a non-standard place, change
# the definition of TCL_LIBRARY to correspond to its location on your
# system.
#

TCL_LIBRARY =	/usr/local/lib/tcl

CC =		gcc

CFLAGS =	-O3 -I. -DTCL_LIBRARY=\"${TCL_LIBRARY}\" -DIS_LINUX
#CFLAGS =	-g -I. -DTCL_LIBRARY=\"${TCL_LIBRARY}\" -DIS_LINUX

GENERIC_OBJS = \
	regexp.o \
	tclassem.o \
	tclbasic.o \
	tclckall.o \
	tclcmdah.o \
	tclcmdil.o \
	tclcmdmz.o \
	tclexpr.o \
	tclget.o \
	tclhash.o \
	tclhist.o \
	tclparse.o \
	tclproc.o \
	tclutil.o \
	tclvar.o

UNIX_OBJS = \
	panic.o \
	tclenv.o \
	tclglob.o \
	tclunxaz.o \
	tclunxst.o \
	tclunxut.o 

COMPAT_OBJS = 

OBJS = ${GENERIC_OBJS} ${UNIX_OBJS} ${COMPAT_OBJS}

all: libtcl.a

libtcl.a: ${OBJS} ${COMPAT_OBJS}
	rm -f libtcl.a
	ar cr libtcl.a ${OBJS}
	ranlib libtcl.a

tcltest: tcltest.o libtcl.a
	${CC} ${CFLAGS} tcltest.o libtcl.a -o tcltest

test: tcltest
	( echo cd tests ; echo source all ) | ./tcltest

clean:
	rm -f ${OBJS} libtcl.a tcltest.o tcltest

# The following target is used during configuration to compile
# a test program to see if certain facilities are available on
# the system.

configtest:
	${CC} ${CFLAGS} test.c

${OBJS}: tcl.h tclhash.h tclint.h
${UNIX_OBJS}: tclunix.h
