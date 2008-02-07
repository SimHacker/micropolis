/*
 * tk.h --
 *
 *	Declarations for Tk-related things that are visible
 *	outside of the Tk module itself.
 *
 * Copyright 1989-1992 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /user6/ouster/wish/RCS/tk.h,v 1.90 92/08/20 16:36:30 ouster Exp $ SPRITE (Berkeley)
 */

#ifndef _TK
#define _TK

#ifndef _TCL
#include <tcl.h>
#endif
#ifndef _XLIB_H
#include <X11/Xlib.h>
#endif
#ifdef __STDC__

/* This is so gcc's stddef.h doesn't clash with sunos4.1.1's stdtypes.h */
#ifdef __sys_stdtypes_h
#define _PTRDIFF_T
#define _SIZE_T
#define _WCHAR_T
#endif

#include <stddef.h>
#endif

/*
 * Dummy types that are used by clients:
 */

typedef struct Tk_ErrorHandler_ *Tk_ErrorHandler;
typedef struct Tk_TimerToken_ *Tk_TimerToken;
typedef struct Tk_Window_ *Tk_Window;
typedef struct Tk_3DBorder_ *Tk_3DBorder;
typedef struct Tk_BindingTable_ *Tk_BindingTable;

/*
 * Additional types exported to clients.
 */

typedef char *Tk_Uid;

/*
 * Structure used to specify how to handle argv options.
 */

typedef struct {
    char *key;		/* The key string that flags the option in the
			 * argv array. */
    int type;		/* Indicates option type;  see below. */
    char *src;		/* Value to be used in setting dst;  usage
			 * depends on type. */
    char *dst;		/* Address of value to be modified;  usage
			 * depends on type. */
    char *help;		/* Documentation message describing this option. */
} Tk_ArgvInfo;

/*
 * Legal values for the type field of a Tk_ArgvInfo: see the user
 * documentation for details.
 */

#define TK_ARGV_CONSTANT		15
#define TK_ARGV_INT			16
#define TK_ARGV_STRING			17
#define TK_ARGV_UID			18
#define TK_ARGV_REST			19
#define TK_ARGV_FLOAT			20
#define TK_ARGV_FUNC			21
#define TK_ARGV_GENFUNC			22
#define TK_ARGV_HELP			23
#define TK_ARGV_CONST_OPTION		24
#define TK_ARGV_OPTION_VALUE		25
#define TK_ARGV_OPTION_NAME_VALUE	26
#define TK_ARGV_END			27

/*
 * Flag bits for passing to Tk_ParseArgv:
 */

#define TK_ARGV_NO_DEFAULTS		0x1
#define TK_ARGV_NO_LEFTOVERS		0x2
#define TK_ARGV_NO_ABBREV		0x4
#define TK_ARGV_DONT_SKIP_FIRST_ARG	0x8

/*
 * Structure used to describe application-specific configuration
 * options:  indicates procedures to call to parse an option and
 * to return a text string describing an option.
 */

typedef int (Tk_OptionParseProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, Tk_Window tkwin, char *value, char *widgRec,
	int offset));
typedef char *(Tk_OptionPrintProc) _ANSI_ARGS_((ClientData clientData,
	Tk_Window tkwin, char *widgRec, int offset,
	Tcl_FreeProc **freeProcPtr));

typedef struct Tk_CustomOption {
    Tk_OptionParseProc *parseProc;	/* Procedure to call to parse an
					 * option and store it in converted
					 * form. */
    Tk_OptionPrintProc *printProc;	/* Procedure to return a printable
					 * string describing an existing
					 * option. */
    ClientData clientData;		/* Arbitrary one-word value used by
					 * option parser:  passed to
					 * parseProc and printProc. */
} Tk_CustomOption;

/*
 * Structure used to specify information for Tk_ConfigureWidget.  Each
 * structure gives complete information for one option, including
 * how the option is specified on the command line, where it appears
 * in the option database, etc.
 */

