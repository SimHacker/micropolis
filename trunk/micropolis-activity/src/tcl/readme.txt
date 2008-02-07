Tcl

by John Ousterhout
University of California at Berkeley
ouster@sprite.berkeley.edu

1. Introduction
---------------

This directory contains the sources and documentation for Tcl, an
embeddable tool command language.  The information here corresponds
to release 6.4.  This release is identical to the 6.3 release except
for a few bug fixes and one new feature (the Tcl_GlobalEval procedure).
The file "changes" has a complete list of all changes made to Tcl,
with incompatible changes specially marked. Tcl 6.4 has no incompatible
changes relative to 6.3.

For an introduction to the facilities provided by Tcl, see the paper
``Tcl:  An Embeddable Command Language'', in the Proceedings of the
1990 Winter USENIX Conference.  A copy of that paper is included here
in Postscript form in the file "doc/usenix.ps" and in text form in the
file "doc/usenix.text".  However, the paper corresponds to a much
earlier version of Tcl (approximately version 3.0), so some details
of the paper may not be correct anymore and there are many new features
not documented in the paper.

2. Documentation
----------------

The "doc" subdirectory contains a complete set of manual entries
for Tcl.  The file "doc/Tcl.man" gives an overall description of the
Tcl language and describes the core Tcl commands.  The other ".man"
files in "doc" describe the library procedures that Tcl provides for
Tcl-based applications.  Read the "Tcl" man page first.  To print any
of the man pages, cd to the "doc" directory and invoke your favorite
variant of troff using the normal -man macros, for example

		ditroff -man <file>

where <file> is the name of the man page you'd like to print.

3. Machines supported
---------------------

If you follow the directions below, this release should compile
and run on the following configurations either "out of the box"
or with only trivial changes:

    - Sun-3's, Sun-4's, SPARCstation-1's and SPARCstation-2's running
      many variants of SunOS, such as 4.1.
    - DECstation-3100's and DECstation-5000's running many versions of
      Ultrix, such as 2.0 and 4.2.
    - DEC VAXes running many versions of Ultrix or BSD UNIX. 
    - Intel 386 based systems running SCO Unix 3.2v2.
    - Intel 386 based systems running SCO Xenix 2.3.3.
    - Intel 386 based systems running Bell-Tech (now Intel) Unix 3.2.0.
    - Silicon Graphics systems running IRIX 4.0.
    - Various H-P machines running versions of HP-UX such as 7.05
    - Sequent Symmetry running versions of Dynix/ptx such as v1.2.4

If you find problems running Tcl on any of the above configurations,
please let me know.  Also, if you are able to compile Tcl and run the
test suite successfully on configurations not listed above, please
let me know and tell me what changes, if any, you needed to make to
do it.  I'd like to keep the above list up-to-date and continue to
improve the portability of Tcl.

Tcl can be used on many other configurations with only a few
modifications.  The file "porting.notes" contains information sent to
me about what it took to get Tcl to run on various other configurations.
I make no guarantees that this information is accurate or complete, but
you may find it useful.  If you get Tcl running on a new configuration,
I'd be deligted to receive new information to add to "porting.notes".

4. Compiling Tcl
----------------

