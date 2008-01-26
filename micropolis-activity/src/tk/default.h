/*
 * default.h --
 *
 *	This file defines the defaults for all options for all of
 *	the Tk widgets.
 *
 * Copyright 1991-1992 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that this copyright
 * notice appears in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /user6/ouster/wish/RCS/default.h,v 1.31 92/08/21 11:45:19 ouster Exp $ SPRITE (Berkeley)
 */

#ifndef _DEFAULT
#define _DEFAULT

/*
 * The definitions below provide the absolute values for certain colors.
 * The values should be the same as in the color database, but hard-coding
 * them here allows Tk to run smoothly at sites that have an incomplete
 * or non-standard color database.
 */

#define BLACK		"Black"
#define WHITE		"White"
#define GRAY		"#b0b0b0"

#define BISQUE1		"#ffe4c4"
#define BISQUE2		"#eed5b7"
#define BISQUE3		"#cdb79e"

#define LIGHTBLUE2	"#b2dfee"

#define LIGHTPINK1	"#ffaeb9"

#define MAROON		"#b03060"

/*
 * Defaults for labels, buttons, checkbuttons, and radiobuttons:
 */

#define DEF_BUTTON_ANCHOR		"center"
#define DEF_BUTTON_ACTIVE_BG_COLOR	BISQUE2
#define DEF_BUTTON_ACTIVE_BG_MONO	BLACK
#define DEF_BUTTON_ACTIVE_FG_COLOR	BLACK
#define DEF_BUTTON_ACTIVE_FG_MONO	WHITE
#define DEF_BUTTON_BG_COLOR		BISQUE1
#define DEF_BUTTON_BG_MONO		WHITE
#define DEF_BUTTON_BITMAP		((char *) NULL)
#define DEF_BUTTON_BORDER_WIDTH		"2"
#define DEF_BUTTON_CURSOR		((char *) NULL)
#define DEF_BUTTON_COMMAND		((char *) NULL)
#define DEF_BUTTON_DISABLED_FG_COLOR	GRAY
#define DEF_BUTTON_DISABLED_FG_MONO	((char *) NULL)
#define DEF_BUTTON_FONT			"-Adobe-Helvetica-Bold-R-Normal-*-120-*"
#define DEF_BUTTON_FG			BLACK
#define DEF_BUTTON_HEIGHT		"0"
#define DEF_BUTTON_OFF_VALUE		"0"
#define DEF_BUTTON_ON_VALUE		"1"
#define DEF_BUTTON_PADX			"1"
#define DEF_BUTTON_PADY			"1"
#define DEF_BUTTON_RELIEF		"raised"
#define DEF_LABEL_RELIEF		"flat"
#define DEF_BUTTON_SELECTOR_COLOR	MAROON
#define DEF_BUTTON_SELECTOR_MONO	BLACK
#define DEF_BUTTON_STATE		"normal"
#define DEF_BUTTON_TEXT			" "
#define DEF_BUTTON_TEXT_VARIABLE	((char *) NULL)
#define DEF_BUTTON_VALUE		((char *) NULL)
#define DEF_BUTTON_WIDTH		"0"
#define DEF_RADIOBUTTON_VARIABLE	"selectedButton"
#define DEF_CHECKBUTTON_VARIABLE	((char *) NULL)

/*
 * Defaults for canvases:
 */

#define DEF_CANVAS_BG_COLOR		BISQUE1
#define DEF_CANVAS_BG_MONO		WHITE
#define DEF_CANVAS_BORDER_WIDTH		"2"
#define DEF_CANVAS_CLOSE_ENOUGH		"1"
#define DEF_CANVAS_CONFINE		"1"
#define DEF_CANVAS_CURSOR		((char *) NULL)
#define DEF_CANVAS_CURSOR_BG		BLACK
#define DEF_CANVAS_CURSOR_BD_COLOR	"0"
#define DEF_CANVAS_CURSOR_BD_MONO	"0"
#define DEF_CANVAS_CURSOR_OFF_TIME	"300"
#define DEF_CANVAS_CURSOR_ON_TIME	"600"
#define DEF_CANVAS_CURSOR_WIDTH		"2"
#define DEF_CANVAS_HEIGHT		"7c"
#define DEF_CANVAS_RELIEF		"flat"
#define DEF_CANVAS_SCROLL_INCREMENT	"10"
#define DEF_CANVAS_SCROLL_REGION	((char *) NULL)
#define DEF_CANVAS_SELECT_COLOR		LIGHTBLUE2
#define DEF_CANVAS_SELECT_MONO		BLACK
#define DEF_CANVAS_SELECT_BD_COLOR	"1"
#define DEF_CANVAS_SELECT_BD_MONO	"0"
#define DEF_CANVAS_SELECT_FG_COLOR	BLACK
#define DEF_CANVAS_SELECT_FG_MONO	WHITE
#define DEF_CANVAS_WIDTH		"10c"
#define DEF_CANVAS_X_SCROLL_CMD		((char *) NULL)
#define DEF_CANVAS_Y_SCROLL_CMD		((char *) NULL)