typedef struct Tk_ConfigSpec {
    int type;			/* Type of option, such as TK_CONFIG_COLOR;
				 * see definitions below.  Last option in
				 * table must have type TK_CONFIG_END. */
    char *argvName;		/* Switch used to specify option in argv.
				 * NULL means this spec is part of a group. */
    char *dbName;		/* Name for option in option database. */
    char *dbClass;		/* Class for option in database. */
    char *defValue;		/* Default value for option if not
				 * specified in command line or database. */
    int offset;			/* Where in widget record to store value;
				 * use Tk_Offset macro to generate values
				 * for this. */
    int specFlags;		/* Any combination of the values defined
				 * below;  other bits are used internally
				 * by tkConfig.c. */
    Tk_CustomOption *customPtr;	/* If type is TK_CONFIG_CUSTOM then this is
				 * a pointer to info about how to parse and
				 * print the option.  Otherwise it is
				 * irrelevant. */
} Tk_ConfigSpec;

/*
 * Type values for Tk_ConfigSpec structures.  See the user
 * documentation for details.
 */

#define TK_CONFIG_BOOLEAN	1
#define TK_CONFIG_INT		2
#define TK_CONFIG_DOUBLE	3
#define TK_CONFIG_STRING	4
#define TK_CONFIG_UID		5
#define TK_CONFIG_COLOR		6
#define TK_CONFIG_FONT		7
#define TK_CONFIG_BITMAP	8
#define TK_CONFIG_BORDER	9
#define TK_CONFIG_RELIEF	10
#define TK_CONFIG_CURSOR	11
#define TK_CONFIG_ACTIVE_CURSOR	12
#define TK_CONFIG_JUSTIFY	13
#define TK_CONFIG_ANCHOR	14
#define TK_CONFIG_SYNONYM	15
#define TK_CONFIG_CAP_STYLE	16
#define TK_CONFIG_JOIN_STYLE	17
#define TK_CONFIG_PIXELS	18
#define TK_CONFIG_MM		19
#define TK_CONFIG_WINDOW	20
#define TK_CONFIG_CUSTOM	21
#define TK_CONFIG_END		22
#define TK_CONFIG_PIXMAP	30

/*
 * Macro to use to fill in "offset" fields of Tk_ConfigInfos.
 * Computes number of bytes from beginning of structure to a
 * given field.
 */

#ifdef offsetof
#define Tk_Offset(type, field) ((int) offsetof(type, field))
#else
#define Tk_Offset(type, field) ((int) ((char *) &((type *) 0)->field))
#endif

/*
 * Possible values for flags argument to Tk_ConfigureWidget:
 */

#define TK_CONFIG_ARGV_ONLY	1

/*
 * Possible flag values for Tk_ConfigInfo structures.  Any bits at
 * or above TK_CONFIG_USER_BIT may be used by clients for selecting
 * certain entries.  Before changing any values here, coordinate with
 * tkConfig.c (internal-use-only flags are defined there).
 */

#define TK_CONFIG_COLOR_ONLY		1
#define TK_CONFIG_MONO_ONLY		2
#define TK_CONFIG_NULL_OK		4
#define TK_CONFIG_DONT_SET_DEFAULT	8
#define TK_CONFIG_OPTION_SPECIFIED	0x10
#define TK_CONFIG_USER_BIT		0x100

/*
 * Bits to pass to Tk_CreateFileHandler to indicate what sorts
 * of events are of interest:
 */

#define TK_READABLE	1
#define TK_WRITABLE	2
#define TK_EXCEPTION	4

/*
 * Flag values to pass to Tk_DoOneEvent to disable searches
 * for some kinds of events:
 */

#define TK_DONT_WAIT		1
#define TK_X_EVENTS		2
#define TK_FILE_EVENTS		4
#define TK_TIMER_EVENTS		8
#define TK_IDLE_EVENTS		0x10
#define TK_ALL_EVENTS		0x1e

/*
 * Priority levels to pass to Tk_AddOption:
 */

