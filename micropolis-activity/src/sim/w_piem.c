/* w_piem.c:  Pie Menus
 */

/*
 *
 * Pie Menus for Tk.
 * Copyright (C) 1992 by Don Hopkins.
 *
 * This program is provided for unrestricted use, provided that this 
 * copyright message is preserved. There is no warranty, and no author 
 * or distributer accepts responsibility for any damage caused by this 
 * program. 
 * 
 * This code and the ideas behind it were developed over time by Don Hopkins 
 * with the support of the University of Maryland, UniPress Software, Sun
 * Microsystems, DUX Software, the Turing Institute, and Carnegie Mellon
 * University.  Pie menus are NOT patented or restricted, and the interface 
 * and algorithms may be freely copied and improved upon. 
 *
 */


#if 0
/* workaround to make gcc work on suns */
#ifndef SOLARIS2
#define _PTRDIFF_T
#define _SIZE_T
#ifndef sgi
typedef unsigned int size_t;
#endif
#endif
#endif

#include "tkconfig.h"
#include "default.h"
#include "tkint.h"
#include <X11/extensions/shape.h>

#define PI 3.1415926535897932
#define TWO_PI 6.2831853071795865
#define DEG_TO_RAD(d) (((d) * TWO_PI) / 360.0)
#define RAD_TO_DEG(d) (((d) * 360.0) / TWO_PI)
#define PIE_SPOKE_INSET 6
#define PIE_BG_COLOR "#bfbfbf"
#define PIE_BG_MONO WHITE
#define PIE_ACTIVE_FG_COLOR BLACK
#define PIE_ACTIVE_FG_MONO BLACK
#define PIE_ACTIVE_BG_COLOR "#bfbfbf"
#define PIE_ACTIVE_BG_MONO WHITE
#define PIE_FG BLACK
#define PIE_FONT "-Adobe-Helvetica-Bold-R-Normal-*-120-*"
#define PIE_ACTIVE_BORDER_WIDTH "2"
#define PIE_INACTIVE_RADIUS "8"
#define PIE_INACTIVE_RADIUS_NUM 8
#define PIE_MIN_RADIUS "16"
#define PIE_MIN_RADIUS_NUM 16
#define PIE_EXTRA_RADIUS "2"
#define PIE_EXTRA_RADIUS_NUM 2
#define PIE_BORDER_WIDTH "2"
#define PIE_POPUP_DELAY "250"
#define PIE_POPUP_DELAY_NUM 250
#define PIE_ENTRY_ACTIVE_BG ((char *) NULL)
#define PIE_ENTRY_BG ((char *) NULL)
#define PIE_ENTRY_FONT ((char *) NULL)

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#define ABS(x)	 (((x)<0)?(-(x)):(x))

static int HaveShape = -1;

/*
 * One of the following data structures is kept for each entry of each
 * pie menu managed by this file:
 */

typedef struct PieMenuEntry {
    int type;
    struct PieMenu *piemenuPtr;
    char *label;
    int labelLength;
    Pixmap bitmap;

    /*
     * Information related to displaying entry:
     */

    int width, height;
    int x, y;
    int x_offset, y_offset;
    int label_x, label_y;

    Tk_3DBorder border;
    Tk_3DBorder activeBorder;
    XFontStruct *fontPtr;
    GC textGC;
    GC activeGC;

    /*
     * Information used for pie menu layout & tracking:
     */

    int slice;			/* Desired relative slice size */
    float angle;		/* Angle through center of slice */
    float dx, dy;		/* Cosine and sine of angle */
    float subtend;		/* Angle subtended by slice */
    int quadrant;		/* Quadrant of leading edge */
    float slope;		/* Slope of leading edge */

    /*
     * Information used to implement this entry's action:
     */

    char *command;
    char *preview;
    char *name;

    /*
     * Miscellaneous information:
     */

    int flags;			/* Various flags. See below for definitions. */
} PieMenuEntry;

/*
 * Flag values defined for menu entries:
 *
 * ENTRY_NEEDS_REDISPLAY:	Non-zero means the entry should be redisplayed.
 */

#define ENTRY_NEEDS_REDISPLAY	1

/*
 * Types defined for PieMenuEntries:
 */

#define COMMAND_ENTRY		0
#define PIEMENU_ENTRY		1

/*
 * Mask bits for above types:
 */

#define COMMAND_MASK		TK_CONFIG_USER_BIT
#define PIEMENU_MASK		(TK_CONFIG_USER_BIT << 1)
#define ALL_MASK		(COMMAND_MASK | PIEMENU_MASK)

/*
 * Configuration specs for individual menu entries:
 */

