/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     MONTH = 259,
     DAY = 260,
     MERIDIAN = 261,
     NUMBER = 262,
     UNIT = 263,
     MUNIT = 264,
     SUNIT = 265,
     ZONE = 266,
     DAYZONE = 267,
     AGO = 268
   };
#endif
/* Tokens.  */
#define ID 258
#define MONTH 259
#define DAY 260
#define MERIDIAN 261
#define NUMBER 262
#define UNIT 263
#define MUNIT 264
#define SUNIT 265
#define ZONE 266
#define DAYZONE 267
#define AGO 268




/* Copy the first part of user declarations.  */
#line 26 "tclxgdat.y"

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

	static time_t timeconv(int hh, int mm, int ss, int mer);
	static time_t daylcorr(time_t future, time_t now);
	static int lookup(char *id);
	static void yyerror(const char *msg);

#define AM 1
#define PM 2
#define DAYLIGHT 1
#define STANDARD 2
#define MAYBE    3


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 164 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   32

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  17
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  9
/* YYNRULES -- Number of rules.  */
#define YYNRULES  35
/* YYNRULES -- Number of states.  */
#define YYNSTATES  42

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   268

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    15,     2,     2,    16,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    14,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    17,
      19,    21,    24,    28,    33,    38,    44,    51,    58,    60,
      62,    64,    67,    70,    74,    80,    83,    88,    91,    95,
      98,   101,   104,   106,   108,   110
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      18,     0,    -1,    -1,    18,    19,    -1,    21,    -1,    22,
      -1,    24,    -1,    23,    -1,    25,    -1,    20,    -1,     7,
      -1,     7,     6,    -1,     7,    14,     7,    -1,     7,    14,
       7,     6,    -1,     7,    14,     7,     7,    -1,     7,    14,
       7,    14,     7,    -1,     7,    14,     7,    14,     7,     6,
      -1,     7,    14,     7,    14,     7,     7,    -1,    11,    -1,
      12,    -1,     5,    -1,     5,    15,    -1,     7,     5,    -1,
       7,    16,     7,    -1,     7,    16,     7,    16,     7,    -1,
       4,     7,    -1,     4,     7,    15,     7,    -1,     7,     4,
      -1,     7,     4,     7,    -1,     7,     8,    -1,     7,     9,
      -1,     7,    10,    -1,     8,    -1,     9,    -1,    10,    -1,
      25,    13,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    59,    59,    60,    62,    64,    66,    68,    70,    72,
      74,    78,    80,    82,    84,    87,    89,    91,    95,    97,
     100,   102,   104,   107,   109,   111,   113,   115,   117,   121,
     123,   125,   127,   129,   131,   133
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "MONTH", "DAY", "MERIDIAN",
  "NUMBER", "UNIT", "MUNIT", "SUNIT", "ZONE", "DAYZONE", "AGO", "':'",
  "','", "'/'", "$accept", "timedate", "item", "nspec", "tspec", "zone",
  "dyspec", "dtspec", "rspec", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,    58,    44,    47
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    17,    18,    18,    19,    19,    19,    19,    19,    19,
      20,    21,    21,    21,    21,    21,    21,    21,    22,    22,
      23,    23,    23,    24,    24,    24,    24,    24,    24,    25,
      25,    25,    25,    25,    25,    25
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     2,     3,     4,     4,     5,     6,     6,     1,     1,
       1,     2,     2,     3,     5,     2,     4,     2,     3,     2,
       2,     2,     1,     1,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,    20,    10,    32,    33,    34,    18,
      19,     3,     9,     4,     5,     7,     6,     8,    25,    21,
      27,    22,    11,    29,    30,    31,     0,     0,    35,     0,
      28,    12,    23,    26,    13,    14,     0,     0,    15,    24,
      16,    17
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,    11,    12,    13,    14,    15,    16,    17
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -10
static const yytype_int8 yypact[] =
{
     -10,     0,   -10,    -4,    -9,     9,   -10,   -10,   -10,   -10,
     -10,   -10,   -10,   -10,   -10,   -10,   -10,     3,     7,   -10,
      17,   -10,   -10,   -10,   -10,   -10,    19,    20,   -10,    22,
     -10,    14,    15,   -10,   -10,   -10,    23,    25,    -5,   -10,
     -10,   -10
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -10,   -10,   -10,   -10,   -10,   -10,   -10,   -10,   -10
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
       2,    40,    41,    18,     3,     4,    19,     5,     6,     7,
       8,     9,    10,    20,    21,    22,    28,    23,    24,    25,
      34,    35,    29,    26,    30,    27,    31,    32,    36,    33,
      38,    37,    39
};