#define TK_WIDGET_DEFAULT_PRIO	20
#define TK_STARTUP_FILE_PRIO	40
#define TK_USER_DEFAULT_PRIO	60
#define TK_INTERACTIVE_PRIO	80
#define TK_MAX_PRIO		100

/*
 * Relief values returned by Tk_GetRelief:
 */

#define TK_RELIEF_RAISED	1
#define TK_RELIEF_FLAT		2
#define TK_RELIEF_SUNKEN	4

/*
 * Special EnterNotify/LeaveNotify "mode" for use in events
 * generated by tkShare.c.  Pick a high enough value that it's
 * unlikely to conflict with existing values (like NotifyNormal)
 * or any new values defined in the future.
 */

#define TK_NOTIFY_SHARE		20

/*
 * Enumerated type for describing a point by which to anchor something:
 */

typedef enum {
    TK_ANCHOR_N, TK_ANCHOR_NE, TK_ANCHOR_E, TK_ANCHOR_SE,
    TK_ANCHOR_S, TK_ANCHOR_SW, TK_ANCHOR_W, TK_ANCHOR_NW,
    TK_ANCHOR_CENTER
} Tk_Anchor;

/*
 * Enumerated type for describing a style of justification:
 */

typedef enum {
    TK_JUSTIFY_LEFT, TK_JUSTIFY_RIGHT,
    TK_JUSTIFY_CENTER, TK_JUSTIFY_FILL
} Tk_Justify;

/*
 *--------------------------------------------------------------
 *
 * Macros for querying Tk_Window structures.  See the
 * manual entries for documentation.
 *
 *--------------------------------------------------------------
 */

#define Tk_Display(tkwin)		(((Tk_FakeWin *) (tkwin))->display)
#define Tk_ScreenNumber(tkwin)		(((Tk_FakeWin *) (tkwin))->screenNum)
#define Tk_Screen(tkwin)		(ScreenOfDisplay(Tk_Display(tkwin), \
	Tk_ScreenNumber(tkwin)))
#define Tk_WindowId(tkwin)		(((Tk_FakeWin *) (tkwin))->window)
#define Tk_PathName(tkwin) 		(((Tk_FakeWin *) (tkwin))->pathName)
#define Tk_Name(tkwin)			(((Tk_FakeWin *) (tkwin))->nameUid)
#define Tk_Class(tkwin) 		(((Tk_FakeWin *) (tkwin))->classUid)
#define Tk_X(tkwin)			(((Tk_FakeWin *) (tkwin))->changes.x)
#define Tk_Y(tkwin)			(((Tk_FakeWin *) (tkwin))->changes.y)
#define Tk_Width(tkwin)			(((Tk_FakeWin *) (tkwin))->changes.width)
#define Tk_Height(tkwin) \
    (((Tk_FakeWin *) (tkwin))->changes.height)
#define Tk_Changes(tkwin)		(&((Tk_FakeWin *) (tkwin))->changes)
#define Tk_Attributes(tkwin)		(&((Tk_FakeWin *) (tkwin))->atts)
#define Tk_IsMapped(tkwin) \
    (((Tk_FakeWin *) (tkwin))->flags & TK_MAPPED)
#define Tk_ReqWidth(tkwin)		(((Tk_FakeWin *) (tkwin))->reqWidth)
#define Tk_ReqHeight(tkwin)		(((Tk_FakeWin *) (tkwin))->reqHeight)
#define Tk_InternalBorderWidth(tkwin) \
    (((Tk_FakeWin *) (tkwin))->internalBorderWidth)
#define Tk_Parent(tkwin)		(((Tk_FakeWin *) (tkwin))->parentPtr)

/*
 * The structure below is needed by the macros above so that they can
 * access the fields of a Tk_Window.  The fields not needed by the macros
 * are declared as "dummyX".  The structure has its own type in order to
 * prevent applications from accessing Tk_Window fields except using
 * official macros.  WARNING!! The structure definition must be kept
 * consistent with the TkWindow structure in tkInt.h.  If you change one,
 * then change the other.  See the declaration in tkInt.h for
 * documentation on what the fields are used for internally.
 */