/*
 * Defaults for entries:
 */

#define DEF_ENTRY_BG_COLOR		BISQUE1
#define DEF_ENTRY_BG_MONO		WHITE
#define DEF_ENTRY_BORDER_WIDTH		"2"
#define DEF_ENTRY_CURSOR		"xterm"
#define DEF_ENTRY_CURSOR_BG		BLACK
#define DEF_ENTRY_CURSOR_BD_COLOR	"0"
#define DEF_ENTRY_CURSOR_BD_MONO	"0"
#define DEF_ENTRY_CURSOR_OFF_TIME	"300"
#define DEF_ENTRY_CURSOR_ON_TIME	"600"
#define DEF_ENTRY_CURSOR_WIDTH		"2"
#define DEF_ENTRY_EXPORT_SELECTION	"yes"
#define DEF_ENTRY_FONT			"-Adobe-Helvetica-Medium-R-Normal-*-120-*"
#define DEF_ENTRY_FG			BLACK
#define DEF_ENTRY_RELIEF		"flat"
#define DEF_ENTRY_SCROLL_COMMAND	((char *) NULL)
#define DEF_ENTRY_SELECT_COLOR		LIGHTBLUE2
#define DEF_ENTRY_SELECT_MONO		BLACK
#define DEF_ENTRY_SELECT_BD_COLOR	"1"
#define DEF_ENTRY_SELECT_BD_MONO	"0"
#define DEF_ENTRY_SELECT_FG_COLOR	BLACK
#define DEF_ENTRY_SELECT_FG_MONO	WHITE
#define DEF_ENTRY_STATE			"normal"
#define DEF_ENTRY_TEXT_VARIABLE		((char *) NULL)
#define DEF_ENTRY_WIDTH			"20"

/*
 * Defaults for frames:
 */

#define DEF_FRAME_BG_COLOR		BISQUE1
#define DEF_FRAME_BG_MONO		WHITE
#define DEF_FRAME_BORDER_WIDTH		"0"
#define DEF_FRAME_CURSOR		((char *) NULL)
#define DEF_FRAME_GEOMETRY		((char *) NULL)
#define DEF_FRAME_HEIGHT		"0"
#define DEF_FRAME_RELIEF		"flat"
#define DEF_FRAME_WIDTH			"0"

/*
 * Defaults for listboxes:
 */

#define DEF_LISTBOX_BG_COLOR		BISQUE1
#define DEF_LISTBOX_BG_MONO		WHITE
#define DEF_LISTBOX_BORDER_WIDTH	"2"
#define DEF_LISTBOX_CURSOR		((char *) NULL)
#define DEF_LISTBOX_EXPORT_SELECTION	"yes"
#define DEF_LISTBOX_FONT		"-Adobe-Helvetica-Bold-R-Normal-*-120-*"
#define DEF_LISTBOX_FG			BLACK
#define DEF_LISTBOX_GEOMETRY		"20x10"
#define DEF_LISTBOX_RELIEF		"flat"
#define DEF_LISTBOX_SCROLL_COMMAND	((char *) NULL)
#define DEF_LISTBOX_SELECT_COLOR	LIGHTBLUE2
#define DEF_LISTBOX_SELECT_MONO		BLACK
#define DEF_LISTBOX_SELECT_BD		"1"
#define DEF_LISTBOX_SELECT_FG_COLOR	BLACK
#define DEF_LISTBOX_SELECT_FG_MONO	WHITE

/*
 * Defaults for individual entries of menus:
 */

#define DEF_MENU_ENTRY_ACTIVE_BG	((char *) NULL)
#define DEF_MENU_ENTRY_ACCELERATOR	((char *) NULL)
#define DEF_MENU_ENTRY_BG		((char *) NULL)
#define DEF_MENU_ENTRY_BITMAP		((char *) NULL)
#define DEF_MENU_ENTRY_COMMAND		((char *) NULL)
#define DEF_MENU_ENTRY_FONT		((char *) NULL)
#define DEF_MENU_ENTRY_LABEL		""
#define DEF_MENU_ENTRY_MENU		((char *) NULL)
#define DEF_MENU_ENTRY_OFF_VALUE	"0"
#define DEF_MENU_ENTRY_ON_VALUE		"1"
#define DEF_MENU_ENTRY_VALUE		((char *) NULL)
#define DEF_MENU_ENTRY_CHECK_VARIABLE	((char *) NULL)
#define DEF_MENU_ENTRY_RADIO_VARIABLE	"selectedButton"
#define DEF_MENU_ENTRY_STATE		"normal"
#define DEF_MENU_ENTRY_UNDERLINE	"-1"

