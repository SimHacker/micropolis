////////////////////////////////////////////////////////////////////////
// linuxcompat.h

#ifndef _LINUXCOMPAT_H_
#define _LINUXCOMPAT_H_ 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _WIN32

struct RECT {
  int left;
  int right;
  int top;
  int bottom;
};

typedef const char *LPCTSTR;
typedef long LONG;
#define stricmp(a, b) strcasecmp((a), (b))

#else

#pragma warning(disable: 4244)
#pragma warning(disable: 4312)

#define stricmp(a, b) _stricmp((a), (b))

#endif

#endif // _LINUXCOMPAT_H_