typedef struct Tk_FakeWin {
    Display *display;
    char *dummy1;
    int screenNum;
    Window window;
    char *dummy2;
    Tk_Window parentPtr;
    char *dummy4;
    char *dummy5;
    char *pathName;
    Tk_Uid nameUid;
    Tk_Uid classUid;
    XWindowChanges changes;
    unsigned int dummy6;
    XSetWindowAttributes atts;
    unsigned long dummy7;
    unsigned int flags;
    char *dummy8;
    char *dummy9;
    ClientData dummy10;
    int dummy12;
    char *dummy13;
    char *dummy14;
    ClientData dummy15;
    char *dummy16;
    ClientData dummy17;
    int reqWidth, reqHeight;
    int internalBorderWidth;
    char *dummyX;
} Tk_FakeWin;

/*
 * Flag values for TkWindow (and Tk_FakeWin) structures are:
 *
 * TK_MAPPED:			1 means window is currently mapped,
 *				0 means unmapped.
 * TK_RECURSIVE_DESTROY:	1 means a recursive destroy is in
 *				progress, so some cleanup operations
 *				can be omitted.
 * TK_TOP_LEVEL:		1 means this is a top-level window (it
 *				was or will be created as a child of
 *				a root window).
 * TK_ALREADY_DEAD:		1 means the window is in the process of
 *				being destroyed already.
 * TK_NEED_CONFIG_NOTIFY:	1 means that the window has been reconfigured
 *				before it was made to exist.  At the time of
 *				making it exist a ConfigureNotify event needs
 *				to be generated.
 * TK_GRAB_FLAG:		Used to manage grabs.  See tkGrab.c for
 *				details.
 */

#define TK_MAPPED		1
#define TK_RECURSIVE_DESTROY	2
#define TK_TOP_LEVEL		4
#define TK_ALREADY_DEAD		8
#define TK_NEED_CONFIG_NOTIFY	0x10
#define TK_GRAB_FLAG		0x20

/*
 *--------------------------------------------------------------
 *
 * Additional procedure types defined by Tk.
 *
 *--------------------------------------------------------------
 */

typedef int (Tk_ErrorProc) _ANSI_ARGS_((ClientData clientData,
	XErrorEvent *errEventPtr));
typedef void (Tk_EventProc) _ANSI_ARGS_((ClientData clientData,
	XEvent *eventPtr));
typedef void (Tk_FileProc) _ANSI_ARGS_((ClientData clientData, int mask));
typedef void (Tk_FocusProc) _ANSI_ARGS_((ClientData clientData, int gotFocus));
typedef void (Tk_FreeProc) _ANSI_ARGS_((ClientData clientData));
typedef int (Tk_GenericProc) _ANSI_ARGS_((ClientData clientData,
	XEvent *eventPtr));
typedef void (Tk_GeometryProc) _ANSI_ARGS_((ClientData clientData,
	Tk_Window tkwin));
typedef int (Tk_GetSelProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, char *portion));
typedef void (Tk_IdleProc) _ANSI_ARGS_((ClientData clientData));
typedef void (Tk_LostSelProc) _ANSI_ARGS_((ClientData clientData));
typedef Bool (Tk_RestrictProc) _ANSI_ARGS_((Display *display, XEvent *eventPtr,
	char *arg));
typedef int (Tk_SelectionProc) _ANSI_ARGS_((ClientData clientData,
	int offset, char *buffer, int maxBytes));
typedef void (Tk_TimerProc) _ANSI_ARGS_((ClientData clientData));

/*
 *--------------------------------------------------------------
 *
 * Exported procedures and variables.
 *
 *--------------------------------------------------------------
 */

extern XColor *		Tk_3DBorderColor _ANSI_ARGS_((Tk_3DBorder border));
extern void		Tk_AddOption _ANSI_ARGS_((Tk_Window tkwin, char *name,
			    char *value, int priority));
extern void		Tk_BindEvent _ANSI_ARGS_((Tk_BindingTable bindingTable,
			    XEvent *eventPtr, Tk_Window tkwin, int numObjects,
			    ClientData *objectPtr));