/*
 * Defaults for menus overall:
 */

#define DEF_MENU_ACTIVE_BG_COLOR	BISQUE2
#define DEF_MENU_ACTIVE_BG_MONO		BLACK
#define DEF_MENU_ACTIVE_BORDER_WIDTH	"1"
#define DEF_MENU_ACTIVE_FG_COLOR	BLACK
#define DEF_MENU_ACTIVE_FG_MONO		WHITE
#define DEF_MENU_BG_COLOR		BISQUE1
#define DEF_MENU_BG_MONO		WHITE
#define DEF_MENU_BORDER_WIDTH		"2"
#define DEF_MENU_CURSOR			"arrow"
#define DEF_MENU_DISABLED_FG_COLOR	GRAY
#define DEF_MENU_DISABLED_FG_MONO	((char *) NULL)
#define DEF_MENU_FONT			"-Adobe-Helvetica-Bold-R-Normal-*-120-*"
#define DEF_MENU_FG			BLACK
#define DEF_MENU_SELECTOR_COLOR		MAROON
#define DEF_MENU_SELECTOR_MONO		BLACK

/*
 * Defaults for menubuttons:
 */

#define DEF_MENUBUTTON_ANCHOR		"center"
#define DEF_MENUBUTTON_ACTIVE_BG_COLOR	BISQUE2
#define DEF_MENUBUTTON_ACTIVE_BG_MONO	BLACK
#define DEF_MENUBUTTON_ACTIVE_FG_COLOR	BLACK
#define DEF_MENUBUTTON_ACTIVE_FG_MONO	WHITE
#define DEF_MENUBUTTON_BG_COLOR		BISQUE1
#define DEF_MENUBUTTON_BG_MONO		WHITE
#define DEF_MENUBUTTON_BITMAP		((char *) NULL)
#define DEF_MENUBUTTON_BORDER_WIDTH	"2"
#define DEF_MENUBUTTON_CURSOR		((char *) NULL)
#define DEF_MENUBUTTON_DISABLED_FG_COLOR GRAY
#define DEF_MENUBUTTON_DISABLED_FG_MONO	((char *) NULL)
#define DEF_MENUBUTTON_FONT		"-Adobe-Helvetica-Bold-R-Normal-*-120-*"
#define DEF_MENUBUTTON_FG		BLACK
#define DEF_MENUBUTTON_HEIGHT		"0"
#define DEF_MENUBUTTON_MENU		((char *) NULL)
#define DEF_MENUBUTTON_PADX		"2"
#define DEF_MENUBUTTON_PADY		"2"
#define DEF_MENUBUTTON_RELIEF		"flat"
#define DEF_MENUBUTTON_STATE		"normal"
#define DEF_MENUBUTTON_TEXT		" "
#define DEF_MENUBUTTON_TEXT_VARIABLE	((char *) NULL)
#define DEF_MENUBUTTON_VARIABLE		"postedMenu"
#define DEF_MENUBUTTON_UNDERLINE	"-1"
#define DEF_MENUBUTTON_WIDTH		"0"

/*
 * Defaults for messages:
 */

#define DEF_MESSAGE_ANCHOR		"center"
#define DEF_MESSAGE_ASPECT		"150"
#define DEF_MESSAGE_BG_COLOR		BISQUE1
#define DEF_MESSAGE_BG_MONO		WHITE
#define DEF_MESSAGE_BORDER_WIDTH	"2"
#define DEF_MESSAGE_CURSOR		((char *) NULL)
#define DEF_MESSAGE_FONT		"-Adobe-Helvetica-Bold-R-Normal-*-120-*"
#define DEF_MESSAGE_FG			BLACK
#define DEF_MESSAGE_JUSTIFY		((char *) NULL)
#define DEF_MESSAGE_PADX		"-1"
#define DEF_MESSAGE_PADY		"-1"
#define DEF_MESSAGE_RELIEF		"flat"
#define DEF_MESSAGE_TEXT		" "
#define DEF_MESSAGE_TEXT_VARIABLE	((char *) NULL)
#define DEF_MESSAGE_WIDTH		"0"