static Tk_ConfigSpec entryConfigSpecs[] = {
    {TK_CONFIG_BORDER, "-activebackground", (char *) NULL, (char *) NULL,
	PIE_ENTRY_ACTIVE_BG, Tk_Offset(PieMenuEntry, activeBorder),
	ALL_MASK|TK_CONFIG_NULL_OK},
    {TK_CONFIG_BORDER, "-background", (char *) NULL, (char *) NULL,
	PIE_ENTRY_BG, Tk_Offset(PieMenuEntry, border),
	ALL_MASK|TK_CONFIG_NULL_OK},
    {TK_CONFIG_PIXMAP, "-bitmap", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(PieMenuEntry, bitmap),
	ALL_MASK|TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-command", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(PieMenuEntry, command),
	COMMAND_MASK},
    {TK_CONFIG_STRING, "-preview", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(PieMenuEntry, preview),
	ALL_MASK},
    {TK_CONFIG_FONT, "-font", (char *) NULL, (char *) NULL,
	PIE_ENTRY_FONT, Tk_Offset(PieMenuEntry, fontPtr),
	ALL_MASK|TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-label", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(PieMenuEntry, label),
	ALL_MASK},
    {TK_CONFIG_STRING, "-piemenu", (char *) NULL, (char *) NULL,
        (char *) NULL, Tk_Offset(PieMenuEntry, name),
        ALL_MASK},
    {TK_CONFIG_INT, "-xoffset", "xOffset", "XOffset",
	"0", Tk_Offset(PieMenuEntry, x_offset),
        ALL_MASK},
    {TK_CONFIG_INT, "-yoffset", "yOffset", "YOffset",
	"0", Tk_Offset(PieMenuEntry, y_offset),
        ALL_MASK},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * A data structure of the following type is kept for each
 * pie menu managed by this file:
 */

typedef struct PieMenu {
    Tk_Window tkwin;
    Tcl_Interp *interp;
    char *title;
    int titleLength;
    char *preview;
    PieMenuEntry **entries;
    int numEntries;
    int active;
    Tk_Uid group;
    int root_x, root_y;
    int dx, dy;

    /*
     * Information used when displaying widget:
     */

    Tk_3DBorder border;
    int borderWidth;
    Tk_3DBorder activeBorder;
    int activeBorderWidth;
    XFontStruct *fontPtr;
    XFontStruct *titlefontPtr;
    XColor *fg;
    GC textGC;
    XColor *activeFg;
    GC activeGC;

    /*
     * Information used to layout pie menu:
     */

    int width, height;			/* size of the pie menu */
    int title_x, title_y;		/* position of menu title */
    int title_width, title_height;	/* size of menu title */
    int initial_angle;			/* pie menu initial angle in radians */
    int inactive_radius;		/* inactive inner radius */
    int min_radius;			/* minimum label radius */
    int fixed_radius;			/* fixed label radius */
    int extra_radius;			/* extra label radius pad */
    int label_radius;			/* Radius of labels from menu center */
    int center_x, center_y;		/* Menu center */
    XSegment *segments;			/* Line segments to draw */

    /*
     * Miscellaneous information:
     */

    Tk_TimerToken popup_timer_token;
    Cursor cursor;
    PieMenuEntry *postedPie;
    int flags;
    int phase;
    int popup_delay;			/* Delay before popup */
    int shaped;				/* Use SHAPE extension */
} PieMenu;

/*
 * Flag bits for menus:
 *
 * REDRAW_PENDING:		Non-zero means a DoWhenIdle handler
 *				has already been queued to redraw
 *				this window.
 * UPDATE_PENDING:		Non-zero means a DoWhenIdle handler
 *				has already been queued to update
 *				this window.
 * RESIZE_PENDING:		Non-zero means a call to ComputeMenuGeometry
 *				has already been scheduled.
 * POPUP_PENDING:		Non-zero means a call to PopupPieMenu has
 *				already been scheduled.
 */

#define REDRAW_PENDING		1
#define UPDATE_PENDING		2
#define RESIZE_PENDING		4
#define POPUP_PENDING		8

/*
 * Configuration specs valid for the menu as a whole:
 */

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BORDER, "-activebackground", "activeBackground", "Foreground",
	PIE_ACTIVE_BG_COLOR, Tk_Offset(PieMenu, activeBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-activebackground", "activeBackground", "Foreground",
	PIE_ACTIVE_BG_MONO, Tk_Offset(PieMenu, activeBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_PIXELS, "-activeborderwidth", "activeBorderWidth", "BorderWidth",
	PIE_ACTIVE_BORDER_WIDTH, Tk_Offset(PieMenu, activeBorderWidth), 0},
    {TK_CONFIG_COLOR, "-activeforeground", "activeForeground", "Background",
	PIE_ACTIVE_FG_COLOR, Tk_Offset(PieMenu, activeFg),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-activeforeground", "activeForeground", "Background",
	PIE_ACTIVE_FG_MONO, Tk_Offset(PieMenu, activeFg),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	PIE_BG_COLOR, Tk_Offset(PieMenu, border), TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	PIE_BG_MONO, Tk_Offset(PieMenu, border), TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	PIE_BORDER_WIDTH, Tk_Offset(PieMenu, borderWidth), 0},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	"circle", Tk_Offset(PieMenu, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
        PIE_FG, Tk_Offset(PieMenu, fg), 0},
    {TK_CONFIG_FONT, "-font", "font", "Font",
	PIE_FONT, Tk_Offset(PieMenu, fontPtr), 0},
    {TK_CONFIG_STRING, "-title", (char *) NULL, (char *) NULL,
	"", Tk_Offset(PieMenu, title), 0},
    {TK_CONFIG_STRING, "-preview", (char *) NULL, (char *) NULL,
	"", Tk_Offset(PieMenu, preview), 0},
    {TK_CONFIG_FONT, "-titlefont", "font", "Font",
	PIE_FONT, Tk_Offset(PieMenu, titlefontPtr), 0},
    {TK_CONFIG_INT, "-initialangle", "initialAngle", "InitialAngle",
	"0", Tk_Offset(PieMenu, initial_angle), 0},
    {TK_CONFIG_INT, "-inactiveradius", "inactiveRadius", "InactiveRadius",
	PIE_INACTIVE_RADIUS, Tk_Offset(PieMenu, inactive_radius), 0},
    {TK_CONFIG_INT, "-minradius", "minRadius", "MinRadius",
        PIE_MIN_RADIUS, Tk_Offset(PieMenu, min_radius), 0},
    {TK_CONFIG_INT, "-extraradius", "extraRadius", "ExtraRadius",
        PIE_EXTRA_RADIUS, Tk_Offset(PieMenu, extra_radius), 0},
    {TK_CONFIG_INT, "-fixedradius", "fixedRadius", "FixedRadius",
	"0", Tk_Offset(PieMenu, fixed_radius), 0},
    {TK_CONFIG_INT, "-active", "active", "Active",
	"-1", Tk_Offset(PieMenu, active), 0},
    {TK_CONFIG_INT, "-popupdelay", "popupDelay", "PopupDelay",
	PIE_POPUP_DELAY, Tk_Offset(PieMenu, popup_delay), 0},
    {TK_CONFIG_INT, "-shaped", "shaped", "Shaped",
	"1", Tk_Offset(PieMenu, shaped), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Forward declarations for procedures defined later in this file:
 */

int		Tk_PieMenuCmd(ClientData clientData, Tcl_Interp *interp,
			      int argc, char **argv);
static int	ActivatePieMenuEntry _ANSI_ARGS_((PieMenu *menuPtr,
		    int index, int preview));
static void	ComputePieMenuGeometry _ANSI_ARGS_((
		    ClientData clientData));
static int	ConfigurePieMenu _ANSI_ARGS_((Tcl_Interp *interp,
		    PieMenu *menuPtr, int argc, char **argv,
		    int flags));
static int	ConfigurePieMenuEntry _ANSI_ARGS_((Tcl_Interp *interp,
		    PieMenu *menuPtr, PieMenuEntry *mePtr, int index,
		    int argc, char **argv, int flags));
static void	DestroyPieMenu _ANSI_ARGS_((ClientData clientData));
static void	DestroyPieMenuEntry _ANSI_ARGS_((ClientData clientData));
static void	DisplayPieMenu _ANSI_ARGS_((ClientData clientData));
static void	UpdatePieMenu _ANSI_ARGS_((ClientData clientData));
static void	PopupPieMenu _ANSI_ARGS_((ClientData clientData));
static void	EventuallyRedrawPieMenu _ANSI_ARGS_((PieMenu *menuPtr,
		    int index));
static int	GetPieMenuIndex _ANSI_ARGS_((Tcl_Interp *interp,
		    PieMenu *menuPtr, char *string, int *indexPtr));
static void	PieMenuEventProc _ANSI_ARGS_((ClientData clientData,
		    XEvent *eventPtr));
static int	PieMenuWidgetCmd _ANSI_ARGS_((ClientData clientData,
		    Tcl_Interp *interp, int argc, char **argv));
static int	UnpostSubPieMenu _ANSI_ARGS_((Tcl_Interp *interp,
		    PieMenu *menuPtr));
static void	PopupPieMenu _ANSI_ARGS_((ClientData clientData));
static void	NowPopupPieMenu _ANSI_ARGS_((PieMenu *menuPtr));
static void	NeverPopupPieMenu _ANSI_ARGS_((PieMenu *menuPtr));
static void	EventuallyPopupPieMenu _ANSI_ARGS_((PieMenu *menuPtr));
static void	DeferPopupPieMenu _ANSI_ARGS_((PieMenu *menuPtr));
static void	ShapePieMenu _ANSI_ARGS_((PieMenu *menuPtr));


/*
 *--------------------------------------------------------------
 *
 * Tk_PieMenuCmd --
 *
 *	This procedure is invoked to process the "piemenu" Tcl
 *	command.  Read the code and write some user documentation for
 *	details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation for "menu", which this was based on.
 *
 *--------------------------------------------------------------
 */

int
Tk_PieMenuCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    Tk_Window new;
    register PieMenu *menuPtr;
    XSetWindowAttributes atts;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Create the new window.  Set override-redirect so the window
     * manager won't add a border or argue about placement, and set
     * save-under so that the window can pop up and down without a
     * lot of re-drawing.
     */

    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], "");
    if (new == NULL) {
	return TCL_ERROR;
    }
    atts.override_redirect = True;
    atts.save_under = True;
    Tk_ChangeWindowAttributes(new, CWOverrideRedirect|CWSaveUnder, &atts);

    /*
     * Initialize the data structure for the menu.
     */

    menuPtr = (PieMenu *) ckalloc(sizeof(PieMenu));
    menuPtr->tkwin = new;
    menuPtr->interp = interp;
    menuPtr->title = NULL;
    menuPtr->titleLength = 0;
    menuPtr->preview = NULL;
    menuPtr->entries = NULL;
    menuPtr->numEntries = 0;
    menuPtr->active = -1;
    menuPtr->group = NULL; 
    menuPtr->root_x = 0;
    menuPtr->root_y = 0; 
    menuPtr->border = NULL;
    menuPtr->activeBorder = NULL;
    menuPtr->fontPtr = NULL;
    menuPtr->titlefontPtr = NULL;
    menuPtr->fg = NULL;
    menuPtr->textGC = None;
    menuPtr->activeFg = NULL;
    menuPtr->activeGC = None;
    menuPtr->width = 0;
    menuPtr->height = 0;
    menuPtr->title_x = 0;
    menuPtr->title_y = 0;
    menuPtr->title_width = 0;
    menuPtr->title_height = 0;
    menuPtr->initial_angle = 0;
    menuPtr->inactive_radius = PIE_INACTIVE_RADIUS_NUM;
    menuPtr->min_radius = PIE_MIN_RADIUS_NUM;
    menuPtr->extra_radius = PIE_EXTRA_RADIUS_NUM;
    menuPtr->fixed_radius = 0;
    menuPtr->label_radius = 0;
    menuPtr->center_x = 0;
    menuPtr->center_y = 0;
    menuPtr->segments = NULL;
    menuPtr->cursor = None;
    menuPtr->postedPie = NULL;
    menuPtr->flags = 0;
    menuPtr->phase = 0;
    menuPtr->shaped = 1;
    menuPtr->popup_delay = PIE_POPUP_DELAY_NUM;

    Tk_SetClass(new, "PieMenu");
    Tk_CreateEventHandler(menuPtr->tkwin,
			  ExposureMask | StructureNotifyMask |
			  ButtonPressMask | ButtonReleaseMask |
			  PointerMotionMask,
			  PieMenuEventProc, (ClientData) menuPtr);
    Tcl_CreateCommand(interp, Tk_PathName(menuPtr->tkwin), PieMenuWidgetCmd,
	    (ClientData) menuPtr, (void (*)()) NULL);
    if (ConfigurePieMenu(interp, menuPtr, argc-2, argv+2, 0) != TCL_OK) {
	goto error;
    }

    interp->result = Tk_PathName(menuPtr->tkwin);
    return TCL_OK;

    error:
    Tk_DestroyWindow(menuPtr->tkwin);
    return TCL_ERROR;
}

/*
 *--------------------------------------------------------------
 *
 * PieMenuWidgetCmd --
 *
 *	This procedure is invoked to process the Tcl command
 *	that corresponds to a widget managed by this module.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *--------------------------------------------------------------
 */