extern void		Tk_CancelIdleCall _ANSI_ARGS_((Tk_IdleProc *idleProc,
				ClientData clientData));
extern void		Tk_ChangeWindowAttributes _ANSI_ARGS_((Tk_Window tkwin,
			    unsigned long valueMask,
			    XSetWindowAttributes *attsPtr));
extern int		Tk_ConfigureInfo _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, Tk_ConfigSpec *specs,
			    char *widgRec, char *argvName, int flags));
extern int		Tk_ConfigureWidget _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, Tk_ConfigSpec *specs,
			    int argc, char **argv, char *widgRec,
			    int flags));
extern Tk_Window	Tk_CoordsToWindow _ANSI_ARGS_((int rootX, int rootY,
			    Tk_Window tkwin));
extern unsigned long	Tk_CreateBinding _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_BindingTable bindingTable, ClientData object,
			    char *eventString, char *command, int append));
extern Tk_BindingTable	Tk_CreateBindingTable _ANSI_ARGS_((Tcl_Interp *interp));
extern Tk_ErrorHandler	Tk_CreateErrorHandler _ANSI_ARGS_((Display *display,
			    int error, int request, int minorCode,
			    Tk_ErrorProc *errorProc, ClientData clientData));
extern void		Tk_CreateEventHandler _ANSI_ARGS_((Tk_Window token,
			    unsigned long mask, Tk_EventProc *proc,
			    ClientData clientData));
extern void		Tk_CreateFileHandler _ANSI_ARGS_((int fd, int mask,
			    Tk_FileProc *proc, ClientData clientData));
extern void		Tk_CreateFocusHandler _ANSI_ARGS_((Tk_Window tkwin,
			    Tk_FocusProc *proc, ClientData clientData));
extern void		Tk_CreateGenericHandler _ANSI_ARGS_((
			    Tk_GenericProc *proc, ClientData clientData));
extern Tk_Window	Tk_CreateMainWindow _ANSI_ARGS_((Tcl_Interp *interp,
			    char *screenName, char *baseName));
extern void		Tk_CreateSelHandler _ANSI_ARGS_((Tk_Window tkwin,
			    Atom target, Tk_SelectionProc *proc,
			    ClientData clientData, Atom format));
extern Tk_TimerToken	Tk_CreateTimerHandler _ANSI_ARGS_((int milliseconds,
			    Tk_TimerProc *proc, ClientData clientData));
// Added by Don to support finer timer resolution.
extern Tk_TimerToken	Tk_CreateMicroTimerHandler _ANSI_ARGS_((int seconds,
			    int microseconds, Tk_TimerProc *proc, ClientData clientData));
extern Tk_Window	Tk_CreateWindow _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window parent, char *name, char *screenName));
extern Tk_Window	Tk_CreateWindowFromPath _ANSI_ARGS_((
			    Tcl_Interp *interp, Tk_Window tkwin,
			    char *pathName, char *screenName));
extern int		Tk_DefineBitmap _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Uid name, char *source, unsigned int width,
			    unsigned int height));
extern int		Tk_DefinePixmap _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Uid name, Tk_Window, Pixmap pixmap,
			    char *source, unsigned int width,
			    unsigned int height));
extern void		Tk_DefineCursor _ANSI_ARGS_((Tk_Window window,
			    Cursor cursor));
extern void		Tk_DeleteAllBindings _ANSI_ARGS_((
			    Tk_BindingTable bindingTable, ClientData object));
extern int		Tk_DeleteBinding _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_BindingTable bindingTable, ClientData object,
			    char *eventString));
extern void		Tk_DeleteBindingTable _ANSI_ARGS_((
			    Tk_BindingTable bindingTable));
extern void		Tk_DeleteErrorHandler _ANSI_ARGS_((
			    Tk_ErrorHandler handler));
extern void		Tk_DeleteEventHandler _ANSI_ARGS_((Tk_Window token,
			    unsigned long mask, Tk_EventProc *proc,
			    ClientData clientData));
