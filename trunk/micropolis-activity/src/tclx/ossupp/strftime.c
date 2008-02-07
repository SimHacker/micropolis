/*
 * strftime.c
 *
 * Public-domain relatively quick-and-dirty implemenation of
 * ANSI library routine for System V Unix systems.
 *
 * It's written in old-style C for maximal portability.
 *
 * The code for %c, %x, and %X is my best guess as to what's "appropriate".
 * This version ignores LOCALE information.
 * It also doesn't worry about multi-byte characters.
 * So there.
 *
 * Arnold Robbins
 * January, February, 1991
 *
 * Fixes from ado@elsie.nci.nih.gov
 * February 1991
 *-----------------------------------------------------------------------------
 * $Id: strftime.c,v 2.0 1992/10/16 04:52:16 markd Rel $
 *-----------------------------------------------------------------------------
 */

/*
 * To avoid Unix version problems, this code has been simplified to avoid
 * const and size_t, however this can cause an incompatible definition on
 * ansi-C systems, so a game is played with defines to ignore a strftime
 * declaration in time.h
 */

#define strftime ___srtftime

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

#undef strftime

extern char *strchr();
static int weeknumber();

#ifndef TCL_HAS_TM_ZONE
extern char *tzname[2];
extern int daylight;
#endif

/* strftime --- produce formatted time */

int
strftime(s, maxsize, format, timeptr)
    char            *s;
    int              maxsize;
    char            *format;
    struct tm       *timeptr;
{
	char *endp = s + maxsize;
	char *start = s;
	char tbuf[100];
	int i;

	/* various tables, useful in North America */
	static char *days_a[] = {
		"Sun", "Mon", "Tue", "Wed",
		"Thu", "Fri", "Sat",
	};
	static char *days_l[] = {
		"Sunday", "Monday", "Tuesday", "Wednesday",
		"Thursday", "Friday", "Saturday",
	};
	static char *months_a[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
	};
	static char *months_l[] = {
		"January", "February", "March", "April",
		"May", "June", "July", "August", "September",
		"October", "November", "December",
	};
	static char *ampm[] = { "AM", "PM", };

	if (s == NULL || format == NULL || timeptr == NULL || maxsize == 0)
		return 0;

	if (strchr(format, '%') == NULL && strlen(format) + 1 >= maxsize)
		return 0;

