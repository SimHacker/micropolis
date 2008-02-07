/#include "tkint.h"/a\
#define TK_EXTENDED\
#ifdef TK_EXTENDED\
#    include "tclxtend.h"\
     Tcl_Interp *tk_mainInterp;  /* Need to process signals */\
#endif

/^char initCmd/c\
#ifdef TK_EXTENDED\
char initCmd[] = "load wishx.tcl";\
#else\
char initCmd[] = "source $tk_library/wish.tcl";\
#endif

/    interp = Tcl_CreateInterp();/c\
#ifdef TK_EXTENDED\
    tk_mainInterp = interp = Tcl_CreateExtendedInterp();\
#else\
    interp = Tcl_CreateInterp();\
#endif

/result = Tcl_Eval(interp, initCmd/i\
#ifdef TK_EXTENDED\
    tclAppName     = "Wish";\
    tclAppLongname = "Wish - Tk Shell";\
    tclAppVersion  = TK_VERSION;\
    Tcl_ShellEnvInit (interp, TCLSH_ABORT_STARTUP_ERR,\
                      name,\
                      0, NULL,           /* argv var already set  */\
                      fileName == NULL,  /* interactive?          */\
                      NULL);             /* Standard default file */\
#endif