static int
PieMenuWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Information about menu widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register PieMenu *menuPtr = (PieMenu *) clientData;
    register PieMenuEntry *mePtr;
    int result = TCL_OK;
    int length, type;
    char c;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) menuPtr);
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'a') && (strncmp(argv[1], "activate", length) == 0)
	    && (length >= 2)) {
	int index;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " activate index\"", (char *) NULL);
	    goto error;
	}
	if (GetPieMenuIndex(interp, menuPtr, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	if (menuPtr->active == index) {
	    goto done;
	}
	result = ActivatePieMenuEntry(menuPtr, index, 1);
	DeferPopupPieMenu(menuPtr);
    } else if ((c == 's') && (strncmp(argv[1], "show", length) == 0)
	    && (length >= 2)) {
	int index;

	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " show\"", (char *) NULL);
	    goto error;
	}
	NowPopupPieMenu(menuPtr);
    } else if ((c == 'p') && (strncmp(argv[1], "pending", length) == 0)
	    && (length >= 2)) {
	int index;

	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " pending\"", (char *) NULL);
	    goto error;
	}
	sprintf(interp->result, "%d",
		(menuPtr->flags & POPUP_PENDING) ? 1 : 0);
    } else if ((c == 'd') && (strncmp(argv[1], "defer", length) == 0)
	    && (length >= 2)) {
	int index;

	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " defer\"", (char *) NULL);
	    goto error;
	}
	DeferPopupPieMenu(menuPtr);
    } else if ((c == 'a') && (strncmp(argv[1], "add", length) == 0)
	    && (length >= 2)) {
	PieMenuEntry **newEntries;

	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " add type ?options?\"", (char *) NULL);
	    goto error;
	}

	/*
	 * Figure out the type of the new entry.
	 */

	c = argv[2][0];
	length = strlen(argv[2]);
	if ((c == 'c') && (strncmp(argv[2], "command", length) == 0)) {
	    type = COMMAND_ENTRY;
	} else if ((c == 'p') && (strncmp(argv[2], "piemenu", length) == 0)) {
	    type = PIEMENU_ENTRY;
	} else {
	    Tcl_AppendResult(interp, "bad menu entry type \"",
			     argv[2], "\":  must be command or piemenu",
			     (char *) NULL);
	    goto error;
	}

	/*
	 * Add a new entry to the end of the menu's array of entries,
	 * and process options for it.
	 */

	mePtr = (PieMenuEntry *) ckalloc(sizeof(PieMenuEntry));
	newEntries = (PieMenuEntry **) ckalloc((unsigned)
		((menuPtr->numEntries+1)*sizeof(PieMenuEntry *)));
	if (menuPtr->numEntries != 0) {
	    memcpy((VOID *) newEntries, (VOID *) menuPtr->entries,
		    menuPtr->numEntries*sizeof(PieMenuEntry *));
	    ckfree((char *) menuPtr->entries);
	}
	menuPtr->entries = newEntries;
	menuPtr->entries[menuPtr->numEntries] = mePtr;
	menuPtr->numEntries++;
	mePtr->type = type;
	mePtr->piemenuPtr = menuPtr;
	mePtr->label = NULL;
	mePtr->labelLength = 0;
	mePtr->bitmap = None;
	mePtr->width = 0;
	mePtr->height = 0;
	mePtr->x_offset = 0;
	mePtr->y_offset = 0;
	mePtr->label_x = 0;
	mePtr->label_y = 0;
	mePtr->border = NULL;
	mePtr->activeBorder = NULL;
	mePtr->fontPtr = NULL;
	mePtr->textGC = None;
	mePtr->activeGC = None;
	mePtr->slice = 1.0;
	mePtr->angle = 0.0;
	mePtr->dx = 0.0;
	mePtr->dy = 0.0;
	mePtr->subtend = 0.0;
	mePtr->quadrant = 0;
	mePtr->slope = 0.0;
	mePtr->command = NULL;
	mePtr->preview = NULL;
	mePtr->name = NULL;
	mePtr->flags = 0;
	if (ConfigurePieMenuEntry(interp, menuPtr, mePtr,
				  menuPtr->numEntries-1,
				  argc-3, argv+3, 0) != TCL_OK) {
	    DestroyPieMenuEntry((ClientData) mePtr);
	    menuPtr->numEntries--;
	    goto error;
	}
	if (!(menuPtr->flags & RESIZE_PENDING)) {
	    menuPtr->flags |= RESIZE_PENDING;
	    Tk_DoWhenIdle(ComputePieMenuGeometry, (ClientData) menuPtr);
	}
    } else if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)) {
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, menuPtr->tkwin, configSpecs,
		    (char *) menuPtr, (char *) NULL, 0);
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, menuPtr->tkwin, configSpecs,
		    (char *) menuPtr, argv[2], 0);
	} else {
	    result = ConfigurePieMenu(interp, menuPtr, argc-2, argv+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else if ((c == 'd') && (strncmp(argv[1], "delete", length) == 0)
	    && (length >= 2)) {
	int index, i;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " delete index\"", (char *) NULL);
	    goto error;
	}
	if (GetPieMenuIndex(interp, menuPtr, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	if (index < 0) {
	    goto done;
	}
	Tk_EventuallyFree((ClientData) menuPtr->entries[index],
			  DestroyPieMenuEntry);
	for (i = index; i < menuPtr->numEntries-1; i++) {
	    menuPtr->entries[i] = menuPtr->entries[i+1];
	}
	menuPtr->numEntries -= 1;
	if (menuPtr->active == index) {
	    menuPtr->active = -1;
	} else if (menuPtr->active > index) {
	    menuPtr->active -= 1;
	}
	if (!(menuPtr->flags & RESIZE_PENDING)) {
	    menuPtr->flags |= RESIZE_PENDING;
	    Tk_DoWhenIdle(ComputePieMenuGeometry, (ClientData) menuPtr);
	}
    } else if ((c == 'e') && (length >= 3)
	    && (strncmp(argv[1], "entryconfigure", length) == 0)) {
	int index;

	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " entryconfigure index ?option value ...?\"",
		    (char *) NULL);
	    goto error;
	}
	if (GetPieMenuIndex(interp, menuPtr, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	if (index < 0) {
	    goto done;
	}
	mePtr = menuPtr->entries[index];
	Tk_Preserve((ClientData) mePtr);
	if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, menuPtr->tkwin, entryConfigSpecs,
		    (char *) mePtr, (char *) NULL,
		    COMMAND_MASK << mePtr->type);
	} else if (argc == 4) {
	    result = Tk_ConfigureInfo(interp, menuPtr->tkwin, entryConfigSpecs,
		    (char *) mePtr, argv[3], COMMAND_MASK << mePtr->type);
	} else {
	    result = ConfigurePieMenuEntry(interp, menuPtr, mePtr, index,
					   argc-3, argv+3,
					   TK_CONFIG_ARGV_ONLY |
					     COMMAND_MASK << mePtr->type);
	}
	Tk_Release((ClientData) mePtr);
    } else if ((c == 'i') && (strncmp(argv[1], "index", length) == 0)
	    && (length >= 3)) {
	int index;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " index string\"", (char *) NULL);
	    goto error;
	}
	if (GetPieMenuIndex(interp, menuPtr, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	if (index < 0) {
	    interp->result = "none";
	} else {
	    sprintf(interp->result, "%d", index);
	}
    } else if ((c == 'i') && (strncmp(argv[1], "invoke", length) == 0)
	    && (length >= 3)) {
	int index;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " invoke index\"", (char *) NULL);
	    goto error;
	}
	if (GetPieMenuIndex(interp, menuPtr, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	if (index < 0) {
	    goto done;
	}
	mePtr = menuPtr->entries[index];
	Tk_Preserve((ClientData) mePtr);
	if (mePtr->command != NULL) {
	    result = Tcl_GlobalEval(interp, mePtr->command);
	}
	Tk_Release((ClientData) mePtr);
    } else if ((c == 'p') && (strncmp(argv[1], "post", length) == 0)) {
	int x, y, ix, iy, tmp, err;
	Tk_Uid group;

	if ((argc != 4) && (argc != 5)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " post x y ?group?\"", (char *) NULL);
	    goto error;
	}
	if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK)
		|| (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
	    goto error;
	}
	if (argc == 5) {
	    group = Tk_GetUid(argv[4]);
	} else {
	    group = Tk_GetUid("default");
	}

	/*
	 * Adjust the position of the menu if necessary to keep it
	 * on-screen.
	 */

	x -= menuPtr->center_x; y -= menuPtr->center_y;
#if 0
	ix = x; iy = y;

	tmp = WidthOfScreen(Tk_Screen(menuPtr->tkwin))
		- Tk_Width(menuPtr->tkwin);
	if (x > tmp) {
	    x = tmp;
	}
	if (x < 0) {
	    x = 0;
	}
	tmp = HeightOfScreen(Tk_Screen(menuPtr->tkwin))
		- Tk_Height(menuPtr->tkwin);
	if (y > tmp) {
	    y = tmp;
	}
	if (y < 0) {
	    y = 0;
	}

	/* XXX: warp pointer by (x-ix, y-iy) upon popup */