extern void		Tk_DeleteFileHandler _ANSI_ARGS_((int fd));
extern void		Tk_DeleteGenericHandler _ANSI_ARGS_((
			    Tk_GenericProc *proc, ClientData clientData));
extern void		Tk_DeleteTimerHandler _ANSI_ARGS_((
			    Tk_TimerToken token));
extern void		Tk_DestroyWindow _ANSI_ARGS_((Tk_Window tkwin));
extern char *		Tk_DisplayName _ANSI_ARGS_((Tk_Window tkwin));
extern int		Tk_DoOneEvent _ANSI_ARGS_((int flags));
extern void		Tk_DoWhenIdle _ANSI_ARGS_((Tk_IdleProc *proc,
			    ClientData clientData));
extern void		Tk_Draw3DPolygon _ANSI_ARGS_((Display *display,
			    Drawable drawable, Tk_3DBorder border,
			    XPoint *pointPtr, int numPoints, int borderWidth,
			    int leftRelief));
extern void		Tk_Draw3DRectangle _ANSI_ARGS_((Display *display,
			    Drawable drawable, Tk_3DBorder border, int x,
			    int y, int width, int height, int borderWidth,
			    int relief));
extern void		Tk_EventuallyFree _ANSI_ARGS_((ClientData clientData,
			    Tk_FreeProc *freeProc));
extern void		Tk_Fill3DPolygon _ANSI_ARGS_((Display *display,
			    Drawable drawable, Tk_3DBorder border,
			    XPoint *pointPtr, int numPoints, int borderWidth,
			    int leftRelief));
extern void		Tk_Fill3DRectangle _ANSI_ARGS_((Display *display,
			    Drawable drawable, Tk_3DBorder border, int x,
			    int y, int width, int height, int borderWidth,
			    int relief));
extern void		Tk_Free3DBorder _ANSI_ARGS_((Tk_3DBorder border));
extern void		Tk_FreeBitmap _ANSI_ARGS_((Pixmap bitmap));
extern void		Tk_FreePixmap _ANSI_ARGS_((Pixmap bitmap));
extern void		Tk_FreeColor _ANSI_ARGS_((XColor *colorPtr));
extern int		Tk_IndexOfScreen _ANSI_ARGS_((Screen *screen));
extern int		Tk_DefaultDepth _ANSI_ARGS_((Screen *screen));
extern Visual		*Tk_DefaultVisual _ANSI_ARGS_((Screen *screen));
extern Colormap		Tk_DefaultColormap _ANSI_ARGS_((Screen *screen));
extern Window		Tk_DefaultRootWindow _ANSI_ARGS_((Display *dpy));
extern GC		Tk_DefaultGC _ANSI_ARGS_((Screen *screen));
extern Pixmap		Tk_DefaultPixmap _ANSI_ARGS_((Screen *screen));
extern void		Tk_FreeCursor _ANSI_ARGS_((Cursor cursor));
extern void		Tk_FreeFontStruct _ANSI_ARGS_((
			    XFontStruct *fontStructPtr));
extern void		Tk_FreeGC _ANSI_ARGS_((GC gc));
extern void		Tk_GeometryRequest _ANSI_ARGS_((Tk_Window tkwin,
			    int reqWidth,  int reqHeight));
extern Tk_3DBorder	Tk_Get3DBorder _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, Colormap colormap,
			    Tk_Uid colorName));
extern void		Tk_GetAllBindings _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_BindingTable bindingTable, ClientData object));
extern int		Tk_GetAnchor _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, Tk_Anchor *anchorPtr));
extern char *		Tk_GetAtomName _ANSI_ARGS_((Tk_Window tkwin,
			    Atom atom));
extern char *		Tk_GetBinding _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_BindingTable bindingTable, ClientData object,
			    char *eventString));
extern Pixmap		Tk_GetBitmap _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, Tk_Uid string));
extern Pixmap		Tk_GetPixmap _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, Tk_Uid string));
extern Pixmap		Tk_GetBitmapFromData _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, char *source,
			    unsigned int width, unsigned int height));
