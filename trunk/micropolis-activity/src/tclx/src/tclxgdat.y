/* 
 * tclXgetdate.y --
 *
 * Contains yacc grammer for parsing date and time strings..
 *---------------------------------------------------------------------------
 * Copyright 1992 Karl Lehenbauer and Mark Diekhans.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies.  Karl Lehenbauer and
 * Mark Diekhans make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *---------------------------------------------------------------------------
 * This code is a slightly modified version of getdate.y by:
 *     Steven M. Bellovin (unc!smb)
 *     Dept. of Computer Science
 *     University of North Carolina at Chapel Hill
 *     getdate.y    2.13    9/16/86
 *-----------------------------------------------------------------------------
 * $Id: tclXgetdate.y,v 2.0 1992/10/16 04:51:34 markd Rel $
 *-----------------------------------------------------------------------------
 */

%token ID MONTH DAY MERIDIAN NUMBER UNIT MUNIT SUNIT ZONE DAYZONE AGO
%{
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>

#ifndef NULL
#    define NULL    0
#endif

#define daysec (24L*60L*60L)

        static int timeflag, zoneflag, dateflag, dayflag, relflag;
        static time_t relsec, relmonth;
        static int hh, mm, ss, merid, dayLight;
        static int dayord, dayreq;
        static int month, day, year;
        static int ourzone;

#if 0
	static time_t timeconv(int hh, int mm, int ss, int mer);
	static time_t daylcorr(time_t future, time_t now);
	static lookup(char *id);
#else
	static time_t timeconv();
	static time_t daylcorr();
	static lookup();
#endif

#define AM 1
#define PM 2
#define DAYLIGHT 1
#define STANDARD 2
#define MAYBE    3
%}

%%
timedate:               /* empty */
        | timedate item;

item:   tspec =
                {timeflag++;}
        | zone =
                {zoneflag++;}
        | dtspec =
                {dateflag++;}
        | dyspec =
                {dayflag++;}
        | rspec =
                {relflag++;}
        | nspec;

nspec:  NUMBER =
                {if (timeflag && dateflag && !relflag) year = $1;
                else {timeflag++;hh = $1/100;mm = $1%100;ss = 0;merid = 24;}};

tspec:  NUMBER MERIDIAN =
                {hh = $1; mm = 0; ss = 0; merid = $2;}
        | NUMBER ':' NUMBER =
                {hh = $1; mm = $3; merid = 24;}
        | NUMBER ':' NUMBER MERIDIAN =
                {hh = $1; mm = $3; merid = $4;}
        | NUMBER ':' NUMBER NUMBER =
                {hh = $1; mm = $3; merid = 24;
                dayLight = STANDARD; ourzone = -($4%100 + 60*$4/100);}
        | NUMBER ':' NUMBER ':' NUMBER =
                {hh = $1; mm = $3; ss = $5; merid = 24;}
        | NUMBER ':' NUMBER ':' NUMBER MERIDIAN =
                {hh = $1; mm = $3; ss = $5; merid = $6;}
        | NUMBER ':' NUMBER ':' NUMBER NUMBER =
                {hh = $1; mm = $3; ss = $5; merid = 24;
                dayLight = STANDARD; ourzone = -($6%100 + 60*$6/100);};

zone:   ZONE =
                {ourzone = $1; dayLight = STANDARD;}
        | DAYZONE =
                {ourzone = $1; dayLight = DAYLIGHT;};

dyspec: DAY =
                {dayord = 1; dayreq = $1;}
        | DAY ',' =
                {dayord = 1; dayreq = $1;}
        | NUMBER DAY =
                {dayord = $1; dayreq = $2;};

dtspec: NUMBER '/' NUMBER =
                {month = $1; day = $3;}
        | NUMBER '/' NUMBER '/' NUMBER =
                {month = $1; day = $3; year = $5;}
        | MONTH NUMBER =
                {month = $1; day = $2;}
        | MONTH NUMBER ',' NUMBER =
                {month = $1; day = $2; year = $4;}
        | NUMBER MONTH =
                {month = $2; day = $1;}
        | NUMBER MONTH NUMBER =
                {month = $2; day = $1; year = $3;};


