/* these are just in a sereate file to make it easier to merge into 
 * the stock tkWm.c */

void TkWmSetWmProtocols _ANSI_ARGS_((
    TkWindow *winPtr
));
void TkWmProtocolEventProc _ANSI_ARGS_((
    TkWindow *winPtr,
    XEvent *eventPtr
));
int WmProtocolCmd _ANSI_ARGS_((
    Tcl_Interp *interp,
    char **CmdPtr,
    int argc,
    char **argv
));