	for (; *format && s < endp - 1; format++) {
		tbuf[0] = '\0';
		if (*format != '%') {
			*s++ = *format;
			continue;
		}
		switch (*++format) {
		case '\0':
			*s++ = '%';
			goto out;

		case '%':
			*s++ = '%';
			continue;

		case 'a':	/* abbreviated weekday name */
			if (timeptr->tm_wday < 0 || timeptr->tm_wday > 6)
				strcpy(tbuf, "?");
			else
				strcpy(tbuf, days_a[timeptr->tm_wday]);
			break;

		case 'A':	/* full weekday name */
			if (timeptr->tm_wday < 0 || timeptr->tm_wday > 6)
				strcpy(tbuf, "?");
			else
				strcpy(tbuf, days_l[timeptr->tm_wday]);
			break;

		case 'h':	/* abbreviated month name */
		case 'b':	/* abbreviated month name */
			if (timeptr->tm_mon < 0 || timeptr->tm_mon > 11)
				strcpy(tbuf, "?");
			else
				strcpy(tbuf, months_a[timeptr->tm_mon]);
			break;

		case 'B':	/* full month name */
			if (timeptr->tm_mon < 0 || timeptr->tm_mon > 11)
				strcpy(tbuf, "?");
			else
				strcpy(tbuf, months_l[timeptr->tm_mon]);
			break;

		case 'c':	/* appropriate date and time representation */
			sprintf(tbuf, "%s %s %2d %02d:%02d:%02d %d",
				days_a[timeptr->tm_wday],
				months_a[timeptr->tm_mon],
				timeptr->tm_mday,
				timeptr->tm_hour,
				timeptr->tm_min,
				timeptr->tm_sec,
				timeptr->tm_year + 1900);
			break;

		case 'd':	/* day of the month, 01 - 31 */
			sprintf(tbuf, "%02d", timeptr->tm_mday);
			break;

		case 'H':	/* hour, 24-hour clock, 00 - 23 */
			sprintf(tbuf, "%02d", timeptr->tm_hour);
			break;

		case 'I':	/* hour, 12-hour clock, 01 - 12 */
			i = timeptr->tm_hour;
			if (i == 0)
				i = 12;
			else if (i > 12)
				i -= 12;
			sprintf(tbuf, "%02d", i);
			break;

		case 'j':	/* day of the year, 001 - 366 */
			sprintf(tbuf, "%03d", timeptr->tm_yday + 1);
			break;

		case 'm':	/* month, 01 - 12 */
			sprintf(tbuf, "%02d", timeptr->tm_mon + 1);
			break;

		case 'M':	/* minute, 00 - 59 */
			sprintf(tbuf, "%02d", timeptr->tm_min);
			break;

		case 'p':	/* am or pm based on 12-hour clock */
			if (timeptr->tm_hour < 12)
				strcpy(tbuf, ampm[0]);
			else
				strcpy(tbuf, ampm[1]);
			break;

		case 'S':	/* second, 00 - 61 */
			sprintf(tbuf, "%02d", timeptr->tm_sec);
			break;

		case 'U':	/* week of year, Sunday is first day of week */
			sprintf(tbuf, "%d", weeknumber(timeptr, 0));
			break;

		case 'w':	/* weekday, Sunday == 0, 0 - 6 */
			sprintf(tbuf, "%d", timeptr->tm_wday);
			break;

		case 'W':	/* week of year, Monday is first day of week */
			sprintf(tbuf, "%d", weeknumber(timeptr, 1));
			break;

		case 'x':	/* appropriate date representation */
			sprintf(tbuf, "%s %s %2d %d",
				days_a[timeptr->tm_wday],
				months_a[timeptr->tm_mon],
				timeptr->tm_mday,
				timeptr->tm_year + 1900);
			break;

		case 'X':	/* appropriate time representation */
			sprintf(tbuf, "%02d:%02d:%02d",
				timeptr->tm_hour,
				timeptr->tm_min,
				timeptr->tm_sec);
			break;

		case 'y':	/* year without a century, 00 - 99 */
			i = timeptr->tm_year % 100;
			sprintf(tbuf, "%d", i);
			break;

		case 'Y':	/* year with century */
			sprintf(tbuf, "%d", 1900 + timeptr->tm_year);
			break;

		case 'Z':	/* time zone name or abbrevation */
#ifdef TCL_HAS_TM_ZONE
                        strcpy(tbuf, timeptr->tm_zone);
#else
			i = 0;
			if (daylight && timeptr->tm_isdst)
				i = 1;
			strcpy(tbuf, tzname[i]);
#endif
			break;

		case 'n':	/* same as \n */
			tbuf[0] = '\n';
			tbuf[1] = '\0';
			break;

		case 't':	/* same as \t */
			tbuf[0] = '\t';
			tbuf[1] = '\0';
			break;

		case 'D':	/* date as %m/%d/%y */
			strftime(tbuf, sizeof tbuf, "%m/%d/%y", timeptr);
			break;

		case 'e':	/* day of month, blank padded */
			sprintf(tbuf, "%2d", timeptr->tm_mday);
			break;

		case 'r':	/* time as %I:%M:%S %p */
			strftime(tbuf, sizeof tbuf, "%I:%M:%S %p", timeptr);
			break;

		case 'R':	/* time as %H:%M */
			strftime(tbuf, sizeof tbuf, "%H:%M", timeptr);
			break;

		case 'T':	/* time as %H:%M:%S */
			strftime(tbuf, sizeof tbuf, "%H:%M:%S", timeptr);
			break;

		default:
			tbuf[0] = '%';
			tbuf[1] = *format;
			tbuf[2] = '\0';
			break;
		}
		i = strlen(tbuf);
		if (i)
			if (s + i < endp - 1) {
				strcpy(s, tbuf);
				s += i;
			} else
				return 0;
	}
out:
	if (s < endp && *format == '\0') {
		*s = '\0';
		return (s - start);
	} else
		return 0;
}

/* weeknumber --- figure how many weeks into the year */

/* With thanks and tip of the hatlo to ado@elsie.nci.nih.gov */

static int
weeknumber(timeptr, firstweekday)
    struct tm *timeptr;
    int        firstweekday;
{
	if (firstweekday == 0)
		return (timeptr->tm_yday + 7 - timeptr->tm_wday) / 7;
	else
		return (timeptr->tm_yday + 7 -
			(timeptr->tm_wday ? (timeptr->tm_wday - 1) : 6)) / 7;
}
