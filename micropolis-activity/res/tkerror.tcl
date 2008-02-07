# This file contains a default version of the tkError procedure.  It
# just prints out a stack trace.

proc tkerror err {
    global errorInfo
    puts stdout "$errorInfo"
}
