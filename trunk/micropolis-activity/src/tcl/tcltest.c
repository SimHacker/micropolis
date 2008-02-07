/* 
 * tclTest.c --
 *
 *	Test driver for TCL.
 *
 * Copyright 1987-1991 Regents of the University of California
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appears in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/tcl/tclTest/RCS/tclTest.c,v 1.21 92/01/19 14:15:27 ouster Exp $ SPRITE (Berkeley)";
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "tcl.h"

extern int exit();
extern int Tcl_DumpActiveMemory();

Tcl_Interp *interp;
Tcl_CmdBuf buffer;
char dumpFile[100];
int quitFlag = 0;

char *initCmd =
    "if [file exists [info library]/init.tcl] {source [info library]/init.tcl}";

	/* ARGSUSED */
int
cmdCheckmem(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char *argv[];
{
    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileName\"", (char *) NULL);
	return TCL_ERROR;
    }
    strcpy(dumpFile, argv[1]);
    quitFlag = 1;
    return TCL_OK;
}

	/* ARGSUSED */
int
cmdEcho(clientData, interp, argc, argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char *argv[];
{
    int i;

    for (i = 1; ; i++) {
	if (argv[i] == NULL) {
	    if (i != argc) {
		echoError:
		sprintf(interp->result,
		    "argument list wasn't properly NULL-terminated in \"%s\" command",
		    argv[0]);
	    }
	    break;
	}
	if (i >= argc) {
	    goto echoError;
	}
	fputs(argv[i], stdout);
	if (i < (argc-1)) {
	    printf(" ");
	}
    }
    printf("\n");
    return TCL_OK;
}

int
main()
{
    char line[1000], *cmd;
    int result, gotPartial;

    interp = Tcl_CreateInterp();
#ifdef TCL_MEM_DEBUG
    Tcl_InitMemory(interp);
#endif
    Tcl_CreateCommand(interp, "echo", cmdEcho, (ClientData) "echo",
	    (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "checkmem", cmdCheckmem, (ClientData) 0,
	    (Tcl_CmdDeleteProc *) NULL);
    buffer = Tcl_CreateCmdBuf();
#ifndef TCL_GENERIC_ONLY
    result = Tcl_Eval(interp, initCmd, 0, (char **) NULL);
    if (result != TCL_OK) {
	printf("%s\n", interp->result);
	exit(1);
    }
#endif

    gotPartial = 0;
    while (1) {
	clearerr(stdin);
	if (!gotPartial) {
	    fputs("% ", stdout);
	    fflush(stdout);
	}
	if (fgets(line, 1000, stdin) == NULL) {
	    if (!gotPartial) {
		exit(0);
	    }
	    line[0] = 0;
	}
	cmd = Tcl_AssembleCmd(buffer, line);
	if (cmd == NULL) {
	    gotPartial = 1;
	    continue;
	}

	gotPartial = 0;
	result = Tcl_RecordAndEval(interp, cmd, 0);
	if (result == TCL_OK) {
	    if (*interp->result != 0) {
		printf("%s\n", interp->result);
	    }
	    if (quitFlag) {
		Tcl_DeleteInterp(interp);
		Tcl_DeleteCmdBuf(buffer);
#ifdef TCL_MEM_DEBUG
		Tcl_DumpActiveMemory(dumpFile);
#endif
		exit(0);
	    }
	} else {
	    if (result == TCL_ERROR) {
		printf("Error");
	    } else {
		printf("Error %d", result);
	    }
	    if (*interp->result != 0) {
		printf(": %s\n", interp->result);
	    } else {
		printf("\n");
	    }
	}
    }
}