To compile Tcl on any of the configurations listed above, or systems
similar to them, do the following:

    (a) If your system isn't one of the ones listed above, look in the
        file "porting.notes" to see if your system is listed there.  This
	file contains additonal notes on getting Tcl to run on various
	other systems.

    (b) Edit the "set" commands at the beginning of the file "config"
        if necessary to correspond to your system configuration (they
	should already be right for most versions of Unix).

    (c) Type "./config" in the top-level directory. "Config" is a script
        that pokes around in your system to see if certain almost-standard
	things are missing (header files, library procedures, etc.);
	if your system doesn't seem to have them, it configures Tcl to
	use its own copies of these things instead (Tcl's copies are
	kept in the "compat" subdirectory).  Config prints out messages
	for all the substitutions it made.  You can ignore any of the
	messages unless they say "ERROR!!";  in this case something is
	fundamentally wrong and the config script couldn't handle your
	system configuration.

    (d) Type "make" to compile the library.  This will create the Tcl
	library in "libtcl.a".  The Makefile should work without any
	modifications but you may wish to personalize it, e.g. to
	turn on compiler optimization.

    (e) If the combination of "config" and "make" doesn't work for you,
        then I suggest the following approach:
	    - Start again with a fresh copy of the distribution.
	    - Set the #defines that appear at the very front of
	      tclUnix.h (before the first #include) to correspond
	      to your system.
	    - Modify Makefile to set CC, CFLAGS, etc. for your system.
	    - If things don't compile or don't link, then you may need
	      to copy some of the .c or .h files from the "compat"
	      directory into the main Tcl directory to compensate for
	      files missing from your system.  Modify the COMPAT_OBJS
	      definition in Makefile to include a .o name for each of
	      the .c files that you copied up from the compat directory.

    (f) Create a directory /usr/local/lib/tcl and copy all the files
        from the "library" subdirectory to /usr/local/lib/tcl.  Or,
	you can use some other directory as library, but you'll need
	to modify the Makefile to reflect this fact (change the
	TCL_LIBRARY definition).

    (g) Type "make tclTest", which will create a simple test program that
	you can use to try out the Tcl facilities.  TclTest is just a
	main-program sandwich around the Tcl library.  It reads standard
	input until it reaches the end of a line where parentheses and
	backslashes are balanced, then sends everything it's read to the
	Tcl interpreter.  When the Tcl interpreter returns, tclTest prints
	the return value or error message.  TclTest defines a few other
	additional commands, most notably:
    
			echo arg arg ...
    
	The "echo" command prints its arguments on standard output,
	separated by spaces.

5. Test suite
-------------

There is a relatively complete test suite for all of the Tcl core in
the subdirectory "tests".  To use it just type "make test" in this
directory.  You should then see a printout of the test files processed.
If any errors occur, you'll see a much more substantial printout for
each error.  Tcl should pass the test suite cleanly on all of the
systems listed in Section 3.  See the README file in the "tests"
directory for more information on the test suite.

6. Compiling on non-UNIX systems
--------------------------------

The Tcl features that depend on system calls peculiar to UNIX (stat,
fork, exec, times, etc.) are now separate from the main body of Tcl,
which only requires a few generic library procedures such as malloc
and strcpy.  Thus it should be relatively easy to compile Tcl for
these machines, although a number of UNIX-specific commands will
be absent (e.g.  exec, time, and glob).  See the comments at the
top of Makefile for information on how to compile without the UNIX
features.

7. Special thanks
-----------------

Mark Diekhans and Karl Lehenbauer of Santa Cruz Operation deserve special
thanks for all their help during the development of Tcl 6.0.  Many of the
new features in Tcl 6.0 were inspired by an enhanced version of Tcl 3.3
called Extended Tcl 4.0, which was created by Mark, Karl, and Peter
da Silva.  Mark and Karl explained these features to me and provided
excellent (and challenging) feedback during the 6.0 development process.
They were also a great help in finding and fixing portability problems.
Without their ideas and assistance Tcl 6.0 would be much less powerful.

8. Support
----------

There is no official support organization for Tcl, and I can't promise
to provide much hand-holding to people learning Tcl.  However, I'm very
interested in receiving bug reports and suggestions for improvements.
Bugs usually get fixed quickly (particularly if they are serious), but
enhancements may take a while and may not happen at all unless there
is widespread support for them.

9. Tcl newsgroup
-----------------

There is a network news group "comp.lang.tcl" intended for the exchange
of information about Tcl, Tk, and related applications.  Feel free to use
this newsgroup both for general information questions and for bug reports.
I read the newsgroup and will attempt to fix bugs and problems reported
to it.