#endif

	Tk_MakeWindowExist(menuPtr->tkwin);
	XRaiseWindow(Tk_Display(menuPtr->tkwin), Tk_WindowId(menuPtr->tkwin));

	Tk_MoveWindow(menuPtr->tkwin, x, y);
	menuPtr->root_x = x + menuPtr->center_x;
	menuPtr->root_y = y + menuPtr->center_y;
	
	if (Tk_IsMapped(menuPtr->tkwin)) {
	    if (group != menuPtr->group) {
		Tk_UnshareEvents(menuPtr->tkwin, menuPtr->group);
		Tk_ShareEvents(menuPtr->tkwin, group);
	    }
	} else {
	    Tk_ShareEvents(menuPtr->tkwin, group);
	    EventuallyPopupPieMenu(menuPtr);
	    result = ActivatePieMenuEntry(menuPtr, -1, 1);
	}
	menuPtr->group = group;
    } else if ((c == 'u') && (strncmp(argv[1], "unpost", length) == 0)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " unpost\"", (char *) NULL);
	    goto error;
	}
	NeverPopupPieMenu(menuPtr);
	Tk_UnshareEvents(menuPtr->tkwin, menuPtr->group);
	Tk_UnmapWindow(menuPtr->tkwin);
	result = ActivatePieMenuEntry(menuPtr, -1, 0);
	if (result == TCL_OK) {
	    result = UnpostSubPieMenu(interp, menuPtr);
	}
    } else if ((c == 'g') && (strncmp(argv[1], "grab", length) == 0)) {
      Tk_Window tkwin;
      int err;

      if ((argc != 3) ||
	  ((tkwin = Tk_NameToWindow(interp, argv[2],
				    menuPtr->tkwin)) == NULL)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
			 argv[0], " grab window\"", (char *) NULL);
	goto error;
      }
      
      err =
	XGrabPointer(Tk_Display(tkwin),
		     Tk_WindowId(tkwin),
		     False,
		     ButtonPressMask | ButtonReleaseMask |
		     ButtonMotionMask | PointerMotionMask,
		     GrabModeAsync, GrabModeAsync, None, None,
		     TkCurrentTime(((TkWindow *)tkwin)->dispPtr));
      if (err != 0) {
	if (err == GrabNotViewable) {
	  interp->result = "grab failed: window not viewable";
	} else if (err == AlreadyGrabbed) {
	  interp->result = "grab failed: another application has grab";
	} else if (err == GrabFrozen) {
	  interp->result = "grab failed: keyboard or pointer frozen";
	} else if (err == GrabInvalidTime) {
	  interp->result = "grab failed: invalid time";
	} else {
	  char msg[100];
	  
	  sprintf(msg, "grab failed for unknown reason (code %d)",
		  err);
	  Tcl_AppendResult(interp, msg, (char *) NULL);
	}
	return TCL_ERROR;
      }
    } else if ((c == 'u') && (strncmp(argv[1], "ungrab", length) == 0)) {
      Tk_Window tkwin;

      if ((argc != 3) ||
	  ((tkwin = Tk_NameToWindow(interp, argv[2],
				    menuPtr->tkwin)) == NULL)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
			 argv[0], " ungrab window\"", (char *) NULL);
	goto error;
      }

      XUngrabPointer(Tk_Display(tkwin),
		     TkCurrentTime(((TkWindow *)tkwin)->dispPtr));

    } else if ((c == 'd') && (strncmp(argv[1], "distance", length) == 0)
	    && (length >= 3)) {
	int distance;

	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " distance\"", (char *) NULL);
	    goto error;
	}
	distance = (int)(sqrt((menuPtr->dx * menuPtr->dx) + (menuPtr->dy * menuPtr->dy)) + 0.499);
	sprintf(interp->result, "%d", distance);
    } else if ((c == 'd') && (strncmp(argv[1], "direction", length) == 0)
	    && (length >= 3)) {
	int direction;

	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " direction\"", (char *) NULL);
	    goto error;
	}
	direction = (int)(RAD_TO_DEG(atan2(menuPtr->dy, menuPtr->dx)) + 0.499);
	if (direction < 0) direction += 360;
	sprintf(interp->result, "%d", direction);
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\": must be activate, show, add, configure, delete, ",
		"entryconfigure, index, invoke, post, unpost, pending, ",
		"defer, grab, or ungrab", (char *) NULL);
	goto error;
    }
    done:
    Tk_Release((ClientData) menuPtr);
    return result;

    error:
    Tk_Release((ClientData) menuPtr);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyPieMenu --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a pie menu at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the pie menu is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyPieMenu(clientData)
    ClientData clientData;	/* Info about menu widget. */
{
    register PieMenu *menuPtr = (PieMenu *) clientData;
    int i;

    /* Should we delete the event handler? */

    for (i = 0; i < menuPtr->numEntries; i++) {
	DestroyPieMenuEntry((ClientData) menuPtr->entries[i]);
    }
    if (menuPtr->entries != NULL) {
	ckfree((char *) menuPtr->entries);
    }
    if (menuPtr->border != NULL) {
	Tk_Free3DBorder(menuPtr->border);
    }
    if (menuPtr->activeBorder != NULL) {
	Tk_Free3DBorder(menuPtr->activeBorder);
    }
    if (menuPtr->fontPtr != NULL) {
	Tk_FreeFontStruct(menuPtr->fontPtr);
    }
    if (menuPtr->fg != NULL) {
	Tk_FreeColor(menuPtr->fg);
    }
    if (menuPtr->textGC != None) {
	Tk_FreeGC(menuPtr->textGC);
    }
    if (menuPtr->activeFg != NULL) {
	Tk_FreeColor(menuPtr->activeFg);
    }
    if (menuPtr->activeGC != None) {
	Tk_FreeGC(menuPtr->activeGC);
    }
    if (menuPtr->cursor != None) {
	Tk_FreeCursor(menuPtr->cursor);
    }
    ckfree((char *) menuPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyPieMenuEntry --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a pie menu entry at a safe 
 *	time (when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the pie menu entry is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyPieMenuEntry(clientData)
    ClientData clientData;		/* Pointer to entry to be freed. */
{
    register PieMenuEntry *mePtr = (PieMenuEntry *) clientData;
    PieMenu *menuPtr = mePtr->piemenuPtr;

    if (menuPtr->postedPie == mePtr) {
	if (UnpostSubPieMenu(menuPtr->interp, menuPtr)
		!= TCL_OK) {
	    TkBindError(menuPtr->interp);
	}
    }
    if (mePtr->label != NULL) {
	ckfree(mePtr->label);
    }
    if (mePtr->bitmap != None) {
	Tk_FreePixmap(mePtr->bitmap);
    }
    if (mePtr->border != NULL) {
	Tk_Free3DBorder(mePtr->border);
    }
    if (mePtr->activeBorder != NULL) {
	Tk_Free3DBorder(mePtr->activeBorder);
    }
    if (mePtr->fontPtr != NULL) {
	Tk_FreeFontStruct(mePtr->fontPtr);
    }
    if (mePtr->textGC != NULL) {
	Tk_FreeGC(mePtr->textGC);
    }
    if (mePtr->activeGC != NULL) {
	Tk_FreeGC(mePtr->activeGC);
    }
    if (mePtr->command != NULL) {
	ckfree(mePtr->command);
    }
    if (mePtr->name != NULL) {
	ckfree(mePtr->name);
    }
    ckfree((char *) mePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigurePieMenu --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a menu widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as colors, font, etc. get set
 *	for menuPtr;  old resources get freed, if there were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigurePieMenu(interp, menuPtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register PieMenu *menuPtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    XGCValues gcValues;
    GC newGC;
    int i;

    if (Tk_ConfigureWidget(interp, menuPtr->tkwin, configSpecs,
	    argc, argv, (char *) menuPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few options need special processing, such as setting the
     * background from a 3-D border, or filling in complicated
     * defaults that couldn't be specified to Tk_ConfigureWidget.
     */

    if (menuPtr->title == NULL) {
	menuPtr->titleLength = 0;
    } else {
	menuPtr->titleLength = strlen(menuPtr->title);
    }

    Tk_SetBackgroundFromBorder(menuPtr->tkwin, menuPtr->border);

    gcValues.font = menuPtr->fontPtr->fid;
    gcValues.foreground = menuPtr->fg->pixel;
    gcValues.background = Tk_3DBorderColor(menuPtr->border)->pixel;
    newGC = Tk_GetGC(menuPtr->tkwin, GCForeground|GCBackground|GCFont,
	    &gcValues);
    if (menuPtr->textGC != None) {
	Tk_FreeGC(menuPtr->textGC);
    }
    menuPtr->textGC = newGC;

    gcValues.font = menuPtr->fontPtr->fid;
    gcValues.foreground = menuPtr->activeFg->pixel;
    gcValues.background = Tk_3DBorderColor(menuPtr->activeBorder)->pixel;
    newGC = Tk_GetGC(menuPtr->tkwin, GCForeground|GCBackground|GCFont,
	    &gcValues);
    if (menuPtr->activeGC != None) {
	Tk_FreeGC(menuPtr->activeGC);
    }
    menuPtr->activeGC = newGC;

    /*
     * After reconfiguring a menu, we need to reconfigure all of the
     * entries in the menu, since some of the things in the children
     * (such as graphics contexts) may have to change to reflect changes
     * in the parent.
     */

    for (i = 0; i < menuPtr->numEntries; i++) {
	PieMenuEntry *mePtr;

	mePtr = menuPtr->entries[i];
	ConfigurePieMenuEntry(interp, menuPtr, mePtr, i, 0, (char **) NULL,
		TK_CONFIG_ARGV_ONLY | COMMAND_MASK << mePtr->type);
    }

    if (!(menuPtr->flags & RESIZE_PENDING)) {
	menuPtr->flags |= RESIZE_PENDING;
	Tk_DoWhenIdle(ComputePieMenuGeometry, (ClientData) menuPtr);
    }

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigurePieMenuEntry --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) one entry in a menu.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information such as label and accelerator get
 *	set for mePtr;  old resources get freed, if there were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigurePieMenuEntry(interp, menuPtr, mePtr, index, argc, argv, flags)
    Tcl_Interp *interp;			/* Used for error reporting. */
    PieMenu *menuPtr;			/* Information about whole menu. */
    register PieMenuEntry *mePtr;	/* Information about menu entry;  may
					 * or may not already have values for
					 * some fields. */
    int index;				/* Index of mePtr within menuPtr's
					 * entries. */
    int argc;				/* Number of valid entries in argv. */
    char **argv;			/* Arguments. */
    int flags;				/* Additional flags to pass to
					 * Tk_ConfigureWidget. */
{
    XGCValues gcValues;
    GC newGC, newActiveGC;

    /*
     * If this entry is a piemenu and the piemenu is posted, then unpost
     * it before reconfiguring the entry (otherwise the reconfigure might
     * change the name of the piemenu entry, leaving a posted menu
     * high and dry).
     */

    if (menuPtr->postedPie == mePtr) {
	if (UnpostSubPieMenu(menuPtr->interp, menuPtr)
		!= TCL_OK) {
	    TkBindError(menuPtr->interp);
	}
    }

    if (Tk_ConfigureWidget(interp, menuPtr->tkwin, entryConfigSpecs,
	    argc, argv, (char *) mePtr,
	    flags | (COMMAND_MASK << mePtr->type)) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * The code below handles special configuration stuff not taken
     * care of by Tk_ConfigureWidget, such as special processing for
     * defaults, sizing strings, graphics contexts, etc.
     */

    if (mePtr->label == NULL) {
	mePtr->labelLength = 0;
    } else {
	mePtr->labelLength = strlen(mePtr->label);
    }

    if (index != menuPtr->active) {
	ActivatePieMenuEntry(menuPtr, index, 0);
    }

    if ((mePtr->fontPtr != NULL) ||
	(mePtr->type == PIEMENU_ENTRY)) {
	gcValues.foreground = menuPtr->fg->pixel;
	gcValues.background = Tk_3DBorderColor(
		(mePtr->border != NULL) ? mePtr->border : menuPtr->border)
		->pixel;
	if (mePtr->fontPtr != NULL) {
	  gcValues.font = mePtr->fontPtr->fid;
	} else {
	  if (menuPtr->titlefontPtr != NULL)
	    gcValues.font = menuPtr->titlefontPtr->fid;
	  else
	    gcValues.font = menuPtr->fontPtr->fid;
	}

	/*
	 * Note: disable GraphicsExpose events;  we know there won't be
	 * obscured areas when copying from an off-screen pixmap to the
	 * screen and this gets rid of unnecessary events.
	 */

	gcValues.graphics_exposures = False;
	newGC = Tk_GetGC(menuPtr->tkwin,
		GCForeground|GCBackground|GCFont|GCGraphicsExposures,
		&gcValues);

	gcValues.foreground = menuPtr->activeFg->pixel;
	gcValues.background = Tk_3DBorderColor(
		(mePtr->activeBorder != NULL) ? mePtr->activeBorder
		: menuPtr->activeBorder)->pixel;
	newActiveGC = Tk_GetGC(menuPtr->tkwin,
		GCForeground|GCBackground|GCFont|GCGraphicsExposures,
		&gcValues);
    } else {
	newGC = NULL;
	newActiveGC = NULL;
    }

    if (mePtr->textGC != NULL) {
	    Tk_FreeGC(mePtr->textGC);
    }
    mePtr->textGC = newGC;

    if (mePtr->activeGC != NULL) {
	    Tk_FreeGC(mePtr->activeGC);
    }
    mePtr->activeGC = newActiveGC;

    if (!(menuPtr->flags & RESIZE_PENDING)) {
	menuPtr->flags |= RESIZE_PENDING;
	Tk_DoWhenIdle(ComputePieMenuGeometry, (ClientData) menuPtr);
    }
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * ComputePieMenuGeometry --
 *
 *	This procedure is invoked to recompute the size and
 *	layout of a menu.  It is called as a when-idle handler so
 *	that it only gets done once, even if a group of changes is
 *	made to the menu.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Fields of menu entries are changed to reflect their
 *	current positions, and the size of the menu window
 *	itself may be changed.
 *
 *--------------------------------------------------------------
 */

static void
ComputePieMenuGeometry(clientData)
    ClientData clientData;		/* Structure describing menu. */
{
    PieMenu *menuPtr = (PieMenu *) clientData;

    if (menuPtr->tkwin == NULL) {
	return;
    }

    LayoutPieMenu(menuPtr);

    if ((menuPtr->width != Tk_ReqWidth(menuPtr->tkwin)) ||
	(menuPtr->height != Tk_ReqHeight(menuPtr->tkwin))) {
	Tk_GeometryRequest(menuPtr->tkwin, menuPtr->width, menuPtr->height);
    } else {
	/*
	 * Must always force a redisplay here if the window is mapped
	 * (even if the size didn't change, something else might have
	 * changed in the menu, such as a label or accelerator).  The
	 * resize will force a redisplay above.
	 */

	EventuallyRedrawPieMenu(menuPtr, -1);
    }

    menuPtr->flags &= ~RESIZE_PENDING;
}

/*
 *----------------------------------------------------------------------
 *
 * DisplayPieMenu --
 *
 *	This procedure is invoked to display a pie menu widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Commands are output to X to display the pie menu in its
 *	current mode.
 *
 *----------------------------------------------------------------------
 */

static void
DisplayPieMenu(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register PieMenu *menuPtr = (PieMenu *) clientData;
    register Tk_Window tkwin = menuPtr->tkwin;
    XFontStruct *fontPtr;
    int index;

    menuPtr->flags &= ~REDRAW_PENDING;
    if ((menuPtr->tkwin == NULL) || !Tk_IsMapped(menuPtr->tkwin)) {
	return;
    }

    if (menuPtr->titlefontPtr != NULL) {
	fontPtr = menuPtr->titlefontPtr;
    } else {
	fontPtr = menuPtr->fontPtr;
    }

    if (menuPtr->titleLength != 0) {
      Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin),
			 menuPtr->border,
			 menuPtr->borderWidth, menuPtr->borderWidth,
			 Tk_Width(tkwin) - 2*menuPtr->borderWidth,
			 menuPtr->title_height + 2*menuPtr->borderWidth,
			 menuPtr->borderWidth, TK_RELIEF_RAISED);

      TkDisplayChars(Tk_Display(tkwin), Tk_WindowId(tkwin), menuPtr->textGC,
		     fontPtr, menuPtr->title, menuPtr->titleLength,
		     menuPtr->title_x, menuPtr->title_y,
		     TK_NEWLINES_NOT_SPECIAL);
    }

    if (menuPtr->segments) { 
      XSetLineAttributes(Tk_Display(tkwin), menuPtr->textGC,
			 0, LineSolid, CapButt, JoinMiter);
      XDrawSegments(Tk_Display(tkwin), Tk_WindowId(tkwin),
		    menuPtr->textGC, menuPtr->segments, menuPtr->numEntries);
    }
    
    Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin), menuPtr->border,
		       0, 0, Tk_Width(tkwin), Tk_Height(tkwin),
		       menuPtr->borderWidth, TK_RELIEF_RAISED);

    UpdatePieMenuEntries(menuPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * UpdatePieMenu --
 *
 *	This procedure is invoked to update a pie menu widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Commands are output to X to update the pie menu in its
 *	current mode.
 *
 *----------------------------------------------------------------------
 */

static void
UpdatePieMenu(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register PieMenu *menuPtr = (PieMenu *) clientData;

    menuPtr->flags &= ~UPDATE_PENDING;
    if ((menuPtr->tkwin == NULL) || !Tk_IsMapped(menuPtr->tkwin)) {
	return;
    }

    UpdatePieMenuEntries(menuPtr);
}


UpdatePieMenuEntries(menuPtr)
    PieMenu *menuPtr;
{
    register PieMenuEntry *mePtr;
    register Tk_Window tkwin = menuPtr->tkwin;
    XFontStruct *fontPtr;
    int index;
    GC gc;

    for (index = 0; index < menuPtr->numEntries; index++) {
	mePtr = menuPtr->entries[index];
	if (!(mePtr->flags & ENTRY_NEEDS_REDISPLAY)) {
	    continue;
	}
	mePtr->flags &= ~ENTRY_NEEDS_REDISPLAY;

	/*
	 * Background.
	 */

	Tk_Fill3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin),
			   ((mePtr->activeBorder != NULL)
			      ? mePtr->activeBorder
			      : menuPtr->activeBorder),
			   mePtr->x, mePtr->y,
			   mePtr->width, mePtr->height,
			   menuPtr->activeBorderWidth,
			   ((index == menuPtr->active)
			      ? TK_RELIEF_SUNKEN
			      : ((HaveShape && menuPtr->shaped)
			           ? TK_RELIEF_RAISED
			           : TK_RELIEF_FLAT)));

	gc = mePtr->textGC;
	if (gc == NULL) {
	    gc = menuPtr->textGC;
	}

	/*
	 * Draw label or bitmap for entry.
	 */

	fontPtr = mePtr->fontPtr;
	if (fontPtr == NULL) {
	    fontPtr = menuPtr->fontPtr;
	}
	if (mePtr->bitmap != None) {
	    unsigned int width, height;

	    Tk_SizeOfPixmap(mePtr->bitmap, &width, &height);
            XCopyArea(Tk_Display(tkwin), mePtr->bitmap, Tk_WindowId(tkwin),
		      gc, 0, 0, width, height,
		      mePtr->label_x, mePtr->label_y);
	} else {
	    if (mePtr->label != NULL) {
		TkDisplayChars(Tk_Display(tkwin), Tk_WindowId(tkwin), gc,
			       fontPtr, mePtr->label, mePtr->labelLength,
			       mePtr->label_x, mePtr->label_y,
			       TK_NEWLINES_NOT_SPECIAL);
	    }
	}
    }
}

/*
 *--------------------------------------------------------------
 *
 * GetPieMenuIndex --
 *
 *	Parse a textual index into a pie menu and return the numerical
 *	index of the indicated entry.
 *
 * Results:
 *	A standard Tcl result.  If all went well, then *indexPtr is
 *	filled in with the entry index corresponding to string
 *	(ranges from -1 to the number of entries in the pie menu minus
 *	one).  Otherwise an error message is left in interp->result.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

static int
GetPieMenuIndex(interp, menuPtr, string, indexPtr)
    Tcl_Interp *interp;		/* For error messages. */
    PieMenu *menuPtr;		/* Menu for which the index is being
				 * specified. */
    char *string;		/* Specification of an entry in menu.  See
				 * manual entry for valid .*/
    int *indexPtr;		/* Where to store converted relief. */
{
    int i, y;

    if ((string[0] == 'a') && (strcmp(string, "active") == 0)) {
	*indexPtr = menuPtr->active;
	return TCL_OK;
    }

    if ((string[0] == 'l') && (strcmp(string, "last") == 0)) {
	*indexPtr = menuPtr->numEntries-1;
	return TCL_OK;
    }

    if ((string[0] == 'n') && (strcmp(string, "none") == 0)) {
	*indexPtr = -1;
	return TCL_OK;
    }

    if (string[0] == '@') {
        char xstr[32], ystr[32];
        int x, y;

	if ((sscanf(&string[1], "%31[^,],%31[^,]", xstr, ystr) == 2) &&
	    (Tcl_GetInt(interp, xstr,  &x) == TCL_OK) &&
	    (Tcl_GetInt(interp, ystr,  &y) == TCL_OK)) {
	    *indexPtr = CalcPieMenuItem(menuPtr, x, y);
	    return TCL_OK;
	} else {
	    Tcl_SetResult(interp, (char *) NULL, TCL_STATIC);
	}
    }

    if (isdigit(string[0])) {
	if (Tcl_GetInt(interp, string,  &i) == TCL_OK) {
	    if ((i < menuPtr->numEntries) && (i >= 0)) {
		*indexPtr = i;
		return TCL_OK;
	    }
	} else {
	    Tcl_SetResult(interp, (char *) NULL, TCL_STATIC);
	}
    }

    for (i = 0; i < menuPtr->numEntries; i++) {
	char *label;

	label = menuPtr->entries[i]->label;
	if ((label != NULL)
		&& (Tcl_StringMatch(menuPtr->entries[i]->label, string))) {
	    *indexPtr = i;
	    return TCL_OK;
	}
    }

    Tcl_AppendResult(interp, "bad menu entry index \"",
		     string, "\"", (char *) NULL);
    return TCL_ERROR;
}

/*
 *--------------------------------------------------------------
 *
 * PieMenuEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher for various
 *	events on pie menus.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	When the window gets deleted, internal structures get
 *	cleaned up.  When it gets exposed, it is redisplayed.
 *
 *--------------------------------------------------------------
 */

static void
PieMenuEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    PieMenu *menuPtr = (PieMenu *) clientData;
    switch (eventPtr->type) {
    case Expose:
	if (eventPtr->xexpose.count == 0) {
	    EventuallyRedrawPieMenu(menuPtr, -1);
	}
	break;
    case DestroyNotify:
	Tcl_DeleteCommand(menuPtr->interp, Tk_PathName(menuPtr->tkwin));

	/*
	 * Careful!  Must delete the event-sharing information here
	 * rather than in DestroyPieMenu.  By the time that procedure
	 * is called the tkwin may have been reused, resulting in some
	 * other window accidentally being cut off from shared events.
	 */

	Tk_UnshareEvents(menuPtr->tkwin, menuPtr->group);
	menuPtr->tkwin = NULL;
	if (menuPtr->flags & REDRAW_PENDING) {
	    Tk_CancelIdleCall(DisplayPieMenu, (ClientData) menuPtr);
	}
	if (menuPtr->flags & UPDATE_PENDING) {
	    Tk_CancelIdleCall(UpdatePieMenu, (ClientData) menuPtr);
	}
	if (menuPtr->flags & RESIZE_PENDING) {
	    Tk_CancelIdleCall(ComputePieMenuGeometry, (ClientData) menuPtr);
	}
	if (menuPtr->flags & POPUP_PENDING) {
	    Tk_CancelIdleCall(PopupPieMenu, (ClientData) menuPtr);
	}
	Tk_EventuallyFree((ClientData) menuPtr, DestroyPieMenu);
	break;
    case MotionNotify:
	break;
    case ButtonPress:
	break;
    case ButtonRelease:
	break;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * EventuallyRedrawPieMenu --
 *
 *	Arrange for an entry of a pie menu, or the whole pie menu, 
 *	to be redisplayed at some point in the future.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A when-idle hander is scheduled to do the redisplay, if there
 *	isn't one already scheduled.
 *
 *----------------------------------------------------------------------
 */

static void
EventuallyRedrawPieMenu(menuPtr, index)
    register PieMenu *menuPtr;	/* Information about menu to redraw. */
    int index;			/* Which entry to redraw.  If -1, then
				 * all the entries in the menu are redrawn. */
{
    if (menuPtr->tkwin == NULL) {
	return;
    }
    if (index != -1) {
	menuPtr->entries[index]->flags |= ENTRY_NEEDS_REDISPLAY;
    } else {
	for (index = 0; index < menuPtr->numEntries; index++) {
	    menuPtr->entries[index]->flags |= ENTRY_NEEDS_REDISPLAY;
	}
	index = -1;
    }
    if ((menuPtr->tkwin == NULL) || !Tk_IsMapped(menuPtr->tkwin)
	    || (menuPtr->flags & REDRAW_PENDING)) {
	return;
    }

    if (index == -1) {
	if (menuPtr->flags & UPDATE_PENDING) {
	    Tk_CancelIdleCall(UpdatePieMenu, (ClientData) menuPtr);
	}
	Tk_DoWhenIdle(DisplayPieMenu, (ClientData) menuPtr);
	menuPtr->flags |= REDRAW_PENDING;
    } else {
	Tk_DoWhenIdle(UpdatePieMenu, (ClientData) menuPtr);
	menuPtr->flags |= UPDATE_PENDING;
    }
}


static void
PopupPieMenu(clientData)
    ClientData clientData;	/* Information about widget. */
{
  register PieMenu *menuPtr = (PieMenu *) clientData;

  NeverPopupPieMenu(menuPtr);

  if (Tk_IsMapped(menuPtr->tkwin)) {
    return;
  }

  ShapePieMenu(menuPtr);
  Tk_MapWindow(menuPtr->tkwin);
}


static void
NowPopupPieMenu(menuPtr)
    register PieMenu *menuPtr;
{
  PopupPieMenu((ClientData)menuPtr);
}


static void
NeverPopupPieMenu(menuPtr)
    register PieMenu *menuPtr;
{
  if (menuPtr->flags & POPUP_PENDING) {
    Tk_DeleteTimerHandler(menuPtr->popup_timer_token);
    menuPtr->popup_timer_token = 0;
    menuPtr->flags &= ~POPUP_PENDING;
  }
}


static void
EventuallyPopupPieMenu(menuPtr)
    register PieMenu *menuPtr;
{
  NeverPopupPieMenu(menuPtr);

  if (Tk_IsMapped(menuPtr->tkwin)) {
    return;
  }

  menuPtr->popup_timer_token =
    Tk_CreateTimerHandler(menuPtr->popup_delay,
			  PopupPieMenu, (ClientData) menuPtr);
  menuPtr->flags |= POPUP_PENDING;
}


static void
DeferPopupPieMenu(menuPtr)
    register PieMenu *menuPtr;
{
  if (menuPtr->flags & POPUP_PENDING) {
    EventuallyPopupPieMenu(menuPtr);
  }
}



/*
 *--------------------------------------------------------------
 *
 * UnpostSubPieMenu --
 *
 *	This procedure unposts any submenu.
 *
 * Results:
 *	A standard Tcl return result.  Errors may occur in the
 *	Tcl commands generated to unpost submenus.
 *
 * Side effects:
 *	If there is already a submenu posted, it is unposted.
 *
 *--------------------------------------------------------------
 */

static int
UnpostSubPieMenu(interp, menuPtr)
    Tcl_Interp *interp;		/* Used for invoking sub-commands and
				 * reporting errors. */
    register PieMenu *menuPtr;	/* Information about menu as a whole. */
{
    char string[30];
    int result, x, y, win_x, win_y;
    unsigned int key_buttons;
    Window root, child;

    if (menuPtr->postedPie == NULL) {
	return TCL_OK;
    }

    result = Tcl_VarEval(interp, menuPtr->postedPie->name,
			 " unpost", (char *) NULL);
    menuPtr->postedPie = NULL;

    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * ActivatePieMenuEntry --
 *
 *	This procedure is invoked to make a particular pie menu 
 *	entry the active one, deactivating any other entry that 
 *	might currently be active.
 *
 * Results:
 *	The return value is a standard Tcl result (errors can occur
 *	while posting and unposting submenus).
 *
 * Side effects:
 *	Pie menu entries get redisplayed, and the active entry 
 *	changes.  Submenus may get posted and unposted.
 *
 *----------------------------------------------------------------------
 */

static int
ActivatePieMenuEntry(menuPtr, index, preview)
    register PieMenu *menuPtr;		/* Menu in which to activate. */
    int index;				/* Index of entry to activate, or
					 * -1 to deactivate all entries. */
    int preview;			/* 1 to execute previewer */
{
    register PieMenuEntry *mePtr;
    int result = TCL_OK;

    if (menuPtr->active >= 0) {
	mePtr = menuPtr->entries[menuPtr->active];

	EventuallyRedrawPieMenu(menuPtr, menuPtr->active);
    }
    menuPtr->active = index;
    if (index >= 0) {
	mePtr = menuPtr->entries[index];
	EventuallyRedrawPieMenu(menuPtr, index);
	if (preview) {
	  Tk_Preserve((ClientData) mePtr);
	  if (mePtr->preview != NULL) {
	      result = Tcl_GlobalEval(menuPtr->interp, mePtr->preview);
	  }
	  Tk_Release((ClientData) mePtr);
	}
    } else {
/* We're doing this in tcl these days, for finer control. */
#if 0
      if (preview && menuPtr->preview) {
	    result = Tcl_GlobalEval(menuPtr->interp, menuPtr->preview);
      }
#endif
    }
    return result;
}


/*
 * This pie menu tracking code determines the slice the cursor 
 * is in by representing slice edge angles as (quadrant, slope) 
 * pairs that can be quickly computed and compared. 
 *
 * The slope is defined such that it is greater than or equal to zero,
 * less than infinity, and increasing counter-clockwise around the menu. 
 * Each of the four quadrants encompasses one range of slope.
 *
 *                 Y
 *               ^
 *               |     x>0, y>=0
 *  x<=0, y>0 <--+       y/x
 *    -x/y       |        ^
 *        quad 1 | quad 0 |     X
 * -----+--------+--------+----> 
 *      | quad 2 | quad 3
 *      V        |      -x/y
 *   x<0, y<=0   +--> x>=0, y<0
 *     y/x       |
 *               |
 * 
 * The quadrants and slopes of the item edges are all precalculated,
 * during menu layout.
 * The quadrant and slope of the cursor must be calculated frequently
 * during menu tracking, so we just calculate the numerator and
 * denominator of the slope, and avoid an unnecessary division.
 * Instead of calculating "slope = numerator / denominator" then
 * testing "slope < it->slope", every time the cursor moves, we can
 * just test "numerator < (denominator * it->slope)".
 *
 * This algorithm works in a right-side-up coordinate space, but the final
 * results are tranformed into X-windows's up-side-down coordinate system 
 * by subtracting the y values from the window height. 
 */


#define CALC_QUADRANT_SLOPE(x, y, quadrant, numerator, denominator) \
    if ((y) > 0) (quadrant) = ((x) > 0 ? 0 : 1); \
    else if ((y) < 0) (quadrant) = ((x) < 0 ? 2 : 3); \
    else (quadrant) = ((x) > 0 ? 0 : 2); \
    if ((quadrant) & 1) { \
	(numerator) = ABS((x)); (denominator) = ABS((y)); \
    } else { \
	(numerator) = ABS((y)); (denominator) = ABS((x)); \
    }


int
CalcPieMenuItem(menu, x, y)
  PieMenu *menu;
  int x, y;
{
  register PieMenuEntry *it, *last_it;
  int i, j, order, quadrant;
  int numerator, denominator;
  int first, last_i, last_order;
  
  /*
   * Translate x and y from root window coordinates so they are 
   * relative to the menu center, in right side up coordinates.
   */
  
  menu->dx = x = (x - menu->root_x) + 1;
  menu->dy = y = (menu->root_y - y) - 1;
  
  /*
   * If there are no menu items,
   * or we are within the inactive region in the menu center,
   * then there is no item selected.
   */
  if ((menu->numEntries == 0) ||
      ((x * x) + (y * y) <
       (menu->inactive_radius * menu->inactive_radius))) {
    return(-1);
  }
  
  /*
   * If there's only one item, then that must be it. 
   */
  if (menu->numEntries == 1) {
    return(0);
  }
  
  /*
   * Calculate the quadrant, slope numerator, and slope denominator of
   * the cursor slope, to be used for comparisons.
   */
  CALC_QUADRANT_SLOPE(x, y, quadrant, numerator, denominator);
  
  /*
   * In most cases, during cursor tracking, the menu item that the
   * cursor is over will be the same as it was before (almost all
   * of the time), or one of the neighboring items (most of the
   * rest of the time). So we check those items first. But to keep
   * things simple, instead of actually checking the items in order of
   * frequency (the current, the two neighbors, then the rest), we just
   * start our loop around the menu items at the item *before* the
   * last selected menu item, so we still check the three most common
   * cases first (neighbor, current, neighbor, rest), without having 
   * to complicate the code with special cases. Another strategy, that
   * might be good for menus with ridiculously many items, would be
   * [to check the current item first, then the two neighbors, then]
   * to do a binary search of the menu items (since they are ordered).
   * But that's more complicated and you shouldn't have that many menu
   * items anyway.
   */
  
  /*
   * Start at the item before current one.
   */
  first = menu->active - 1;
  if (first < 0)
    first = menu->numEntries - 1;
  
  /*
   * Initialize last_order such that we will go through the loop
   * at least once, validating last_i, last_order, and last_it for
   * the next time through the loop.
   */
  last_i = last_order = -1;
  i = first;
  
  it = menu->entries[i];
  
  while (1) {

/* Legend: c = cursor, e = edge
   <cursor quad>,<edge quad>
         quad 1 | quad 0
         -------+-------
         quad 2 | quad 3
*/

    /* Set order = 1, if shortest direction from edge to cursor is ccw */
    switch ((quadrant - it->quadrant) & 3) {

case 0: /*
		 0,0	 1,1	 2,2	 3,3
		  |ce	ce|	  |	  |	
		--+--	--+--	--+--	--+--	
		  |	  |	ce|	  |ce
	*/
      /* slope >= it->slope */
      order = ((float)numerator >= (float)(denominator * it->slope));
      break;

case 1: /*
		 1,0	 2,1	 3,2	 0,3
		 c|e	 e|	  |	  |c
		--+--	--+--	--+--	--+--	
		  |	 c|	 e|c	  |e
	*/
      order = 1;
      break;

case 2: /*
		 2,0	 3,1	 0,2	 1,3
		  |e	 e|	  |c	 c|
		--+--	--+--	--+--	--+--	
		 c|	  |c	 e|	  |e
	*/
      /* slope < it->slope */
      order = ((float)numerator < (float)(denominator * it->slope));
      break;

case 3: /*
		 3,0	 0,1	 1,2	 2,3
		  |e	 e|c	 c|	  |
		--+--	--+--	--+--	--+--	
		  |c	  |	 e|	 c|e
	*/
      order = 0;
      break;
    }

    /*
     * If we were counter-clockwise of the last leading edge,
     * and we're clockwise of this leading edge,
     * then we were in the last menu item.
     * (Note: first time through this loop last_order = -1 so we'll
     * go back through the loop at least once, after validating
     * last_order, last_i, and last_it.)
     */
    if ((last_order == 1) && (order == 0)) {
      return(last_i);
    }
    last_order = order;

    /*
     * Remember this menu item index, and move on to the next one
     * counter-clockwise around the circle.
     */
    last_i = i; last_it = it;
    if (++i >= menu->numEntries) {
      i = 0;
    }
    it = menu->entries[i];

    /* 
     * If we've checked all the others, then that must have been it. 
     * This saves us from checking the leading edge of the first
     * item again (It's also insurance against layout bugs.)
     */
    if (i == first) {
      return(last_i);
    }
  }
}


LayoutPieMenu(menu)
  PieMenu *menu;
{
  int i;
  int total_slice, radius;
  int minx, miny, maxx, maxy;
  float angle;
  PieMenuEntry *it, *last;
  XFontStruct *font, *titlefont;

  /*
   * Calculate the sum of the menu item slice sizes.
   * Each menu item will get a (slice / total_slice) sized slice of the pie.
   */
  total_slice = 0;
  for (i = 0; i < menu->numEntries; i++) {
    total_slice += menu->entries[i]->slice;
  }

  if ((titlefont = menu->titlefontPtr) == NULL)
    titlefont = menu->fontPtr;

  /*
   * Calculate the subtend, angle, cosine, sine, quadrant, slope,
   * and size of each menu item.
   */
  angle = DEG_TO_RAD(menu->initial_angle);
  for (i = 0; i < menu->numEntries; i++) {
    register float edge_dx, edge_dy, numerator, denominator, twist;
    register int quadrant;

    it = menu->entries[i];
    if ((font = it->fontPtr) == NULL)
      font = menu->fontPtr;

    if (it->bitmap != None) {
      unsigned int bitmapWidth, bitmapHeight;

      Tk_SizeOfPixmap(it->bitmap, &bitmapWidth, &bitmapHeight);
      it->height = bitmapHeight;
      it->width = bitmapWidth;
    } else {
      it->height = font->ascent + font->descent;
      if (it->label != NULL) {
	(void) TkMeasureChars(font, it->label,
			      it->labelLength, 0, (int) 100000,
			      TK_NEWLINES_NOT_SPECIAL, &it->width);
      } else {
	it->width = 0;
      }
    }
    it->height += 2*menu->activeBorderWidth + 2;
    it->width += 2*menu->activeBorderWidth + 2;

    it->subtend = TWO_PI * it->slice / total_slice;
    twist = it->subtend / 2.0;
    if (i != 0) angle += twist;
    it->angle = angle;
    it->dx = cos(angle);
    it->dy = sin(angle);
    edge_dx = cos(angle - twist);
    edge_dy = sin(angle - twist);
    CALC_QUADRANT_SLOPE(edge_dx, edge_dy, quadrant, numerator, denominator);
    it->quadrant = quadrant;
    it->slope = (float)numerator / (float)denominator;
    angle += twist;
  }

  if ((radius = menu->fixed_radius) == 0) {
    radius = menu->min_radius;
    if (menu->numEntries > 1) {
      last = menu->entries[menu->numEntries - 1];
      for (i = 0; i < menu->numEntries; i++) {
	float dx, dy, ldx, ldy;
	int width, height, lwidth, lheight;

	it = menu->entries[i];

	dx = it->dx;  dy = it->dy;
	width = it->width;  height = it->height;
	ldx = last->dx;  ldy = last->dy;
	lwidth = last->width;  lheight = last->height;
	while (1) {
	  register int x, y, lx, ly, 
		       x0max, y0max, x1min, y1min;

	  x = dx * radius + it->x_offset;
	  y = dy * radius + it->y_offset;
	  lx = ldx * radius + last->x_offset;
	  ly = ldy * radius + last->y_offset;

	  /* Translate x y with respect to label size and position */
	  if (ABS(x) <= 2) {
	    x -= width/2;
	    if (y < 0)
	      y -= height;
	  } else {
	    if (x < 0)
	      x -= width;
	    y -= height/2;
	  }

	  if (ABS(lx) <= 2) {
	    lx -= lwidth/2;
	    if (ly < 0)
	      ly -= lheight;
	  } else {
	    if (lx < 0)
	      lx -= lwidth;
	    ly -= lheight/2;
	  }

	  /* Do rects (x y width height) and (lx ly lwidth lheight) overlap? */
	  x0max = x > lx ? x : lx;
	  y0max = y > ly ? y : ly;
	  x1min = x+width < lx+lwidth ? x+width : lx+lwidth;
	  y1min = y+height < ly+lheight ? y+height : ly+lheight;
	  if (!((x0max < x1min) &&
		(y0max < y1min))) { /* If they don't overlap */
	    /* They are far enough out, so move on. */
	    break;
	  }
	  /* Push the menu radius out a step and try again */
	  radius++;
	}
	/* Loop on to next menu item */
	last = it;
      }
    }
    radius += menu->extra_radius;
  }
  menu->label_radius = radius;

  /* Finally position all the menu labels at the same radius.
     Figure out the bounding box of the labels. */
  minx = miny = maxx = maxy = 0;
  for (i = 0; i < menu->numEntries; i++) {
    it = menu->entries[i];

    it->x = radius * it->dx + it->x_offset;
    it->y = radius * it->dy + it->y_offset;

    /* Translate x y with respect to label size and position */
    if (ABS(it->x) <= 2) {
      it->x -= it->width/2;
      if (it->y < 0)
	it->y -= it->height;
    } else {
      if (it->x < 0)
	it->x -= it->width;
      it->y -= it->height/2;
    }

    it->label_x = it->x + menu->activeBorderWidth + 1;
    it->label_y = it->y - menu->activeBorderWidth - 1;
    if (it->bitmap == None) {
      it->label_y -= (it->fontPtr ? it->fontPtr : menu->fontPtr)->ascent;
    }

    if (it->x < minx) minx = it->x;
    if ((it->x + it->width) > maxx) maxx = (it->x + it->width);
    if (it->y < miny) miny = it->y;
    if ((it->y + it->height) > maxy) maxy = (it->y + it->height);
  }

  
  if (menu->titleLength != 0) {
    menu->title_height = titlefont->ascent + titlefont->descent + 2;
    (void) TkMeasureChars(titlefont, menu->title,
			  menu->titleLength, 0, (int) 100000,
			  TK_NEWLINES_NOT_SPECIAL, &menu->title_width);
    menu->title_width += 2;
    if (-(menu->title_width / 2) < minx)
      minx = -(menu->title_width / 2);
    if ((menu->title_width / 2) > maxx)
      maxx = (menu->title_width / 2);
    maxy += (2 * menu->borderWidth) + menu->title_height;
  } else {
    menu->title_width = menu->title_height = 0;
  }


  minx -= 2*menu->borderWidth;  miny -= 2*menu->borderWidth;
  maxx += 2*menu->borderWidth;  maxy += 2*menu->borderWidth;

  menu->center_x = -minx;
  menu->center_y = maxy; /* y flip */
  menu->width = maxx - minx;
  menu->height = maxy - miny;

/*  menu->title_x = (menu->width - menu->title_width) / 2 + 1; */
  menu->title_x = menu->center_x - menu->title_width/2 + 1;
  menu->title_y = 2*menu->borderWidth + titlefont->ascent + 1;

  /* Translate the menu items to the center of the menu, in X coordinates. */
  for (i = 0; i < menu->numEntries; i++) {
    it = menu->entries[i];
    it->x = menu->center_x + it->x;
    it->y = (menu->center_y - it->y) - it->height; /* y flip */
    it->label_x = menu->center_x + it->label_x;
    it->label_y = (menu->center_y - it->label_y) - it->height; /* y flip */
  }

  if (menu->segments != NULL) {
      ckfree((char *)menu->segments);
  }
  menu->segments = (XSegment *)
    ckalloc(menu->numEntries * sizeof(XSegment));

  if (menu->numEntries > 1) {
    XSegment *seg = menu->segments;

    angle = DEG_TO_RAD(menu->initial_angle) -
            (menu->entries[0]->subtend / 2.0);
    for (i = 0; i < menu->numEntries; i++) {
      it = menu->entries[i];
      seg->x1 = menu->center_x + (cos(angle) * menu->inactive_radius);
      seg->y1 = menu->center_y - (sin(angle) * menu->inactive_radius);
      seg->x2 = menu->center_x +
	(cos(angle) * (menu->label_radius - PIE_SPOKE_INSET));
      seg->y2 = menu->center_y -
	(sin(angle) * (menu->label_radius - PIE_SPOKE_INSET));
      seg++;
      angle += it->subtend;
    }
  }
}


static void
ShapePieMenu(menuPtr)
PieMenu *menuPtr;
{
  Display *dpy;
  Window win, shape;
  GC gc;
  XGCValues values;
  PieMenuEntry *it;
  int i;

  if (HaveShape == 0)
    return;

  if (menuPtr->shaped == 0) {
    return;
  }

  dpy = Tk_Display(menuPtr->tkwin);

  if (HaveShape == -1) {
    int t1, t2;
    if (XShapeQueryExtension(dpy, &t1, &t2)) {
      HaveShape = 1;
    } else {
      HaveShape = 0;
      return;
    }
  }

  Tk_MakeWindowExist(menuPtr->tkwin);
  win = Tk_WindowId(menuPtr->tkwin);

  shape = XCreatePixmap(dpy, RootWindowOfScreen(Tk_Screen(menuPtr->tkwin)),
			menuPtr->width, menuPtr->height, 1);
  gc = XCreateGC(dpy, shape, 0, &values);


  XSetForeground(dpy, gc, 0);
  XFillRectangle(dpy, shape, gc, 0, 0, menuPtr->width, menuPtr->height);

  XSetForeground(dpy, gc, 1);
  if (menuPtr->titleLength != 0) {
    int bw = menuPtr->borderWidth;

    XFillRectangle(dpy, shape, gc, bw, bw, menuPtr->width - bw*2, menuPtr->title_height + bw*2);
  }

  for (i = 0; i < menuPtr->numEntries; i++) {
    it = menuPtr->entries[i];
    XFillRectangle(dpy, shape, gc, it->x, it->y, it->width, it->height);
  }

  XFreeGC(dpy, gc);
  XShapeCombineMask(dpy, win, ShapeBounding, 0, 0, shape, ShapeSet);
}