/*
 * Defaults for scales:
 */

#define DEF_SCALE_ACTIVE_FG_COLOR	LIGHTPINK1
#define DEF_SCALE_ACTIVE_FG_MONO	WHITE
#define DEF_SCALE_BG_COLOR		BISQUE2
#define DEF_SCALE_BG_MONO		WHITE
#define DEF_SCALE_BORDER_WIDTH		"2"
#define DEF_SCALE_CURSOR		((char *) NULL)
#define DEF_SCALE_FONT			"-Adobe-Helvetica-Bold-R-Normal-*-120-*"
#define DEF_SCALE_FG_COLOR		BLACK
#define DEF_SCALE_FG_MONO		BLACK
#define DEF_SCALE_FROM			"0"
#define DEF_SCALE_LABEL			((char *) NULL)
#define DEF_SCALE_LENGTH		"100"
#define DEF_SCALE_ORIENT		"vertical"
#define DEF_SCALE_RELIEF		"flat"
#define DEF_SCALE_SHOW_VALUE		"1"
#define DEF_SCALE_SLIDER_FG_COLOR	BISQUE3
#define DEF_SCALE_SLIDER_FG_MONO	WHITE
#define DEF_SCALE_SLIDER_LENGTH		"30"
#define DEF_SCALE_STATE			"normal"
#define DEF_SCALE_TICK_INTERVAL		"0"
#define DEF_SCALE_TO			"100"
#define DEF_SCALE_WIDTH			"15"

/*
 * Defaults for scrollbars:
 */

#define DEF_SCROLLBAR_ACTIVE_FG_COLOR	LIGHTPINK1
#define DEF_SCROLLBAR_ACTIVE_FG_MONO	BLACK
#define DEF_SCROLLBAR_BG_COLOR		BISQUE3
#define DEF_SCROLLBAR_BG_MONO		WHITE
#define DEF_SCROLLBAR_BORDER_WIDTH	"2"
#define DEF_SCROLLBAR_COMMAND		((char *) NULL)
#define DEF_SCROLLBAR_CURSOR		((char *) NULL)
#define DEF_SCROLLBAR_FG_COLOR		BISQUE1
#define DEF_SCROLLBAR_FG_MONO		WHITE
#define DEF_SCROLLBAR_ORIENT		"vertical"
#define DEF_SCROLLBAR_RELIEF		"flat"
#define DEF_SCROLLBAR_REPEAT_DELAY	"300"
#define DEF_SCROLLBAR_REPEAT_INTERVAL	"100"
#define DEF_SCROLLBAR_WIDTH		"15"

/*
 * Defaults for texts:
 */

#define DEF_TEXT_BG_COLOR		BISQUE1
#define DEF_TEXT_BG_MONO		WHITE
#define DEF_TEXT_BORDER_WIDTH		"0"
#define DEF_TEXT_CURSOR			"xterm"
#define DEF_TEXT_FG			BLACK
#define DEF_TEXT_EXPORT_SELECTION	"yes"
#define DEF_TEXT_FONT			"*-Courier-Medium-R-Normal-*-120-*"
#define DEF_TEXT_FOREGROUND		BLACK
#define DEF_TEXT_HEIGHT			"24"
#define DEF_TEXT_INSERT_BG		BLACK
#define DEF_TEXT_INSERT_BD_COLOR	"0"
#define DEF_TEXT_INSERT_BD_MONO		"0"
#define DEF_TEXT_INSERT_OFF_TIME	"300"
#define DEF_TEXT_INSERT_ON_TIME		"600"
#define DEF_TEXT_INSERT_WIDTH		"2"
#define DEF_TEXT_PADX			"1"
#define DEF_TEXT_PADY			"1"
#define DEF_TEXT_RELIEF			"flat"
#define DEF_TEXT_SELECT_COLOR		LIGHTBLUE2
#define DEF_TEXT_SELECT_MONO		BLACK
#define DEF_TEXT_SELECT_BD_COLOR	"1"
#define DEF_TEXT_SELECT_BD_MONO		"0"
#define DEF_TEXT_SELECT_FG_COLOR	BLACK
#define DEF_TEXT_SELECT_FG_MONO		WHITE
#define DEF_TEXT_SET_GRID		"false"
#define DEF_TEXT_STATE			"normal"
#define DEF_TEXT_WIDTH			"80"
#define DEF_TEXT_WRAP			"char"
#define DEF_TEXT_YSCROLL_COMMAND	((char *) NULL)

#endif /* _DEFAULT */