extern Pixmap		Tk_GetPixmapFromData _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, char *source,
			    unsigned int width, unsigned int height));
extern int		Tk_GetCapStyle _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, int *capPtr));
extern XColor *		Tk_GetColor _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, Colormap colormap, Tk_Uid name));
extern XColor *		Tk_GetColorByValue _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, Colormap colormap,
			    XColor *colorPtr));
extern Cursor		Tk_GetCursor _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, Tk_Uid string));
extern Cursor		Tk_GetCursorFromData _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, char *source, char *mask,
			    unsigned int width, unsigned int height,
			    int xHot, int yHot, Tk_Uid fg, Tk_Uid bg));
extern XFontStruct *	Tk_GetFontStruct _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, Tk_Uid name));
extern GC		Tk_GetGC _ANSI_ARGS_((Tk_Window tkwin,
			    unsigned long valueMask, XGCValues *valuePtr));
extern int		Tk_GetJoinStyle _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, int *joinPtr));
extern int		Tk_GetJustify _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, Tk_Justify *justifyPtr));
extern Tk_Uid		Tk_GetOption _ANSI_ARGS_((Tk_Window tkwin, char *name,
			    char *className));
extern int		Tk_GetPixels _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, char *string, int *intPtr));
extern int		Tk_GetRelief _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name, int *reliefPtr));
extern void		Tk_GetRootCoords _ANSI_ARGS_ ((Tk_Window tkwin,
			    int *xPtr, int *yPtr));
extern int		Tk_GetScreenMM _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, char *string, double *doublePtr));
extern int		Tk_GetSelection _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, Atom target, Tk_GetSelProc *proc,
			    ClientData clientData));
extern Tk_Uid		Tk_GetUid _ANSI_ARGS_((char *string));
extern int		Tk_Grab _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, int grabGlobal));
extern void		Tk_HandleEvent _ANSI_ARGS_((XEvent *eventPtr));
extern Atom		Tk_InternAtom _ANSI_ARGS_((Tk_Window tkwin,
			    char *name));
extern void		Tk_MainLoop _ANSI_ARGS_((void));
extern void		Tk_MakeWindowExist _ANSI_ARGS_((Tk_Window tkwin));
extern void		Tk_ManageGeometry _ANSI_ARGS_((Tk_Window tkwin,
			    Tk_GeometryProc *proc, ClientData clientData));
extern void		Tk_MapWindow _ANSI_ARGS_((Tk_Window tkwin));
extern void		Tk_MoveResizeWindow _ANSI_ARGS_((Tk_Window tkwin,
			    int x, int y, unsigned int width,
			    unsigned int height));
extern void		Tk_MoveWindow _ANSI_ARGS_((Tk_Window tkwin, int x,
			    int y));
extern char *		Tk_NameOf3DBorder _ANSI_ARGS_((Tk_3DBorder border));
extern char *		Tk_NameOfAnchor _ANSI_ARGS_((Tk_Anchor anchor));
extern char *		Tk_NameOfBitmap _ANSI_ARGS_((Pixmap bitmap));
extern char *		Tk_NameOfPixmap _ANSI_ARGS_((Pixmap bitmap));
extern char *		Tk_NameOfCapStyle _ANSI_ARGS_((int cap));
extern char *		Tk_NameOfColor _ANSI_ARGS_((XColor *colorPtr));
extern char *		Tk_NameOfCursor _ANSI_ARGS_((Cursor cursor));
extern char *		Tk_NameOfFontStruct _ANSI_ARGS_((
			    XFontStruct *fontStructPtr));
extern char *		Tk_NameOfJoinStyle _ANSI_ARGS_((int join));
extern char *		Tk_NameOfJustify _ANSI_ARGS_((Tk_Justify justify));
extern char *		Tk_NameOfRelief _ANSI_ARGS_((int relief));
extern Tk_Window	Tk_NameToWindow _ANSI_ARGS_((Tcl_Interp *interp,
			    char *pathName, Tk_Window tkwin));