rspec:  NUMBER UNIT =
                {relsec +=  60L * $1 * $2;}
        | NUMBER MUNIT =
                {relmonth += $1 * $2;}
        | NUMBER SUNIT =
                {relsec += $1;}
        | UNIT =
                {relsec +=  60L * $1;}
        | MUNIT =
                {relmonth += $1;}
        | SUNIT =
                {relsec++;}
        | rspec AGO =
                {relsec = -relsec; relmonth = -relmonth;};
%%

static int mdays[12] =
        {31, 0, 31,  30, 31, 30,  31, 31, 30,  31, 30, 31};
#define epoch 1970

extern struct tm *localtime();

static
time_t dateconv(mm, dd, yy, h, m, s, mer, zone, dayflag)
int mm, dd, yy, h, m, s, mer, zone, dayflag;
{
        time_t tod, jdate;
        register int i;
        time_t timeconv();

        if (yy < 0) yy = -yy;
        if (yy < 100) yy += 1900;
        mdays[1] = 28 + (yy%4 == 0 && (yy%100 != 0 || yy%400 == 0));
        if (yy < epoch || yy > 1999 || mm < 1 || mm > 12 ||
                dd < 1 || dd > mdays[--mm]) return (-1);
        jdate = dd-1;
        for (i=0; i<mm; i++) jdate += mdays[i];
        for (i = epoch; i < yy; i++) jdate += 365 + (i%4 == 0);
        jdate *= daysec;
        jdate += zone * 60L;
        if ((tod = timeconv(h, m, s, mer)) < 0) return (-1);
        jdate += tod;
        if (dayflag==DAYLIGHT || (dayflag==MAYBE&&localtime(&jdate)->tm_isdst))
                jdate += -1*60*60;
        return (jdate);
}

static
time_t dayconv(ord, day, now) int ord, day; time_t now;
{
        register struct tm *loctime;
        time_t tod;
        time_t daylcorr();

        tod = now;
        loctime = localtime(&tod);
        tod += daysec * ((day - loctime->tm_wday + 7) % 7);
        tod += 7*daysec*(ord<=0?ord:ord-1);
        return daylcorr(tod, now);
}

static
time_t timeconv(hh, mm, ss, mer) register int hh, mm, ss, mer;
{
        if (mm < 0 || mm > 59 || ss < 0 || ss > 59) return (-1);
        switch (mer) {
                case AM: if (hh < 1 || hh > 12) return(-1);
                         return (60L * ((hh%12)*60L + mm)+ss);
                case PM: if (hh < 1 || hh > 12) return(-1);
                         return (60L * ((hh%12 +12)*60L + mm)+ss);
                case 24: if (hh < 0 || hh > 23) return (-1);
                         return (60L * (hh*60L + mm)+ss);
                default: return (-1);
        }
}

static
time_t monthadd(sdate, relmonth) time_t sdate, relmonth;
{
        struct tm *ltime;
        time_t dateconv();
        time_t daylcorr();
        int mm, yy;

        if (relmonth == 0) return 0;
        ltime = localtime(&sdate);
        mm = 12*ltime->tm_year + ltime->tm_mon + relmonth;
        yy = mm/12;
        mm = mm%12 + 1;
        return daylcorr(dateconv(mm, ltime->tm_mday, yy, ltime->tm_hour,
                ltime->tm_min, ltime->tm_sec, 24, ourzone, MAYBE), sdate);
}

static
time_t daylcorr(future, now) time_t future, now;
{
        int fdayl, nowdayl;

        nowdayl = (localtime(&now)->tm_hour+1) % 24;
        fdayl = (localtime(&future)->tm_hour+1) % 24;
        return (future-now) + 60L*60L*(nowdayl-fdayl);
}

static char *lptr;