static const yytype_uint8 yycheck[] =
{
       0,     6,     7,     7,     4,     5,    15,     7,     8,     9,
      10,    11,    12,     4,     5,     6,    13,     8,     9,    10,
       6,     7,    15,    14,     7,    16,     7,     7,    14,     7,
       7,    16,     7
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    18,     0,     4,     5,     7,     8,     9,    10,    11,
      12,    19,    20,    21,    22,    23,    24,    25,     7,    15,
       4,     5,     6,     8,     9,    10,    14,    16,    13,    15,
       7,     7,     7,     7,     6,     7,    14,    16,     7,     7,
       6,     7
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
#line 63 "tclxgdat.y"
    {timeflag++;}
    break;

  case 5:
#line 65 "tclxgdat.y"
    {zoneflag++;}
    break;

  case 6:
#line 67 "tclxgdat.y"
    {dateflag++;}
    break;

  case 7:
#line 69 "tclxgdat.y"
    {dayflag++;}
    break;

  case 8:
#line 71 "tclxgdat.y"
    {relflag++;}
    break;

  case 10:
#line 75 "tclxgdat.y"
    {if (timeflag && dateflag && !relflag) year = (yyvsp[(1) - (1)]);
                else {timeflag++;hh = (yyvsp[(1) - (1)])/100;mm = (yyvsp[(1) - (1)])%100;ss = 0;merid = 24;}}
    break;

  case 11:
#line 79 "tclxgdat.y"
    {hh = (yyvsp[(1) - (2)]); mm = 0; ss = 0; merid = (yyvsp[(2) - (2)]);}
    break;

  case 12:
#line 81 "tclxgdat.y"
    {hh = (yyvsp[(1) - (3)]); mm = (yyvsp[(3) - (3)]); merid = 24;}
    break;

  case 13:
#line 83 "tclxgdat.y"
    {hh = (yyvsp[(1) - (4)]); mm = (yyvsp[(3) - (4)]); merid = (yyvsp[(4) - (4)]);}
    break;

  case 14:
#line 85 "tclxgdat.y"
    {hh = (yyvsp[(1) - (4)]); mm = (yyvsp[(3) - (4)]); merid = 24;
                dayLight = STANDARD; ourzone = -((yyvsp[(4) - (4)])%100 + 60*(yyvsp[(4) - (4)])/100);}
    break;

  case 15:
#line 88 "tclxgdat.y"
    {hh = (yyvsp[(1) - (5)]); mm = (yyvsp[(3) - (5)]); ss = (yyvsp[(5) - (5)]); merid = 24;}
    break;

  case 16:
#line 90 "tclxgdat.y"
    {hh = (yyvsp[(1) - (6)]); mm = (yyvsp[(3) - (6)]); ss = (yyvsp[(5) - (6)]); merid = (yyvsp[(6) - (6)]);}
    break;

  case 17:
#line 92 "tclxgdat.y"
    {hh = (yyvsp[(1) - (6)]); mm = (yyvsp[(3) - (6)]); ss = (yyvsp[(5) - (6)]); merid = 24;
                dayLight = STANDARD; ourzone = -((yyvsp[(6) - (6)])%100 + 60*(yyvsp[(6) - (6)])/100);}
    break;

  case 18:
#line 96 "tclxgdat.y"
    {ourzone = (yyvsp[(1) - (1)]); dayLight = STANDARD;}
    break;

  case 19:
#line 98 "tclxgdat.y"
    {ourzone = (yyvsp[(1) - (1)]); dayLight = DAYLIGHT;}
    break;

  case 20:
#line 101 "tclxgdat.y"
    {dayord = 1; dayreq = (yyvsp[(1) - (1)]);}
    break;

  case 21:
#line 103 "tclxgdat.y"
    {dayord = 1; dayreq = (yyvsp[(1) - (2)]);}
    break;

  case 22:
#line 105 "tclxgdat.y"
    {dayord = (yyvsp[(1) - (2)]); dayreq = (yyvsp[(2) - (2)]);}
    break;

  case 23:
#line 108 "tclxgdat.y"
    {month = (yyvsp[(1) - (3)]); day = (yyvsp[(3) - (3)]);}
    break;

  case 24:
#line 110 "tclxgdat.y"
    {month = (yyvsp[(1) - (5)]); day = (yyvsp[(3) - (5)]); year = (yyvsp[(5) - (5)]);}
    break;

  case 25:
#line 112 "tclxgdat.y"
    {month = (yyvsp[(1) - (2)]); day = (yyvsp[(2) - (2)]);}
    break;

  case 26:
#line 114 "tclxgdat.y"
    {month = (yyvsp[(1) - (4)]); day = (yyvsp[(2) - (4)]); year = (yyvsp[(4) - (4)]);}
    break;

  case 27:
#line 116 "tclxgdat.y"
    {month = (yyvsp[(2) - (2)]); day = (yyvsp[(1) - (2)]);}
    break;

  case 28:
#line 118 "tclxgdat.y"
    {month = (yyvsp[(2) - (3)]); day = (yyvsp[(1) - (3)]); year = (yyvsp[(3) - (3)]);}
    break;

  case 29:
#line 122 "tclxgdat.y"
    {relsec +=  60L * (yyvsp[(1) - (2)]) * (yyvsp[(2) - (2)]);}
    break;

  case 30:
#line 124 "tclxgdat.y"
    {relmonth += (yyvsp[(1) - (2)]) * (yyvsp[(2) - (2)]);}
    break;

  case 31:
#line 126 "tclxgdat.y"
    {relsec += (yyvsp[(1) - (2)]);}
    break;

  case 32:
#line 128 "tclxgdat.y"
    {relsec +=  60L * (yyvsp[(1) - (1)]);}
    break;

  case 33:
#line 130 "tclxgdat.y"
    {relmonth += (yyvsp[(1) - (1)]);}
    break;

  case 34:
#line 132 "tclxgdat.y"
    {relsec++;}
    break;

  case 35:
#line 134 "tclxgdat.y"
    {relsec = -relsec; relmonth = -relmonth;}
    break;


/* Line 1267 of yacc.c.  */
#line 1546 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 135 "tclxgdat.y"


static int mdays[12] =
        {31, 0, 31,  30, 31, 30,  31, 31, 30,  31, 30, 31};
#define epoch 1970

extern struct tm *localtime();

static
time_t dateconv(int mm, int dd, int yy, int h, int m, int s, int mer, int zone, int dayflag)
{
        time_t tod, jdate;
        register int i;
        time_t timeconv(int hh, int mm, int ss, int mer);

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
time_t dayconv(int ord, int day, time_t now)
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
time_t timeconv(int hh, int mm, int ss, int mer)
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
int lookup(char *id)
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
Tcl_GetDate (
    char   *p,
    time_t  now,
    long    zone)
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
yyerror(const char *msg)
{
}