extern void		Tk_OwnSelection _ANSI_ARGS_((Tk_Window tkwin,
			    Tk_LostSelProc *proc, ClientData clientData));
extern int		Tk_ParseArgv _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Window tkwin, int *argcPtr, char **argv,
			    Tk_ArgvInfo *argTable, int flags));
extern void		Tk_Preserve _ANSI_ARGS_((ClientData clientData));
extern int		Tk_RegisterInterp _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name, Tk_Window tkwin));
extern void		Tk_Release _ANSI_ARGS_((ClientData clientData));
extern void		Tk_ResizeWindow _ANSI_ARGS_((Tk_Window tkwin,
			    unsigned int width, unsigned int height));
extern Tk_RestrictProc *Tk_RestrictEvents _ANSI_ARGS_((Tk_RestrictProc *proc,
			    char *arg, char **prevArgPtr));
extern void		Tk_SetBackgroundFromBorder _ANSI_ARGS_((
			    Tk_Window tkwin, Tk_3DBorder border));
extern void		Tk_SetClass _ANSI_ARGS_((Tk_Window tkwin,
			    char *className));
extern void		Tk_SetGrid _ANSI_ARGS_((Tk_Window tkwin,
			    int reqWidth, int reqHeight, int gridWidth,
			    int gridHeight));
extern void		Tk_SetInternalBorder _ANSI_ARGS_((Tk_Window tkwin,
			    int width));
extern void		Tk_SetWindowBackground _ANSI_ARGS_((Tk_Window tkwin,
			    unsigned long pixel));
extern void		Tk_SetWindowBackgroundPixmap _ANSI_ARGS_((
			    Tk_Window tkwin, Pixmap pixmap));
extern void		Tk_SetWindowBorder _ANSI_ARGS_((Tk_Window tkwin,
			    unsigned long pixel));
extern void		Tk_SetWindowBorderWidth _ANSI_ARGS_((Tk_Window tkwin,
			    int width));
extern void		Tk_SetWindowBorderPixmap _ANSI_ARGS_((Tk_Window tkwin,
			    Pixmap pixmap));
extern void		Tk_ShareEvents _ANSI_ARGS_((Tk_Window tkwin,
			    Tk_Uid groupId));
extern void		Tk_SizeOfBitmap _ANSI_ARGS_((Pixmap bitmap,
			    unsigned int *widthPtr, unsigned int *heightPtr));
extern void		Tk_SizeOfPixmap _ANSI_ARGS_((Pixmap bitmap,
			    unsigned int *widthPtr, unsigned int *heightPtr));
extern void		Tk_Sleep _ANSI_ARGS_((int ms));
extern void		Tk_UndefineCursor _ANSI_ARGS_((Tk_Window window));
extern int		Tk_UndefinePixmap _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Uid name, Tk_Window tkwin));
extern void		Tk_Ungrab _ANSI_ARGS_((Tk_Window tkwin));
extern void		Tk_UnmapWindow _ANSI_ARGS_((Tk_Window tkwin));
extern void		Tk_UnshareEvents _ANSI_ARGS_((Tk_Window tkwin,
			    Tk_Uid groupId));


extern int		tk_NumMainWindows;

/* 
 * Added by dhopkins for OLPC Micropolis gtk.Socket integration. 
 */
extern Window           tk_RootWindow;

/*
 * Tcl commands exported by Tk:
 */

extern int		Tk_AfterCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_ApplicationCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_BindCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_ButtonCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_CanvasCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_DestroyCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_EntryCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_FrameCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_FocusCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_GrabCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_ListboxCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_MenuCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_MenubuttonCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_MessageCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_OptionCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_PackCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_PlaceCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_ScaleCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_ScrollbarCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_SelectionCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_SendCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_TextCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_TkwaitCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_UpdateCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_WinfoCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tk_WmCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tcp_AcceptCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tcp_ConnectCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tcp_ShutdownCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
extern int		Tcp_FileHandlerCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));

#endif /* _TK */