static
yylex()
{
#ifndef YYSTYPE
//#define YYSTYPE extern int
#define YYSTYPE int
#endif
        YYSTYPE yylval;
        int sign;
        register char c;
        register char *p;
        char idbuf[20];
        int pcnt;

        for (;;) {
                while (isspace(*lptr)) lptr++;

                if (isdigit(c = *lptr) || c == '-' || c == '+') {
                        if (c== '-' || c == '+') {
                                if (c=='-') sign = -1;
                                else sign = 1;
                                if (!isdigit(*++lptr)) {
                                        /* yylval = sign; return (NUMBER); */
                                        return yylex(); /* skip the '-' sign */
                                }
                        } else sign = 1;
                        yylval = 0;
                        while (isdigit(c = *lptr++)) yylval = 10*yylval + c - '0';
                        yylval *= sign;
                        lptr--;
                        return (NUMBER);

                } else if (isalpha(c)) {
                        p = idbuf;
                        while (isalpha(c = *lptr++) || c=='.')
                                if (p < &idbuf[sizeof(idbuf)-1])
                                        *p++ = c;
                        *p = '\0';
                        lptr--;
                        return (lookup(idbuf));
                }

                else if (c == '(') {
                        pcnt = 0;
                        do {
                                c = *lptr++;
                                if (c == '\0') return(c);
                                else if (c == '(') pcnt++;
                                else if (c == ')') pcnt--;
                        } while (pcnt > 0);
                }

                else return (*lptr++);
        }
}

//static
struct table {
        char *name;
        int type, value;
};

static
struct table mdtab[] = {
        {"January", MONTH, 1},
        {"February", MONTH, 2},
        {"March", MONTH, 3},
        {"April", MONTH, 4},
        {"May", MONTH, 5},
        {"June", MONTH, 6},
        {"July", MONTH, 7},
        {"August", MONTH, 8},
        {"September", MONTH, 9},
        {"Sept", MONTH, 9},
        {"October", MONTH, 10},
        {"November", MONTH, 11},
        {"December", MONTH, 12},

        {"Sunday", DAY, 0},
        {"Monday", DAY, 1},
        {"Tuesday", DAY, 2},
        {"Tues", DAY, 2},
        {"Wednesday", DAY, 3},
        {"Wednes", DAY, 3},
        {"Thursday", DAY, 4},
        {"Thur", DAY, 4},
        {"Thurs", DAY, 4},
        {"Friday", DAY, 5},
        {"Saturday", DAY, 6},
        {0, 0, 0}};

#define HRS *60
#define HALFHR 30
static
struct table mztab[] = {
        {"a.m.", MERIDIAN, AM},
        {"am", MERIDIAN, AM},
        {"p.m.", MERIDIAN, PM},
        {"pm", MERIDIAN, PM},
        {"nst", ZONE, 3 HRS + HALFHR},          /* Newfoundland */
        {"n.s.t.", ZONE, 3 HRS + HALFHR},
        {"ast", ZONE, 4 HRS},           /* Atlantic */
        {"a.s.t.", ZONE, 4 HRS},
        {"adt", DAYZONE, 4 HRS},
        {"a.d.t.", DAYZONE, 4 HRS},
        {"est", ZONE, 5 HRS},           /* Eastern */
        {"e.s.t.", ZONE, 5 HRS},
        {"edt", DAYZONE, 5 HRS},
        {"e.d.t.", DAYZONE, 5 HRS},
        {"cst", ZONE, 6 HRS},           /* Central */
        {"c.s.t.", ZONE, 6 HRS},
        {"cdt", DAYZONE, 6 HRS},
        {"c.d.t.", DAYZONE, 6 HRS},
        {"mst", ZONE, 7 HRS},           /* Mountain */
        {"m.s.t.", ZONE, 7 HRS},
        {"mdt", DAYZONE, 7 HRS},
        {"m.d.t.", DAYZONE, 7 HRS},
        {"pst", ZONE, 8 HRS},           /* Pacific */
        {"p.s.t.", ZONE, 8 HRS},
        {"pdt", DAYZONE, 8 HRS},
        {"p.d.t.", DAYZONE, 8 HRS},
        {"yst", ZONE, 9 HRS},           /* Yukon */
        {"y.s.t.", ZONE, 9 HRS},
        {"ydt", DAYZONE, 9 HRS},
        {"y.d.t.", DAYZONE, 9 HRS},
        {"hst", ZONE, 10 HRS},          /* Hawaii */
        {"h.s.t.", ZONE, 10 HRS},
        {"hdt", DAYZONE, 10 HRS},
        {"h.d.t.", DAYZONE, 10 HRS},

        {"gmt", ZONE, 0 HRS},
        {"g.m.t.", ZONE, 0 HRS},
        {"bst", DAYZONE, 0 HRS},                /* British Summer Time */
        {"b.s.t.", DAYZONE, 0 HRS},
        {"eet", ZONE, 0 HRS},           /* European Eastern Time */
        {"e.e.t.", ZONE, 0 HRS},
        {"eest", DAYZONE, 0 HRS},       /* European Eastern Summer Time */
        {"e.e.s.t.", DAYZONE, 0 HRS},
        {"met", ZONE, -1 HRS},          /* Middle European Time */
        {"m.e.t.", ZONE, -1 HRS},
        {"mest", DAYZONE, -1 HRS},      /* Middle European Summer Time */
        {"m.e.s.t.", DAYZONE, -1 HRS},
        {"wet", ZONE, -2 HRS },         /* Western European Time */
        {"w.e.t.", ZONE, -2 HRS },
        {"west", DAYZONE, -2 HRS},      /* Western European Summer Time */
        {"w.e.s.t.", DAYZONE, -2 HRS},

        {"jst", ZONE, -9 HRS},          /* Japan Standard Time */
        {"j.s.t.", ZONE, -9 HRS},       /* Japan Standard Time */
                                        /* No daylight savings time */

        {"aest", ZONE, -10 HRS},        /* Australian Eastern Time */
        {"a.e.s.t.", ZONE, -10 HRS},
        {"aesst", DAYZONE, -10 HRS},    /* Australian Eastern Summer Time */
        {"a.e.s.s.t.", DAYZONE, -10 HRS},
        {"acst", ZONE, -(9 HRS + HALFHR)},      /* Australian Central Time */
        {"a.c.s.t.", ZONE, -(9 HRS + HALFHR)},
        {"acsst", DAYZONE, -(9 HRS + HALFHR)},  /* Australian Central Summer */
        {"a.c.s.s.t.", DAYZONE, -(9 HRS + HALFHR)},
        {"awst", ZONE, -8 HRS},         /* Australian Western Time */
        {"a.w.s.t.", ZONE, -8 HRS},     /* (no daylight time there, I'm told */
        {0, 0, 0}};

static
struct table unittb[] = {
        {"year", MUNIT, 12},
        {"month", MUNIT, 1},
        {"fortnight", UNIT, 14*24*60},
        {"week", UNIT, 7*24*60},
        {"day", UNIT, 1*24*60},
        {"hour", UNIT, 60},
        {"minute", UNIT, 1},
        {"min", UNIT, 1},
        {"second", SUNIT, 1},
        {"sec", SUNIT, 1},
        {0, 0, 0}};

static
struct table othertb[] = {
        {"tomorrow", UNIT, 1*24*60},
        {"yesterday", UNIT, -1*24*60},
        {"today", UNIT, 0},
        {"now", UNIT, 0},
        {"last", NUMBER, -1},
        {"this", UNIT, 0},
        {"next", NUMBER, 2},
        {"first", NUMBER, 1},
        /* {"second", NUMBER, 2}, */
        {"third", NUMBER, 3},
        {"fourth", NUMBER, 4},
        {"fifth", NUMBER, 5},
        {"sixth", NUMBER, 6},
        {"seventh", NUMBER, 7},
        {"eigth", NUMBER, 8},
        {"ninth", NUMBER, 9},
        {"tenth", NUMBER, 10},
        {"eleventh", NUMBER, 11},
        {"twelfth", NUMBER, 12},
        {"ago", AGO, 1},
        {0, 0, 0}};

static
struct table milzone[] = {
        {"a", ZONE, 1 HRS},
        {"b", ZONE, 2 HRS},
        {"c", ZONE, 3 HRS},
        {"d", ZONE, 4 HRS},
        {"e", ZONE, 5 HRS},
        {"f", ZONE, 6 HRS},
        {"g", ZONE, 7 HRS},
        {"h", ZONE, 8 HRS},
        {"i", ZONE, 9 HRS},
        {"k", ZONE, 10 HRS},
        {"l", ZONE, 11 HRS},
        {"m", ZONE, 12 HRS},
        {"n", ZONE, -1 HRS},
        {"o", ZONE, -2 HRS},
        {"p", ZONE, -3 HRS},
        {"q", ZONE, -4 HRS},
        {"r", ZONE, -5 HRS},
        {"s", ZONE, -6 HRS},
        {"t", ZONE, -7 HRS},
        {"u", ZONE, -8 HRS},
        {"v", ZONE, -9 HRS},
        {"w", ZONE, -10 HRS},
        {"x", ZONE, -11 HRS},
        {"y", ZONE, -12 HRS},
        {"z", ZONE, 0 HRS},
        {0, 0, 0}};

static
lookup(id) char *id;
{
#define gotit (yylval=i->value,  i->type)
#define getid for(j=idvar, k=id; *j++ = *k++; )

        char idvar[20];
        register char *j, *k;
        register struct table *i;
        int abbrev;

        getid;
        if (strlen(idvar) == 3) abbrev = 1;
        else if (strlen(idvar) == 4 && idvar[3] == '.') {
                abbrev = 1;
                idvar[3] = '\0';
        }
        else abbrev = 0;

        if (islower(*idvar)) *idvar = toupper(*idvar);

        for (i = mdtab; i->name; i++) {
                k = idvar;
                for (j = i->name; *j++ == *k++;) {
                        if (abbrev && j==i->name+3) return gotit;
                        if (j[-1] == 0) return gotit;
                }
        }

        getid;
        for (i = mztab; i->name; i++)
                if (strcmp(i->name, idvar) == 0) return gotit;

        for (j = idvar; *j; j++)
                if (isupper(*j)) *j = tolower(*j);
        for (i=mztab; i->name; i++)
                if (strcmp(i->name, idvar) == 0) return gotit;

        getid;
        for (i=unittb; i->name; i++)
                if (strcmp(i->name, idvar) == 0) return gotit;

        if (idvar[strlen(idvar)-1] == 's')
                idvar[strlen(idvar)-1] = '\0';
        for (i=unittb; i->name; i++)
                if (strcmp(i->name, idvar) == 0) return gotit;

        getid;
        for (i = othertb; i->name; i++)
                if (strcmp(i->name, idvar) == 0) return gotit;

        getid;
        if (strlen(idvar) == 1 && isalpha(*idvar)) {
                if (isupper(*idvar)) *idvar = tolower(*idvar);
                for (i = milzone; i->name; i++)
                        if (strcmp(i->name, idvar) == 0) return gotit;
        }

        return(ID);
}

time_t
Tcl_GetDate (p, now, zone)
    char   *p;
    time_t  now;
    long    zone;
{
#define mcheck(f)       if (f>1) err++
        time_t monthadd();
        int err;
        struct tm *lt;
        time_t sdate, tod;

        lptr = p;
        if (now <= 0)
                (void) time(&now);
        lt = localtime(&now);
        year = lt->tm_year;
        month = lt->tm_mon+1;
        day = lt->tm_mday;
        relsec = 0; relmonth = 0;
        timeflag=zoneflag=dateflag=dayflag=relflag=0;
        dayLight = MAYBE;
        hh = mm = ss = 0;
        merid = 24;
        ourzone = zone;

        if (err = yyparse()) return (-1);

        mcheck(timeflag);
        mcheck(zoneflag);
        mcheck(dateflag);
        mcheck(dayflag);

        if (err) return (-1);
        if (dateflag || timeflag || dayflag) {
                sdate = dateconv(month,day,year,hh,mm,ss,merid,ourzone,
                                 dayLight);
                if (sdate < 0) return -1;
        }
        else {
                sdate = now;
                if (relflag == 0)
                        sdate -= (lt->tm_sec + lt->tm_min*60 +
                                lt->tm_hour*(60L*60L));
        }

        sdate += relsec;
        sdate += monthadd(sdate, relmonth);

        if (dayflag && !dateflag) {
                tod = dayconv(dayord, dayreq, sdate);
                sdate += tod;
        }

        return sdate;
}

/*
 * Error message are not used, so discard with dummy function.
 */

void
yyerror(msg)
    char *msg;
{
}
