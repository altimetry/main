
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 23 "../coda/libcoda/coda-expr-parser.y"


/* *INDENT-ON* */

/* Make parser independent from other parsers */
#define yymaxdepth coda_expression_maxdepth
#define yyparse coda_expression_parse
#define yylex   coda_expression_lex
#define yyerror coda_expression_error
#define yylval  coda_expression_lval
#define yychar  coda_expression_char
#define yydebug coda_expression_debug
#define yypact  coda_expression_pact
#define yyr1    coda_expression_r1
#define yyr2    coda_expression_r2
#define yydef   coda_expression_def
#define yychk   coda_expression_chk
#define yypgo   coda_expression_pgo
#define yyact   coda_expression_act
#define yyexca  coda_expression_exca
#define yyerrflag coda_expression_errflag
#define yynerrs coda_expression_nerrs
#define yyps    coda_expression_ps
#define yypv    coda_expression_pv
#define yys     coda_expression_s
#define yy_yys  coda_expression_yys
#define yystate coda_expression_state
#define yytmp   coda_expression_tmp
#define yyv     coda_expression_v
#define yy_yyv  coda_expression_yyv
#define yyval   coda_expression_val
#define yylloc  coda_expression_lloc
#define yyreds  coda_expression_reds
#define yytoks  coda_expression_toks
#define yylhs   coda_expression_yylhs
#define yylen   coda_expression_yylen
#define yydefred coda_expression_yydefred
#define yydgoto coda_expression_yydgoto
#define yysindex coda_expression_yysindex
#define yyrindex coda_expression_yyrindex
#define yygindex coda_expression_yygindex
#define yytable  coda_expression_yytable
#define yycheck  coda_expression_yycheck
#define yyname   coda_expression_yyname
#define yyrule   coda_expression_yyrule

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "coda-expr.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static coda_expression *parsed_expression;

/* tokenizer declarations */
int coda_expression_lex(void);
void *coda_expression__scan_string(const char *yy_str);
void coda_expression__delete_buffer(void *);

static void coda_expression_error(const char *error)
{
    coda_set_error(CODA_ERROR_EXPRESSION, "%s", error);
}

/* *INDENT-OFF* */



/* Line 189 of yacc.c  */
#line 147 "libcoda/coda-expr-parser.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT_VALUE = 258,
     FLOAT_VALUE = 259,
     STRING_VALUE = 260,
     NAME = 261,
     INDEX_VAR = 262,
     LOGICAL_OR = 263,
     LOGICAL_AND = 264,
     NOT = 265,
     NOT_EQUAL = 266,
     EQUAL = 267,
     LESS_EQUAL = 268,
     GREATER_EQUAL = 269,
     OR = 270,
     AND = 271,
     UNARY = 272,
     GOTO_PARENT = 273,
     RAW_PREFIX = 274,
     ASCIILINE = 275,
     DO = 276,
     FOR = 277,
     STEP = 278,
     TO = 279,
     NAN_VALUE = 280,
     INF_VALUE = 281,
     TRUE_VALUE = 282,
     FALSE_VALUE = 283,
     FUNC_ABS = 284,
     FUNC_ADD = 285,
     FUNC_ALL = 286,
     FUNC_BITOFFSET = 287,
     FUNC_BITSIZE = 288,
     FUNC_BOOL = 289,
     FUNC_BYTES = 290,
     FUNC_BYTEOFFSET = 291,
     FUNC_BYTESIZE = 292,
     FUNC_CEIL = 293,
     FUNC_COUNT = 294,
     FUNC_EXISTS = 295,
     FUNC_FILENAME = 296,
     FUNC_FILESIZE = 297,
     FUNC_FLOAT = 298,
     FUNC_FLOOR = 299,
     FUNC_GOTO = 300,
     FUNC_IF = 301,
     FUNC_INDEX = 302,
     FUNC_INT = 303,
     FUNC_ISNAN = 304,
     FUNC_ISINF = 305,
     FUNC_ISPLUSINF = 306,
     FUNC_ISMININF = 307,
     FUNC_LENGTH = 308,
     FUNC_LTRIM = 309,
     FUNC_NUMELEMENTS = 310,
     FUNC_MAX = 311,
     FUNC_MIN = 312,
     FUNC_PRODUCTCLASS = 313,
     FUNC_PRODUCTFORMAT = 314,
     FUNC_PRODUCTTYPE = 315,
     FUNC_PRODUCTVERSION = 316,
     FUNC_REGEX = 317,
     FUNC_ROUND = 318,
     FUNC_RTRIM = 319,
     FUNC_STR = 320,
     FUNC_STRTIME = 321,
     FUNC_SUBSTR = 322,
     FUNC_TIME = 323,
     FUNC_TRIM = 324,
     FUNC_UNBOUNDINDEX = 325,
     FUNC_WITH = 326
   };
#endif
/* Tokens.  */
#define INT_VALUE 258
#define FLOAT_VALUE 259
#define STRING_VALUE 260
#define NAME 261
#define INDEX_VAR 262
#define LOGICAL_OR 263
#define LOGICAL_AND 264
#define NOT 265
#define NOT_EQUAL 266
#define EQUAL 267
#define LESS_EQUAL 268
#define GREATER_EQUAL 269
#define OR 270
#define AND 271
#define UNARY 272
#define GOTO_PARENT 273
#define RAW_PREFIX 274
#define ASCIILINE 275
#define DO 276
#define FOR 277
#define STEP 278
#define TO 279
#define NAN_VALUE 280
#define INF_VALUE 281
#define TRUE_VALUE 282
#define FALSE_VALUE 283
#define FUNC_ABS 284
#define FUNC_ADD 285
#define FUNC_ALL 286
#define FUNC_BITOFFSET 287
#define FUNC_BITSIZE 288
#define FUNC_BOOL 289
#define FUNC_BYTES 290
#define FUNC_BYTEOFFSET 291
#define FUNC_BYTESIZE 292
#define FUNC_CEIL 293
#define FUNC_COUNT 294
#define FUNC_EXISTS 295
#define FUNC_FILENAME 296
#define FUNC_FILESIZE 297
#define FUNC_FLOAT 298
#define FUNC_FLOOR 299
#define FUNC_GOTO 300
#define FUNC_IF 301
#define FUNC_INDEX 302
#define FUNC_INT 303
#define FUNC_ISNAN 304
#define FUNC_ISINF 305
#define FUNC_ISPLUSINF 306
#define FUNC_ISMININF 307
#define FUNC_LENGTH 308
#define FUNC_LTRIM 309
#define FUNC_NUMELEMENTS 310
#define FUNC_MAX 311
#define FUNC_MIN 312
#define FUNC_PRODUCTCLASS 313
#define FUNC_PRODUCTFORMAT 314
#define FUNC_PRODUCTTYPE 315
#define FUNC_PRODUCTVERSION 316
#define FUNC_REGEX 317
#define FUNC_ROUND 318
#define FUNC_RTRIM 319
#define FUNC_STR 320
#define FUNC_STRTIME 321
#define FUNC_SUBSTR 322
#define FUNC_TIME 323
#define FUNC_TRIM 324
#define FUNC_UNBOUNDINDEX 325
#define FUNC_WITH 326




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 96 "../coda/libcoda/coda-expr-parser.y"

    char *stringval;
    struct coda_expression_struct *expr;



/* Line 214 of yacc.c  */
#line 332 "libcoda/coda-expr-parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 344 "libcoda/coda-expr-parser.c"

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
# if YYENABLE_NLS
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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  203
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2515

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  93
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  12
/* YYNRULES -- Number of rules.  */
#define YYNRULES  244
/* YYNRULES -- Number of states.  */
#define YYNSTATES  671

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   326

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    80,    23,     2,     2,
      85,    86,    21,    19,    87,    20,    88,    22,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    89,    84,
      12,    81,    11,     2,    92,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    82,     2,    83,    24,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    90,     2,    91,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9,    10,    13,    14,    15,    16,
      17,    18,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    31,    33,    35,    37,
      39,    41,    43,    45,    47,    49,    51,    53,    55,    57,
      59,    61,    63,    65,    67,    69,    71,    73,    75,    77,
      79,    81,    83,    85,    87,    89,    91,    93,    95,    97,
      99,   101,   103,   105,   107,   109,   111,   113,   115,   117,
     119,   121,   123,   125,   130,   138,   142,   151,   162,   167,
     176,   178,   180,   184,   188,   191,   195,   199,   203,   207,
     211,   215,   219,   223,   227,   231,   235,   239,   243,   247,
     251,   255,   259,   263,   267,   271,   275,   279,   283,   287,
     291,   295,   299,   303,   307,   311,   315,   320,   325,   330,
     335,   342,   347,   354,   362,   369,   378,   387,   389,   394,
     399,   404,   409,   412,   418,   420,   423,   426,   430,   434,
     438,   442,   446,   450,   454,   458,   463,   470,   477,   482,
     489,   496,   501,   506,   511,   516,   520,   524,   529,   534,
     539,   546,   554,   563,   570,   579,   588,   590,   592,   594,
     599,   604,   609,   614,   617,   620,   624,   628,   632,   636,
     640,   644,   648,   652,   656,   660,   664,   668,   672,   676,
     680,   684,   688,   692,   696,   700,   705,   710,   715,   720,
     727,   734,   741,   748,   755,   762,   769,   776,   785,   794,
     803,   812,   814,   817,   822,   827,   832,   839,   844,   851,
     855,   864,   869,   874,   879,   886,   895,   904,   908,   912,
     916,   920,   925,   930,   937,   944,   953,   962,   964,   966,
     968,   970,   972,   975,   980,   984,   988,   994,   998,  1003,
    1006,  1011,  1015,  1021,  1023
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      94,     0,    -1,    98,    -1,    99,    -1,   100,    -1,   101,
      -1,    97,    -1,   104,    -1,    27,    -1,    28,    -1,    29,
      -1,    30,    -1,    31,    -1,    32,    -1,    33,    -1,    34,
      -1,    35,    -1,    36,    -1,    37,    -1,    38,    -1,    39,
      -1,    40,    -1,    42,    -1,    43,    -1,    44,    -1,    45,
      -1,    46,    -1,    47,    -1,    48,    -1,    49,    -1,    50,
      -1,    51,    -1,    52,    -1,    53,    -1,    54,    -1,    55,
      -1,    56,    -1,    57,    -1,    58,    -1,    59,    -1,    60,
      -1,    61,    -1,    62,    -1,    63,    -1,    64,    -1,    65,
      -1,    66,    -1,    67,    -1,    68,    -1,    69,    -1,    70,
      -1,    71,    -1,    72,    -1,    73,    -1,    74,    -1,    75,
      -1,    76,    -1,    77,    -1,    78,    -1,    79,    -1,     7,
      -1,     6,    -1,    95,    -1,    80,    96,    81,    99,    -1,
      80,    96,    82,    99,    83,    81,    99,    -1,    97,    84,
      97,    -1,    30,     7,    81,    99,    32,    99,    29,    97,
      -1,    30,     7,    81,    99,    32,    99,    31,    99,    29,
      97,    -1,    53,    85,   104,    86,    -1,    79,    85,     7,
      81,    99,    87,    97,    86,    -1,    35,    -1,    36,    -1,
      98,     9,    98,    -1,    98,     8,    98,    -1,    10,    98,
      -1,    99,    16,    99,    -1,    99,    15,    99,    -1,    99,
      14,    99,    -1,    99,    13,    99,    -1,    99,    12,    99,
      -1,    99,    11,    99,    -1,   100,    16,   100,    -1,   100,
      16,    99,    -1,    99,    16,   100,    -1,   100,    15,   100,
      -1,   100,    15,    99,    -1,    99,    15,   100,    -1,   100,
      14,   100,    -1,   100,    14,    99,    -1,    99,    14,   100,
      -1,   100,    13,   100,    -1,   100,    13,    99,    -1,    99,
      13,   100,    -1,   100,    12,   100,    -1,   100,    12,    99,
      -1,    99,    12,   100,    -1,   100,    11,   100,    -1,   100,
      11,    99,    -1,    99,    11,   100,    -1,   101,    14,   101,
      -1,   101,    13,   101,    -1,   101,    12,   101,    -1,   101,
      11,   101,    -1,   101,    16,   101,    -1,   101,    15,   101,
      -1,    85,    98,    86,    -1,    57,    85,   100,    86,    -1,
      58,    85,   100,    86,    -1,    59,    85,   100,    86,    -1,
      60,    85,   100,    86,    -1,    70,    85,   101,    87,   101,
      86,    -1,    48,    85,   104,    86,    -1,    48,    85,   104,
      87,    98,    86,    -1,    48,    85,    80,    96,    87,    98,
      86,    -1,    39,    85,   104,    87,    98,    86,    -1,    54,
      85,    98,    87,    98,    87,    98,    86,    -1,    79,    85,
       7,    81,    99,    87,    98,    86,    -1,     3,    -1,    56,
      85,    99,    86,    -1,    56,    85,   104,    86,    -1,    56,
      85,    98,    86,    -1,    56,    85,   101,    86,    -1,    80,
      96,    -1,    80,    96,    82,    99,    83,    -1,     7,    -1,
      20,    99,    -1,    19,    99,    -1,    99,    19,    99,    -1,
      99,    20,    99,    -1,    99,    21,    99,    -1,    99,    22,
      99,    -1,    99,    23,    99,    -1,    99,    18,    99,    -1,
      99,    17,    99,    -1,    85,    99,    86,    -1,    37,    85,
      99,    86,    -1,    64,    85,    99,    87,    99,    86,    -1,
      65,    85,    99,    87,    99,    86,    -1,    63,    85,   104,
      86,    -1,    47,    85,   104,    87,    98,    86,    -1,    38,
      85,   104,    87,    99,    86,    -1,    61,    85,   101,    86,
      -1,    61,    85,   104,    86,    -1,    41,    85,   104,    86,
      -1,    45,    85,   104,    86,    -1,    69,    85,    86,    -1,
      50,    85,    86,    -1,    40,    85,   104,    86,    -1,    44,
      85,   104,    86,    -1,    55,    85,   104,    86,    -1,    55,
      85,   104,    87,    98,    86,    -1,    55,    85,    80,    96,
      87,    98,    86,    -1,    54,    85,    98,    87,    99,    87,
      99,    86,    -1,    78,    85,   104,    87,    98,    86,    -1,
      78,    85,   104,    87,    98,    87,    98,    86,    -1,    79,
      85,     7,    81,    99,    87,    99,    86,    -1,     4,    -1,
      33,    -1,    34,    -1,    51,    85,   100,    86,    -1,    51,
      85,   104,    86,    -1,    51,    85,    99,    86,    -1,    51,
      85,   101,    86,    -1,    20,   100,    -1,    19,   100,    -1,
     100,    19,   100,    -1,   100,    19,    99,    -1,    99,    19,
     100,    -1,   100,    20,   100,    -1,   100,    20,    99,    -1,
      99,    20,   100,    -1,   100,    21,   100,    -1,   100,    21,
      99,    -1,    99,    21,   100,    -1,   100,    22,   100,    -1,
     100,    22,    99,    -1,    99,    22,   100,    -1,   100,    23,
     100,    -1,   100,    23,    99,    -1,    99,    23,   100,    -1,
     100,    24,   100,    -1,   100,    24,    99,    -1,    99,    24,
     100,    -1,    99,    24,    99,    -1,    85,   100,    86,    -1,
      37,    85,   100,    86,    -1,    46,    85,   100,    86,    -1,
      52,    85,   100,    86,    -1,    71,    85,   100,    86,    -1,
      64,    85,   100,    87,   100,    86,    -1,    64,    85,   100,
      87,    99,    86,    -1,    64,    85,    99,    87,   100,    86,
      -1,    65,    85,   100,    87,   100,    86,    -1,    65,    85,
     100,    87,    99,    86,    -1,    65,    85,    99,    87,   100,
      86,    -1,    76,    85,   101,    87,   101,    86,    -1,    38,
      85,   104,    87,   100,    86,    -1,    54,    85,    98,    87,
     100,    87,   100,    86,    -1,    54,    85,    98,    87,   100,
      87,    99,    86,    -1,    54,    85,    98,    87,    99,    87,
     100,    86,    -1,    79,    85,     7,    81,    99,    87,   100,
      86,    -1,     5,    -1,    27,     5,    -1,    73,    85,   101,
      86,    -1,    73,    85,    99,    86,    -1,    73,    85,   104,
      86,    -1,    73,    85,   104,    87,    99,    86,    -1,    43,
      85,   104,    86,    -1,    43,    85,   104,    87,    99,    86,
      -1,   101,    19,   101,    -1,    75,    85,    99,    87,    99,
      87,   101,    86,    -1,    62,    85,   101,    86,    -1,    72,
      85,   101,    86,    -1,    77,    85,   101,    86,    -1,    38,
      85,   104,    87,   101,    86,    -1,    70,    85,   101,    87,
     101,    87,    99,    86,    -1,    70,    85,   101,    87,   101,
      87,   101,    86,    -1,    66,    85,    86,    -1,    67,    85,
      86,    -1,    68,    85,    86,    -1,    49,    85,    86,    -1,
      74,    85,    99,    86,    -1,    74,    85,   100,    86,    -1,
      74,    85,    99,    87,   101,    86,    -1,    74,    85,   100,
      87,   101,    86,    -1,    54,    85,    98,    87,   101,    87,
     101,    86,    -1,    79,    85,     7,    81,    99,    87,   101,
      86,    -1,    22,    -1,    88,    -1,    89,    -1,    26,    -1,
      28,    -1,   102,    96,    -1,   102,    90,    99,    91,    -1,
     103,    22,    26,    -1,   103,    22,    96,    -1,   103,    22,
      90,    99,    91,    -1,    82,    99,    83,    -1,   104,    82,
      99,    83,    -1,    92,    96,    -1,    92,    90,    99,    91,
      -1,   104,    92,    96,    -1,   104,    92,    90,    99,    91,
      -1,   102,    -1,   103,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   191,   191,   192,   193,   194,   195,   196,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   255,   256,   260,   264,   268,   272,   276,   280,   284,
     291,   295,   299,   303,   307,   311,   315,   319,   323,   327,
     331,   335,   339,   343,   347,   351,   355,   359,   363,   367,
     371,   375,   379,   383,   387,   391,   395,   399,   403,   407,
     411,   415,   419,   423,   427,   431,   432,   436,   440,   444,
     448,   452,   456,   460,   464,   468,   472,   479,   483,   486,
     490,   494,   498,   502,   506,   510,   514,   515,   519,   523,
     527,   531,   535,   539,   543,   544,   548,   552,   556,   560,
     564,   568,   572,   576,   580,   584,   588,   592,   596,   600,
     604,   608,   612,   616,   620,   624,   631,   635,   639,   643,
     646,   650,   654,   658,   662,   663,   667,   671,   675,   679,
     683,   687,   691,   695,   699,   703,   707,   711,   715,   719,
     723,   727,   731,   735,   739,   740,   744,   748,   752,   756,
     760,   764,   768,   772,   776,   780,   784,   788,   792,   796,
     800,   807,   811,   815,   818,   822,   826,   830,   834,   838,
     842,   846,   850,   854,   858,   862,   866,   870,   874,   878,
     882,   886,   890,   894,   898,   902,   906,   913,   920,   924,
     928,   932,   936,   940,   944,   948,   952,   956,   960,   964,
     968,   972,   976,   983,   984
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT_VALUE", "FLOAT_VALUE",
  "STRING_VALUE", "NAME", "INDEX_VAR", "LOGICAL_OR", "LOGICAL_AND", "NOT",
  "'>'", "'<'", "NOT_EQUAL", "EQUAL", "LESS_EQUAL", "GREATER_EQUAL", "OR",
  "AND", "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", "UNARY", "GOTO_PARENT",
  "RAW_PREFIX", "ASCIILINE", "DO", "FOR", "STEP", "TO", "NAN_VALUE",
  "INF_VALUE", "TRUE_VALUE", "FALSE_VALUE", "FUNC_ABS", "FUNC_ADD",
  "FUNC_ALL", "FUNC_BITOFFSET", "FUNC_BITSIZE", "FUNC_BOOL", "FUNC_BYTES",
  "FUNC_BYTEOFFSET", "FUNC_BYTESIZE", "FUNC_CEIL", "FUNC_COUNT",
  "FUNC_EXISTS", "FUNC_FILENAME", "FUNC_FILESIZE", "FUNC_FLOAT",
  "FUNC_FLOOR", "FUNC_GOTO", "FUNC_IF", "FUNC_INDEX", "FUNC_INT",
  "FUNC_ISNAN", "FUNC_ISINF", "FUNC_ISPLUSINF", "FUNC_ISMININF",
  "FUNC_LENGTH", "FUNC_LTRIM", "FUNC_NUMELEMENTS", "FUNC_MAX", "FUNC_MIN",
  "FUNC_PRODUCTCLASS", "FUNC_PRODUCTFORMAT", "FUNC_PRODUCTTYPE",
  "FUNC_PRODUCTVERSION", "FUNC_REGEX", "FUNC_ROUND", "FUNC_RTRIM",
  "FUNC_STR", "FUNC_STRTIME", "FUNC_SUBSTR", "FUNC_TIME", "FUNC_TRIM",
  "FUNC_UNBOUNDINDEX", "FUNC_WITH", "'$'", "'='", "'['", "']'", "';'",
  "'('", "')'", "','", "'.'", "':'", "'{'", "'}'", "'@'", "$accept",
  "input", "reserved_identifier", "identifier", "voidexpr", "boolexpr",
  "intexpr", "floatexpr", "stringexpr", "rootnode", "nonrootnode", "node", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,    62,    60,   266,   267,   268,   269,   270,   271,    43,
      45,    42,    47,    37,    94,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   293,   294,   295,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
      36,    61,    91,    93,    59,    40,    41,    44,    46,    58,
     123,   125,    64
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    93,    94,    94,    94,    94,    94,    94,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    96,    96,    97,    97,    97,    97,    97,    97,    97,
      98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    98,    98,    98,    98,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   102,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   104,   104
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     7,     3,     8,    10,     4,     8,
       1,     1,     3,     3,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     4,     4,     4,     4,
       6,     4,     6,     7,     6,     8,     8,     1,     4,     4,
       4,     4,     2,     5,     1,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     4,     6,     6,     4,     6,
       6,     4,     4,     4,     4,     3,     3,     4,     4,     4,
       6,     7,     8,     6,     8,     8,     1,     1,     1,     4,
       4,     4,     4,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     4,     4,     4,     4,     6,
       6,     6,     6,     6,     6,     6,     6,     8,     8,     8,
       8,     1,     2,     4,     4,     4,     6,     4,     6,     3,
       8,     4,     4,     4,     6,     8,     8,     3,     3,     3,
       3,     4,     4,     6,     6,     8,     8,     1,     1,     1,
       1,     1,     2,     4,     3,     3,     5,     3,     4,     2,
       4,     3,     5,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,   117,   156,   201,   124,     0,     0,     0,   227,   230,
       0,   231,     0,   157,   158,    70,    71,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   228,   229,     0,     0,     6,     2,     3,     4,
       5,   243,   244,     7,     0,     0,    74,     0,     0,     0,
       0,     0,     0,     0,   126,   164,   125,   163,   202,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    61,    60,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    62,   122,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   239,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   232,     0,     0,     0,     0,   122,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   220,   146,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     217,   218,   219,   145,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   126,   125,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   237,   105,   134,   184,
       0,     0,     0,    65,    73,    72,    80,    98,    79,    95,
      78,    92,    77,    89,    76,    86,    75,    83,   133,   132,
     127,   167,   128,   170,   129,   173,   130,   176,   131,   179,
     183,   182,    97,    96,    94,    93,    91,    90,    88,    87,
      85,    84,    82,    81,   166,   165,   169,   168,   172,   171,
     175,   174,   178,   177,   181,   180,   102,   101,   100,    99,
     104,   103,   209,     0,   234,     0,   235,     0,     0,   241,
       0,     0,     0,     0,     0,     0,   135,   185,     0,     0,
     147,   143,   207,     0,   148,   144,   186,     0,     0,   111,
       0,     0,     0,     0,   161,   159,   162,   160,   187,    68,
       0,     0,   149,     0,   120,   118,   121,   119,   106,   107,
     108,   109,     0,     0,     0,   141,   142,   211,   138,     0,
       0,     0,     0,     0,   188,   212,     0,     0,     0,   204,
     203,   205,     0,   221,     0,   222,     0,     0,     0,   213,
       0,     0,    63,     0,     0,     0,     0,     0,     0,     0,
     127,   128,   129,   130,   131,   240,     0,     0,   233,     0,
     238,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   123,     0,     0,
       0,     0,     0,     0,     0,   236,   242,     0,   123,     0,
       0,     0,     0,   140,   196,   214,   114,   208,   139,     0,
     112,     0,     0,     0,     0,     0,     0,     0,     0,   150,
       0,     0,     0,   136,   191,   190,   189,   137,   194,   193,
     192,   110,     0,     0,     0,     0,   206,   223,   224,     0,
     195,   153,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   113,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   151,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    64,     0,
       0,     0,     0,     0,     0,    66,     0,     0,   115,   152,
     199,   198,   197,   225,     0,   215,   216,     0,   210,   154,
      69,   116,   155,   200,   226,     0,     0,     0,     0,     0,
      67
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    65,   186,   187,   633,   634,    77,    78,    79,    71,
      72,    73
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -77
static const yytype_int16 yypact[] =
{
     742,   -77,   -77,   -77,   -77,  1465,  1702,  1702,   -77,   -77,
      43,   -77,     7,   -77,   -77,   -77,   -77,   -49,   -30,   -21,
     -14,    20,    22,    26,    42,    66,    75,    77,    80,    97,
     112,   118,   122,   162,   217,   237,   262,   266,   280,   282,
     289,   292,   294,   304,   310,   312,   319,   324,   334,   337,
     339,   340,   341,   365,   366,   367,   373,   375,   376,  1920,
    1757,  1465,   -77,   -77,  1161,    52,    -1,    69,  2483,  2084,
    1159,  1235,   147,     2,   388,  1920,   -77,  2483,  2084,  1159,
     389,   391,   403,  1702,   -77,   -77,   -77,   -77,   -77,   127,
    1702,   604,   604,   604,   604,   604,   604,   604,  1702,   604,
     170,   125,   167,   922,  1702,   604,  1465,  1631,   832,  1702,
    1702,  1702,  1702,   326,   662,   604,  1702,  1702,   220,   284,
     325,   360,   662,  1702,   662,  1010,  1702,  1757,   662,   662,
     604,    79,   -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,
     -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,
     -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,
     -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,
     -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,   -77,
     -77,   -77,   -77,   -77,   -77,   -77,   -77,   253,  1757,  1757,
     404,   407,   409,   410,   411,   413,  1757,  2378,     1,  1832,
     550,  1757,   -77,   -77,   293,  1465,  1465,  1702,  1702,  1702,
    1702,  1702,  1702,  1757,  1757,  1702,  1702,  1702,  1702,  1702,
    1702,  1702,  1702,  1702,  1702,  1702,  1702,  1702,  1702,  1702,
    1702,  1702,  1702,   662,   662,   662,   662,   662,   662,   662,
    1757,   -77,  1087,  1757,  1309,   371,   338,   604,  1465,   464,
    2008,  1388,  1757,  2016,  2232,   221,   223,    99,   320,   113,
     335,   361,  2491,  2240,   362,  1920,   263,   -77,   -77,   415,
     416,   417,  2024,  2248,     3,   377,  2256,   386,    -5,  1920,
     274,    34,  1846,   285,   393,  2264,  2272,  2280,  2300,   428,
     436,   441,    16,   398,    55,   405,   736,  1546,   826,  1625,
     -77,   -77,   -77,   -77,    -6,  2308,    78,   448,   449,   456,
    2094,    91,   276,  1022,  1467,  1004,     9,   116,   582,   402,
    1757,  1757,   -77,   -77,  1757,   604,  1465,  1757,  1757,   551,
    2102,  1757,  1757,  1757,  1757,  1757,   -77,   -77,   -77,   -77,
     364,   475,  1920,    -1,   -77,   -77,  2491,  1224,  2491,  1224,
    2491,  1224,  2491,  1224,  2491,  1224,  2491,  1224,   563,   563,
     528,   840,   528,   840,   555,   580,   555,   580,   555,   580,
     139,   581,  2491,  1224,  2491,  1224,  2491,  1224,  2491,  1224,
    2491,  1224,  2491,  1224,  1083,   840,  1083,   840,   166,   580,
     166,   580,   166,   580,   139,   581,   556,   556,   556,   556,
     556,   556,   -77,   497,   -77,  1757,   -77,  2386,  1757,   -77,
     515,  1757,   598,    -2,   531,  1868,   -77,   -77,  1544,  1465,
     -77,   -77,   -77,  1757,   -77,   -77,   -77,  1465,   519,   -77,
    1465,  1465,   662,   606,   -77,   -77,   -77,   -77,   -77,   -77,
    1465,   532,   -77,  1465,   -77,   -77,   -77,   -77,   -77,   -77,
     -77,   -77,   604,  1465,   614,   -77,   -77,   -77,   -77,  1702,
    1702,  1702,  1702,   662,   -77,   -77,   604,  1465,   618,   -77,
     -77,   -77,  1757,   -77,   662,   -77,   662,  1757,   662,   -77,
    1465,  1757,  2430,  2394,  2110,   615,    17,  1162,  1236,   546,
     126,   126,   -77,   -77,   -77,   -77,   621,   260,   -77,   525,
     -77,   572,  1757,  2402,  1702,  1702,  1757,  1757,  2032,  2316,
     123,    48,  2164,    51,  1465,    53,    25,    19,   554,    32,
     418,   516,   227,  1465,    59,   616,    36,   559,  2040,  2324,
    2048,  2332,  2118,  2340,  2126,  2348,   -11,   627,    38,   560,
    2172,   168,   169,  1310,   191,    -7,  1459,   562,  1757,  1757,
    1757,  1757,  1757,   579,  1757,   -77,   -77,  1538,   -77,   916,
    2225,  1617,  1884,   -77,   -77,   -77,   -77,   -77,   -77,    64,
     -77,  1544,   662,  1757,  1465,  1702,  1702,   662,    82,   -77,
     662,   662,  1757,   -77,   -77,   -77,   -77,   -77,   -77,   -77,
     -77,   -77,  1623,  1623,  1623,  1757,   -77,   -77,   -77,   662,
     -77,   -77,  1465,  1386,  1757,  2180,  1709,  2188,  2196,  1810,
    1757,  2410,  1465,  1702,   293,  1757,   -77,    31,    39,  1916,
      84,  2134,  2354,  2142,  2360,   225,   -77,  1987,  2204,   241,
    1994,   242,    90,   -33,    92,  1860,   635,   305,  2430,  1757,
    1757,  2001,   562,  2150,  2368,    -1,  1902,  1544,   -77,   -77,
     -77,   -77,   -77,   -77,   662,   -77,   -77,  1623,   -77,   -77,
     -77,   -77,   -77,   -77,   -77,  2212,  2220,   293,   293,   257,
      -1
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -77,   -77,   -77,    65,     0,   251,     5,    63,    30,   -77,
     -77,   -76
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_int16 yytable[] =
{
      66,   205,   206,   205,   206,    68,   205,   206,   239,   205,
     206,    84,    86,   239,    89,   255,   256,   257,   258,   259,
     260,   261,   239,   264,   266,   205,   206,   275,   239,   277,
      70,   280,   284,   205,   206,   239,    90,   293,   239,   295,
     205,   206,   205,   206,   205,   206,   205,   206,    88,   312,
     239,   204,   203,   660,   318,    91,   205,   206,   239,   205,
     206,   205,   206,    69,    92,   197,   199,   205,   206,    85,
      87,    93,   205,   206,   239,   591,   592,   205,   206,   601,
     602,   463,   440,   204,   243,   505,   319,   337,   250,   436,
     205,   206,   205,   206,   244,   253,   478,   239,   205,   206,
     205,   206,   455,   262,   549,    94,   572,    95,   272,   262,
     239,    96,   571,   282,   262,   262,   262,   262,   577,   574,
     444,   296,   298,   581,   200,   594,   592,    97,   262,   202,
     310,   313,   315,   274,   566,   239,   241,   568,   283,   570,
     246,   457,   239,   292,   294,   579,   251,   333,   334,   335,
     616,    98,   304,   254,   306,   311,   213,   214,   316,   317,
      99,   263,   100,    -1,   465,   101,   273,   276,   626,   242,
     648,   412,   285,   286,   287,   288,   659,   470,   661,   297,
     299,   243,   102,   213,   214,   420,   305,   239,   239,   314,
     220,   244,     8,   322,   323,   243,     9,   103,    11,   422,
     423,   330,   479,   104,   343,   244,   340,   105,   252,   565,
     239,   267,   346,   348,   350,   352,   354,   356,   358,   359,
     360,   362,   364,   366,   368,   370,   372,   374,   376,   378,
     380,   382,   384,   386,   388,   390,   392,   394,   233,   234,
     235,   236,   237,   238,   239,   403,   239,   106,   407,   485,
     265,    67,    60,   268,   597,   598,    76,   415,    62,    63,
     239,   239,    64,   396,   397,   398,   399,   400,   401,   402,
     347,   349,   351,   353,   355,   357,   239,   600,   361,   363,
     365,   367,   369,   371,   373,   375,   377,   379,   381,   383,
     385,   387,   389,   391,   393,   395,   233,   234,   235,   236,
     237,   238,   107,   243,   239,   243,   300,   406,   418,   409,
     419,   653,   198,   244,   577,   244,   233,   234,   235,   236,
     237,   238,   108,    12,   239,   482,   483,   656,   658,   484,
     428,     3,   487,   488,   320,   321,   490,   491,   492,   493,
     494,   320,   554,   664,   441,   243,    32,   109,     8,   429,
     430,   110,     9,    10,    11,   244,   243,   278,   243,   281,
     442,   443,   471,   472,   289,   111,   244,   112,   244,    22,
     301,   446,   341,   342,   113,    28,   525,   114,   410,   115,
     290,   213,   214,   331,   332,   333,   334,   335,    41,   116,
     537,   664,    45,    46,    47,   117,   270,   118,    51,    52,
      53,    54,   243,    56,   119,   291,   421,   497,    60,   120,
     499,   302,   244,   501,    62,    63,   503,   243,    64,   121,
     411,   424,   122,   508,   123,   124,   125,   244,   512,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   243,   243,   520,   303,   425,   510,   427,
     126,   127,   128,   244,   244,   495,   344,   345,   129,   243,
     130,   131,   517,   437,   528,   530,   532,   534,   243,   244,
     522,   414,   439,   245,   247,   243,   248,   540,   244,   447,
     243,   509,   543,   481,   456,   244,   546,   243,   249,   324,
     244,   458,   325,   536,   326,   327,   328,   244,   329,   413,
     431,   432,   433,   521,   541,   575,   542,   557,   544,   508,
     559,   561,   562,   452,   213,   214,   331,   332,   333,   334,
     335,   453,   529,   531,   533,   535,   454,   221,   222,   223,
     224,   225,   226,   466,   467,   227,   228,   229,   230,   231,
     232,   468,   213,   214,   331,   332,   333,   334,   335,   217,
     218,   219,   220,   605,   606,   607,   608,   609,   489,   611,
     496,   221,   222,   223,   224,   225,   226,   509,   560,   227,
     228,   229,   230,   231,   232,   239,   559,   486,   619,   220,
     621,   623,   331,   332,   333,   334,   335,   627,   498,   213,
     214,   331,   332,   333,   334,   335,   502,   628,   605,   606,
     630,   617,   618,   576,   232,    -1,   514,   625,   635,   638,
     510,   617,   506,   518,   645,   641,   555,   635,   643,   523,
     646,   527,   629,   510,   617,   539,     8,   552,   553,   631,
       9,     0,    11,   637,   560,   573,   339,     0,   622,   624,
     582,   595,   637,   604,   665,   666,   221,   222,   223,   224,
     225,   226,   643,     0,   227,   228,   229,   230,   231,   232,
     610,     0,   666,   556,   243,     0,   636,     3,   670,   480,
     511,     0,     0,     0,   244,   636,   644,   669,   513,     0,
     243,   515,   516,     0,   669,   504,    60,   669,     0,    10,
     244,   519,    62,    63,   524,     0,    64,   243,   243,     0,
     289,     0,   548,   580,   526,    22,     0,   244,   244,   243,
     644,    28,     0,     0,   593,     0,   290,     0,   538,   244,
       0,   663,     0,     0,    41,     0,     0,     0,    45,    46,
      47,   545,   270,     0,    51,    52,    53,    54,     0,    56,
       0,   291,     0,     0,     0,     1,     2,     3,     0,     4,
       0,     0,     5,   213,   214,   215,   216,   217,   218,   219,
     220,     6,     7,     0,     8,   569,     0,     0,     9,    10,
      11,     0,    12,     0,   578,    13,    14,    15,    16,    17,
      18,    19,    20,    21,     0,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,   459,    60,   620,     0,    61,     0,     0,
      62,    63,     0,     0,    64,     1,     2,     3,     0,     4,
       0,     0,     5,   213,   214,   215,   216,   217,   218,   219,
     220,     6,     7,   632,     8,     0,     0,     0,     9,    10,
      11,   229,   230,   231,   232,    13,    14,    15,    16,    17,
      18,    19,    20,    21,     0,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,     0,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    74,    75,   461,    60,     0,     0,    61,     0,     0,
      62,    63,     0,     0,    64,     1,     2,     3,     0,     4,
       0,     0,     0,   213,   214,   215,   216,   217,   218,   219,
     220,     6,     7,     0,     8,     0,     0,     0,     9,    10,
      11,     0,     0,     0,     0,    13,    14,     0,     0,    17,
      18,     0,    20,    21,     0,    22,    23,    24,    25,    26,
       0,    28,    29,    30,    31,     0,   269,    34,    35,     0,
       0,     0,     0,    40,    41,    42,    43,    44,    45,    46,
      47,    48,   270,    50,    51,    52,    53,    54,    55,    56,
      57,   271,    75,   575,    60,     0,     0,    83,     0,     0,
      62,    63,     0,     1,    64,     3,     0,     4,     0,     0,
       0,   213,   214,   331,   332,   333,   334,   335,     0,   188,
     189,     0,     8,     0,     0,     0,     9,    10,    11,   213,
     214,   215,   216,   217,   218,   219,   220,   190,   307,     0,
      20,    21,     0,    22,    23,    24,     0,    26,     0,    28,
      29,     0,     0,     0,   308,    34,    35,     0,     0,     0,
       0,    40,    41,    42,   193,   194,    45,    46,    47,    48,
     270,     0,    51,    52,    53,    54,     0,    56,    57,   309,
      75,   477,    60,   132,   133,   196,     0,     0,    62,    63,
     213,   214,    64,     0,   217,   218,   219,   220,   473,   474,
       0,     0,     0,   404,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,     0,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   132,   133,     0,
     233,   234,   235,   236,   237,   238,     0,   405,   239,   213,
     214,   331,   332,   333,   334,   335,     0,     0,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,     0,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   132,   133,   227,   228,   229,   230,   231,   232,   550,
       0,   201,     0,   213,   214,   331,   332,   333,   334,   335,
       0,     0,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,     0,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   132,   133,     0,     0,     0,
       0,     0,     0,   551,     0,   240,     0,   213,   214,   331,
     332,   333,   334,   335,     0,     0,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
       0,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,     1,
       2,     3,     0,     4,     0,     0,     5,   599,     0,   408,
       0,     0,     0,     0,     0,     6,     7,   227,   228,   229,
     230,   231,   232,    10,     0,     0,    12,     0,     0,    13,
      14,    15,    16,    17,    18,    19,    20,    21,     0,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,     0,     1,     2,
       3,    61,     4,     0,   339,     5,   213,   214,   331,   332,
     333,   334,   335,     0,     6,     7,   227,   228,   229,   230,
     231,   232,    10,     0,     0,     0,     0,     0,    13,    14,
      15,    16,    17,    18,    19,    20,    21,     0,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,     0,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    74,    75,   603,     1,     2,     3,
      61,     4,     0,   475,   476,   213,   214,   331,   332,   333,
     334,   335,     0,     6,     7,   227,   228,   229,   230,   231,
     232,    10,     0,     0,     0,     0,     0,    13,    14,     0,
       0,    17,    18,     0,    20,    21,     0,    22,    23,    24,
      25,    26,     0,    28,    29,    30,    31,     0,   269,    34,
      35,     0,     0,     0,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,   270,    50,    51,    52,    53,    54,
      55,    56,    57,   271,    75,   612,     1,     0,     3,    83,
       4,     0,     0,   460,   213,   214,   331,   332,   333,   334,
     335,     0,   188,   189,   227,   228,   229,   230,   231,   232,
      10,     0,     0,     8,     0,     0,     0,     9,     0,    11,
     190,   307,     0,    20,    21,     0,    22,    23,    24,     0,
      26,     0,    28,    29,     0,     0,     0,   308,    34,    35,
       0,     0,     0,     0,    40,    41,    42,   193,   194,    45,
      46,    47,    48,   270,     0,    51,    52,    53,    54,     0,
      56,    57,   309,    75,   613,     1,     2,     0,   196,     4,
       0,   279,   462,    60,     0,     0,     0,     0,     0,    62,
      63,     6,     7,    64,     0,     0,   213,   214,   331,   332,
     333,   334,   335,     0,     0,    13,    14,     0,     0,    17,
      80,     0,    20,    21,     0,     0,    23,    24,    25,    26,
       0,     0,    29,    30,    31,     0,    81,    34,    35,     0,
       1,     0,     0,    40,     4,    42,    43,    44,     0,     0,
       0,    48,     0,    50,     0,     0,   188,   189,    55,     0,
      57,    82,    75,     0,     0,     0,     0,    83,     0,     0,
       0,     0,     0,     0,   190,   191,   639,    20,    21,     0,
       0,    23,    24,     0,    26,     0,     0,    29,     0,     0,
       0,   192,    34,    35,     0,     0,     0,     0,    40,     0,
      42,   193,   194,     0,     0,     0,    48,   213,   214,   331,
     332,   333,   334,   335,     0,    57,   195,    75,     0,     0,
       0,     0,   196,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   213,   214,   331,   332,   333,
     334,   335,     0,     0,     0,     0,     0,   640,     0,     0,
     507,   213,   214,   331,   332,   333,   334,   335,     0,     0,
       0,     0,     0,   614,     0,   615,     0,     0,   338,   213,
     214,   331,   332,   333,   334,   335,   132,   133,     0,     0,
       0,   668,   445,   213,   214,   331,   332,   333,   334,   335,
       0,     0,     0,     0,     0,     0,   662,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,     0,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
       0,     0,     0,   647,   213,   214,   331,   332,   333,   334,
     335,   213,   214,   331,   332,   333,   334,   335,   213,   214,
     331,   332,   333,   334,   335,   213,   214,   215,   216,   217,
     218,   219,   220,   213,   214,   215,   216,   217,   218,   219,
     220,   213,   214,   215,   216,   217,   218,   219,   220,   213,
     214,   215,   216,   217,   218,   219,   220,   213,   214,   215,
     216,   217,   218,   219,   220,   213,   214,   215,   216,   217,
     218,   219,   220,     0,   654,     0,     0,     0,     0,     0,
       0,   657,     0,     0,     0,     0,     0,     0,   667,     0,
       0,     0,     0,     0,   338,   221,   222,   223,   224,   225,
     226,     0,   416,   227,   228,   229,   230,   231,   232,     0,
     434,   213,   214,   331,   332,   333,   334,   335,   563,   213,
     214,   331,   332,   333,   334,   335,   583,   213,   214,   331,
     332,   333,   334,   335,   585,   213,   214,   215,   216,   217,
     218,   219,   220,   213,   214,   215,   216,   217,   218,   219,
     220,   213,   214,   215,   216,   217,   218,   219,   220,   213,
     214,   215,   216,   217,   218,   219,   220,   213,   214,   215,
     216,   217,   218,   219,   220,     0,     0,     0,     0,     0,
     469,   213,   214,   331,   332,   333,   334,   335,   338,   213,
     214,   331,   332,   333,   334,   335,   416,   213,   214,   331,
     332,   333,   334,   335,   587,   213,   214,   331,   332,   333,
     334,   335,   589,   213,   214,   331,   332,   333,   334,   335,
     649,   213,   214,   331,   332,   333,   334,   335,   651,   213,
     214,   331,   332,   333,   334,   335,   662,   213,   214,   331,
     332,   333,   334,   335,   227,   228,   229,   230,   231,   232,
     567,   227,   228,   229,   230,   231,   232,     0,   596,   227,
     228,   229,   230,   231,   232,     0,   563,   227,   228,   229,
     230,   231,   232,     0,   583,   227,   228,   229,   230,   231,
     232,     0,   587,   227,   228,   229,   230,   231,   232,     0,
     655,   227,   228,   229,   230,   231,   232,     0,   649,   227,
     228,   229,   230,   231,   232,     0,   662,     0,     0,     0,
       0,     0,   576,     0,     0,     0,     0,     0,   417,   227,
     228,   229,   230,   231,   232,     0,   426,   227,   228,   229,
     230,   231,   232,     0,   435,   227,   228,   229,   230,   231,
     232,     0,   438,   227,   228,   229,   230,   231,   232,     0,
     448,   227,   228,   229,   230,   231,   232,     0,   449,   227,
     228,   229,   230,   231,   232,     0,   450,   227,   228,   229,
     230,   231,   232,   227,   228,   229,   230,   231,   232,   227,
     228,   229,   230,   231,   232,     0,   451,   227,   228,   229,
     230,   231,   232,     0,   464,   213,   214,   331,   332,   333,
     334,   335,   564,   213,   214,   331,   332,   333,   334,   335,
     584,   213,   214,   331,   332,   333,   334,   335,   586,   213,
     214,   331,   332,   333,   334,   335,   588,   213,   214,   331,
     332,   333,   334,   335,   590,     0,     0,     0,     0,     0,
     650,     0,     0,     0,     0,     0,   652,   213,   214,   331,
     332,   333,   334,   335,   663,     0,     0,     0,     0,     0,
       0,   336,     0,     0,     0,     0,     0,     0,     0,   500,
       0,     0,     0,     0,     0,     0,     0,   547,     0,     0,
       0,     0,     0,     0,     0,   558,     0,     0,     0,     0,
       0,     0,     0,   642,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   213,   214,
     215,   216,   217,   218,   219,   220
};

static const yytype_int16 yycheck[] =
{
       0,     8,     9,     8,     9,     0,     8,     9,    19,     8,
       9,     6,     7,    19,     7,    91,    92,    93,    94,    95,
      96,    97,    19,    99,   100,     8,     9,   103,    19,   105,
       0,   107,   108,     8,     9,    19,    85,   113,    19,   115,
       8,     9,     8,     9,     8,     9,     8,     9,     5,   125,
      19,    84,     0,    86,   130,    85,     8,     9,    19,     8,
       9,     8,     9,     0,    85,    60,    61,     8,     9,     6,
       7,    85,     8,     9,    19,    86,    87,     8,     9,    86,
      87,    87,    87,    84,    82,    87,     7,    86,    83,    86,
       8,     9,     8,     9,    92,    90,    87,    19,     8,     9,
       8,     9,    86,    98,    87,    85,    87,    85,   103,   104,
      19,    85,    87,   108,   109,   110,   111,   112,    87,    87,
      86,   116,   117,    87,    61,    87,    87,    85,   123,    64,
     125,   126,   127,   103,    86,    19,    71,    86,   108,    86,
      75,    86,    19,   113,   114,    86,    83,    21,    22,    23,
      86,    85,   122,    90,   124,   125,    17,    18,   128,   129,
      85,    98,    85,    24,    86,    85,   103,   104,    86,    22,
      86,   247,   109,   110,   111,   112,    86,    86,    86,   116,
     117,    82,    85,    17,    18,    86,   123,    19,    19,   126,
      24,    92,    22,   188,   189,    82,    26,    85,    28,    86,
      87,   196,    86,    85,   204,    92,   201,    85,    81,    86,
      19,    86,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,    11,    12,
      13,    14,    15,    16,    19,   240,    19,    85,   243,   325,
      80,     0,    82,    86,    86,    86,     5,   252,    88,    89,
      19,    19,    92,   233,   234,   235,   236,   237,   238,   239,
     207,   208,   209,   210,   211,   212,    19,    86,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,    11,    12,    13,    14,
      15,    16,    85,    82,    19,    82,    86,   242,    87,   244,
      87,    86,    61,    92,    87,    92,    11,    12,    13,    14,
      15,    16,    85,    30,    19,   320,   321,    86,    86,   324,
     265,     5,   327,   328,    81,    82,   331,   332,   333,   334,
     335,    81,    82,    86,   279,    82,    53,    85,    22,    86,
      87,    85,    26,    27,    28,    92,    82,   106,    82,   108,
      86,    87,    86,    87,    38,    85,    92,    85,    92,    43,
      86,    86,    79,    80,    85,    49,   452,    85,     7,    85,
      54,    17,    18,    19,    20,    21,    22,    23,    62,    85,
     466,    86,    66,    67,    68,    85,    70,    85,    72,    73,
      74,    75,    82,    77,    85,    79,    86,   342,    82,    85,
     405,    86,    92,   408,    88,    89,   411,    82,    92,    85,
      82,    86,    85,   418,    85,    85,    85,    92,   423,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    82,    82,   440,    86,    86,   418,    87,
      85,    85,    85,    92,    92,    91,   205,   206,    85,    82,
      85,    85,   432,    86,   459,   460,   461,   462,    82,    92,
     440,     7,    86,    85,    85,    82,    85,   472,    92,    86,
      82,   418,   477,    81,    86,    92,   481,    82,    85,    85,
      92,    86,    85,   463,    85,    85,    85,    92,    85,   248,
      85,    85,    85,   440,   474,    87,   476,   502,   478,   504,
     505,   506,   507,    85,    17,    18,    19,    20,    21,    22,
      23,    85,   459,   460,   461,   462,    85,    11,    12,    13,
      14,    15,    16,    85,    85,    19,    20,    21,    22,    23,
      24,    85,    17,    18,    19,    20,    21,    22,    23,    21,
      22,    23,    24,   548,   549,   550,   551,   552,     7,   554,
      85,    11,    12,    13,    14,    15,    16,   504,   505,    19,
      20,    21,    22,    23,    24,    19,   571,   326,   573,    24,
     575,   576,    19,    20,    21,    22,    23,   582,    91,    17,
      18,    19,    20,    21,    22,    23,    81,   592,   593,   594,
     595,   571,   572,    87,    24,    24,    87,   577,   603,   604,
     580,   581,    81,     7,   614,   610,    91,   612,   613,    87,
     615,     7,   592,   593,   594,     7,    22,    81,     7,   599,
      26,    -1,    28,   603,   571,    81,    86,    -1,   575,   576,
      81,    81,   612,    81,   639,   640,    11,    12,    13,    14,
      15,    16,   647,    -1,    19,    20,    21,    22,    23,    24,
      81,    -1,   657,    91,    82,    -1,   603,     5,   668,    87,
     419,    -1,    -1,    -1,    92,   612,   613,   647,   427,    -1,
      82,   430,   431,    -1,   654,    87,    82,   657,    -1,    27,
      92,   440,    88,    89,   443,    -1,    92,    82,    82,    -1,
      38,    -1,    87,    87,   453,    43,    -1,    92,    92,    82,
     647,    49,    -1,    -1,    87,    -1,    54,    -1,   467,    92,
      -1,    86,    -1,    -1,    62,    -1,    -1,    -1,    66,    67,
      68,   480,    70,    -1,    72,    73,    74,    75,    -1,    77,
      -1,    79,    -1,    -1,    -1,     3,     4,     5,    -1,     7,
      -1,    -1,    10,    17,    18,    19,    20,    21,    22,    23,
      24,    19,    20,    -1,    22,   514,    -1,    -1,    26,    27,
      28,    -1,    30,    -1,   523,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    87,    82,   574,    -1,    85,    -1,    -1,
      88,    89,    -1,    -1,    92,     3,     4,     5,    -1,     7,
      -1,    -1,    10,    17,    18,    19,    20,    21,    22,    23,
      24,    19,    20,   602,    22,    -1,    -1,    -1,    26,    27,
      28,    21,    22,    23,    24,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    -1,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    87,    82,    -1,    -1,    85,    -1,    -1,
      88,    89,    -1,    -1,    92,     3,     4,     5,    -1,     7,
      -1,    -1,    -1,    17,    18,    19,    20,    21,    22,    23,
      24,    19,    20,    -1,    22,    -1,    -1,    -1,    26,    27,
      28,    -1,    -1,    -1,    -1,    33,    34,    -1,    -1,    37,
      38,    -1,    40,    41,    -1,    43,    44,    45,    46,    47,
      -1,    49,    50,    51,    52,    -1,    54,    55,    56,    -1,
      -1,    -1,    -1,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    87,    82,    -1,    -1,    85,    -1,    -1,
      88,    89,    -1,     3,    92,     5,    -1,     7,    -1,    -1,
      -1,    17,    18,    19,    20,    21,    22,    23,    -1,    19,
      20,    -1,    22,    -1,    -1,    -1,    26,    27,    28,    17,
      18,    19,    20,    21,    22,    23,    24,    37,    38,    -1,
      40,    41,    -1,    43,    44,    45,    -1,    47,    -1,    49,
      50,    -1,    -1,    -1,    54,    55,    56,    -1,    -1,    -1,
      -1,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    -1,    72,    73,    74,    75,    -1,    77,    78,    79,
      80,    87,    82,     6,     7,    85,    -1,    -1,    88,    89,
      17,    18,    92,    -1,    21,    22,    23,    24,    86,    87,
      -1,    -1,    -1,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     6,     7,    -1,
      11,    12,    13,    14,    15,    16,    -1,    90,    19,    17,
      18,    19,    20,    21,    22,    23,    -1,    -1,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,     6,     7,    19,    20,    21,    22,    23,    24,    87,
      -1,    90,    -1,    17,    18,    19,    20,    21,    22,    23,
      -1,    -1,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,     6,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    87,    -1,    90,    -1,    17,    18,    19,
      20,    21,    22,    23,    -1,    -1,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,     3,
       4,     5,    -1,     7,    -1,    -1,    10,    87,    -1,    90,
      -1,    -1,    -1,    -1,    -1,    19,    20,    19,    20,    21,
      22,    23,    24,    27,    -1,    -1,    30,    -1,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    -1,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    -1,     3,     4,
       5,    85,     7,    -1,    86,    10,    17,    18,    19,    20,
      21,    22,    23,    -1,    19,    20,    19,    20,    21,    22,
      23,    24,    27,    -1,    -1,    -1,    -1,    -1,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    -1,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    87,     3,     4,     5,
      85,     7,    -1,    86,    87,    17,    18,    19,    20,    21,
      22,    23,    -1,    19,    20,    19,    20,    21,    22,    23,
      24,    27,    -1,    -1,    -1,    -1,    -1,    33,    34,    -1,
      -1,    37,    38,    -1,    40,    41,    -1,    43,    44,    45,
      46,    47,    -1,    49,    50,    51,    52,    -1,    54,    55,
      56,    -1,    -1,    -1,    -1,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    87,     3,    -1,     5,    85,
       7,    -1,    -1,    87,    17,    18,    19,    20,    21,    22,
      23,    -1,    19,    20,    19,    20,    21,    22,    23,    24,
      27,    -1,    -1,    22,    -1,    -1,    -1,    26,    -1,    28,
      37,    38,    -1,    40,    41,    -1,    43,    44,    45,    -1,
      47,    -1,    49,    50,    -1,    -1,    -1,    54,    55,    56,
      -1,    -1,    -1,    -1,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    -1,    72,    73,    74,    75,    -1,
      77,    78,    79,    80,    87,     3,     4,    -1,    85,     7,
      -1,    80,    87,    82,    -1,    -1,    -1,    -1,    -1,    88,
      89,    19,    20,    92,    -1,    -1,    17,    18,    19,    20,
      21,    22,    23,    -1,    -1,    33,    34,    -1,    -1,    37,
      38,    -1,    40,    41,    -1,    -1,    44,    45,    46,    47,
      -1,    -1,    50,    51,    52,    -1,    54,    55,    56,    -1,
       3,    -1,    -1,    61,     7,    63,    64,    65,    -1,    -1,
      -1,    69,    -1,    71,    -1,    -1,    19,    20,    76,    -1,
      78,    79,    80,    -1,    -1,    -1,    -1,    85,    -1,    -1,
      -1,    -1,    -1,    -1,    37,    38,    87,    40,    41,    -1,
      -1,    44,    45,    -1,    47,    -1,    -1,    50,    -1,    -1,
      -1,    54,    55,    56,    -1,    -1,    -1,    -1,    61,    -1,
      63,    64,    65,    -1,    -1,    -1,    69,    17,    18,    19,
      20,    21,    22,    23,    -1,    78,    79,    80,    -1,    -1,
      -1,    -1,    85,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    17,    18,    19,    20,    21,
      22,    23,    -1,    -1,    -1,    -1,    -1,    87,    -1,    -1,
      32,    17,    18,    19,    20,    21,    22,    23,    -1,    -1,
      -1,    -1,    -1,    29,    -1,    31,    -1,    -1,    86,    17,
      18,    19,    20,    21,    22,    23,     6,     7,    -1,    -1,
      -1,    29,    86,    17,    18,    19,    20,    21,    22,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      -1,    -1,    -1,    87,    17,    18,    19,    20,    21,    22,
      23,    17,    18,    19,    20,    21,    22,    23,    17,    18,
      19,    20,    21,    22,    23,    17,    18,    19,    20,    21,
      22,    23,    24,    17,    18,    19,    20,    21,    22,    23,
      24,    17,    18,    19,    20,    21,    22,    23,    24,    17,
      18,    19,    20,    21,    22,    23,    24,    17,    18,    19,
      20,    21,    22,    23,    24,    17,    18,    19,    20,    21,
      22,    23,    24,    -1,    87,    -1,    -1,    -1,    -1,    -1,
      -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,    87,    -1,
      -1,    -1,    -1,    -1,    86,    11,    12,    13,    14,    15,
      16,    -1,    86,    19,    20,    21,    22,    23,    24,    -1,
      86,    17,    18,    19,    20,    21,    22,    23,    86,    17,
      18,    19,    20,    21,    22,    23,    86,    17,    18,    19,
      20,    21,    22,    23,    86,    17,    18,    19,    20,    21,
      22,    23,    24,    17,    18,    19,    20,    21,    22,    23,
      24,    17,    18,    19,    20,    21,    22,    23,    24,    17,
      18,    19,    20,    21,    22,    23,    24,    17,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,
      86,    17,    18,    19,    20,    21,    22,    23,    86,    17,
      18,    19,    20,    21,    22,    23,    86,    17,    18,    19,
      20,    21,    22,    23,    86,    17,    18,    19,    20,    21,
      22,    23,    86,    17,    18,    19,    20,    21,    22,    23,
      86,    17,    18,    19,    20,    21,    22,    23,    86,    17,
      18,    19,    20,    21,    22,    23,    86,    17,    18,    19,
      20,    21,    22,    23,    19,    20,    21,    22,    23,    24,
      86,    19,    20,    21,    22,    23,    24,    -1,    86,    19,
      20,    21,    22,    23,    24,    -1,    86,    19,    20,    21,
      22,    23,    24,    -1,    86,    19,    20,    21,    22,    23,
      24,    -1,    86,    19,    20,    21,    22,    23,    24,    -1,
      86,    19,    20,    21,    22,    23,    24,    -1,    86,    19,
      20,    21,    22,    23,    24,    -1,    86,    -1,    -1,    -1,
      -1,    -1,    87,    -1,    -1,    -1,    -1,    -1,    86,    19,
      20,    21,    22,    23,    24,    -1,    86,    19,    20,    21,
      22,    23,    24,    -1,    86,    19,    20,    21,    22,    23,
      24,    -1,    86,    19,    20,    21,    22,    23,    24,    -1,
      86,    19,    20,    21,    22,    23,    24,    -1,    86,    19,
      20,    21,    22,    23,    24,    -1,    86,    19,    20,    21,
      22,    23,    24,    19,    20,    21,    22,    23,    24,    19,
      20,    21,    22,    23,    24,    -1,    86,    19,    20,    21,
      22,    23,    24,    -1,    86,    17,    18,    19,    20,    21,
      22,    23,    86,    17,    18,    19,    20,    21,    22,    23,
      86,    17,    18,    19,    20,    21,    22,    23,    86,    17,
      18,    19,    20,    21,    22,    23,    86,    17,    18,    19,
      20,    21,    22,    23,    86,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    -1,    -1,    -1,    86,    17,    18,    19,
      20,    21,    22,    23,    86,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    17,    18,
      19,    20,    21,    22,    23,    24
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     7,    10,    19,    20,    22,    26,
      27,    28,    30,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      82,    85,    88,    89,    92,    94,    97,    98,    99,   100,
     101,   102,   103,   104,    79,    80,    98,    99,   100,   101,
      38,    54,    79,    85,    99,   100,    99,   100,     5,     7,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,     6,     7,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    95,    96,    19,    20,
      37,    38,    54,    64,    65,    79,    85,    99,    98,    99,
     100,    90,    96,     0,    84,     8,     9,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    11,    12,    13,    14,    15,    16,    19,    20,    21,
      22,    23,    24,    11,    12,    13,    14,    15,    16,    19,
      90,    96,    22,    82,    92,    85,    96,    85,    85,    85,
      99,   100,    81,    99,   100,   104,   104,   104,   104,   104,
     104,   104,    99,   100,   104,    80,   104,    86,    86,    54,
      70,    79,    99,   100,   101,   104,   100,   104,    98,    80,
     104,    98,    99,   101,   104,   100,   100,   100,   100,    38,
      54,    79,   101,   104,   101,   104,    99,   100,    99,   100,
      86,    86,    86,    86,   101,   100,   101,    38,    54,    79,
      99,   101,   104,    99,   100,    99,   101,   101,   104,     7,
      81,    82,    99,    99,    85,    85,    85,    85,    85,    85,
      99,    19,    20,    21,    22,    23,    83,    86,    86,    86,
      99,    79,    80,    97,    98,    98,    99,   100,    99,   100,
      99,   100,    99,   100,    99,   100,    99,   100,    99,    99,
      99,   100,    99,   100,    99,   100,    99,   100,    99,   100,
      99,   100,    99,   100,    99,   100,    99,   100,    99,   100,
      99,   100,    99,   100,    99,   100,    99,   100,    99,   100,
      99,   100,    99,   100,    99,   100,   101,   101,   101,   101,
     101,   101,   101,    99,    26,    90,    96,    99,    90,    96,
       7,    82,   104,    98,     7,    99,    86,    86,    87,    87,
      86,    86,    86,    87,    86,    86,    86,    87,    96,    86,
      87,    85,    85,    85,    86,    86,    86,    86,    86,    86,
      87,    96,    86,    87,    86,    86,    86,    86,    86,    86,
      86,    86,    85,    85,    85,    86,    86,    86,    86,    87,
      87,    87,    87,    87,    86,    86,    85,    85,    85,    86,
      86,    86,    87,    86,    87,    86,    87,    87,    87,    86,
      87,    81,    99,    99,    99,   104,    98,    99,    99,     7,
      99,    99,    99,    99,    99,    91,    85,    96,    91,    99,
      83,    99,    81,    99,    87,    87,    81,    32,    99,   100,
     101,    98,    99,    98,    87,    98,    98,   101,     7,    98,
      99,   100,   101,    87,    98,   104,    98,     7,    99,   100,
      99,   100,    99,   100,    99,   100,   101,   104,    98,     7,
      99,   101,   101,    99,   101,    98,    99,    83,    87,    87,
      87,    87,    81,     7,    82,    91,    91,    99,    83,    99,
     100,    99,    99,    86,    86,    86,    86,    86,    86,    98,
      86,    87,    87,    81,    87,    87,    87,    87,    98,    86,
      87,    87,    81,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    87,    87,    87,    81,    86,    86,    86,    87,
      86,    86,    87,    87,    81,    99,    99,    99,    99,    99,
      81,    99,    87,    87,    29,    31,    86,   101,   101,    99,
      98,    99,   100,    99,   100,   101,    86,    99,    99,   101,
      99,   101,    98,    97,    98,    99,   100,   101,    99,    87,
      87,    99,    83,    99,   100,    97,    99,    87,    86,    86,
      86,    86,    86,    86,    87,    86,    86,    87,    86,    86,
      86,    86,    86,    86,    86,    99,    99,    87,    29,   101,
      97
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
# if YYLTYPE_IS_TRIVIAL
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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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
      case 3: /* "INT_VALUE" */

/* Line 1000 of yacc.c  */
#line 181 "../coda/libcoda/coda-expr-parser.y"
	{ free((yyvaluep->stringval)); };

/* Line 1000 of yacc.c  */
#line 2161 "libcoda/coda-expr-parser.c"
	break;
      case 4: /* "FLOAT_VALUE" */

/* Line 1000 of yacc.c  */
#line 181 "../coda/libcoda/coda-expr-parser.y"
	{ free((yyvaluep->stringval)); };

/* Line 1000 of yacc.c  */
#line 2170 "libcoda/coda-expr-parser.c"
	break;
      case 5: /* "STRING_VALUE" */

/* Line 1000 of yacc.c  */
#line 181 "../coda/libcoda/coda-expr-parser.y"
	{ free((yyvaluep->stringval)); };

/* Line 1000 of yacc.c  */
#line 2179 "libcoda/coda-expr-parser.c"
	break;
      case 6: /* "NAME" */

/* Line 1000 of yacc.c  */
#line 181 "../coda/libcoda/coda-expr-parser.y"
	{ free((yyvaluep->stringval)); };

/* Line 1000 of yacc.c  */
#line 2188 "libcoda/coda-expr-parser.c"
	break;
      case 96: /* "identifier" */

/* Line 1000 of yacc.c  */
#line 181 "../coda/libcoda/coda-expr-parser.y"
	{ free((yyvaluep->stringval)); };

/* Line 1000 of yacc.c  */
#line 2197 "libcoda/coda-expr-parser.c"
	break;
      case 97: /* "voidexpr" */

/* Line 1000 of yacc.c  */
#line 180 "../coda/libcoda/coda-expr-parser.y"
	{ coda_expression_delete((yyvaluep->expr)); };

/* Line 1000 of yacc.c  */
#line 2206 "libcoda/coda-expr-parser.c"
	break;
      case 98: /* "boolexpr" */

/* Line 1000 of yacc.c  */
#line 180 "../coda/libcoda/coda-expr-parser.y"
	{ coda_expression_delete((yyvaluep->expr)); };

/* Line 1000 of yacc.c  */
#line 2215 "libcoda/coda-expr-parser.c"
	break;
      case 99: /* "intexpr" */

/* Line 1000 of yacc.c  */
#line 180 "../coda/libcoda/coda-expr-parser.y"
	{ coda_expression_delete((yyvaluep->expr)); };

/* Line 1000 of yacc.c  */
#line 2224 "libcoda/coda-expr-parser.c"
	break;
      case 100: /* "floatexpr" */

/* Line 1000 of yacc.c  */
#line 180 "../coda/libcoda/coda-expr-parser.y"
	{ coda_expression_delete((yyvaluep->expr)); };

/* Line 1000 of yacc.c  */
#line 2233 "libcoda/coda-expr-parser.c"
	break;
      case 101: /* "stringexpr" */

/* Line 1000 of yacc.c  */
#line 180 "../coda/libcoda/coda-expr-parser.y"
	{ coda_expression_delete((yyvaluep->expr)); };

/* Line 1000 of yacc.c  */
#line 2242 "libcoda/coda-expr-parser.c"
	break;
      case 104: /* "node" */

/* Line 1000 of yacc.c  */
#line 180 "../coda/libcoda/coda-expr-parser.y"
	{ coda_expression_delete((yyvaluep->expr)); };

/* Line 1000 of yacc.c  */
#line 2251 "libcoda/coda-expr-parser.c"
	break;

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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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
        case 2:

/* Line 1455 of yacc.c  */
#line 191 "../coda/libcoda/coda-expr-parser.y"
    { parsed_expression = (yyvsp[(1) - (1)].expr); }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 192 "../coda/libcoda/coda-expr-parser.y"
    { parsed_expression = (yyvsp[(1) - (1)].expr); }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 193 "../coda/libcoda/coda-expr-parser.y"
    { parsed_expression = (yyvsp[(1) - (1)].expr); }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 194 "../coda/libcoda/coda-expr-parser.y"
    { parsed_expression = (yyvsp[(1) - (1)].expr); }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 195 "../coda/libcoda/coda-expr-parser.y"
    { parsed_expression = (yyvsp[(1) - (1)].expr); }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 196 "../coda/libcoda/coda-expr-parser.y"
    { parsed_expression = (yyvsp[(1) - (1)].expr); }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 199 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "r"; }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 200 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "asciiline"; }
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 201 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "do"; }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 202 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "for"; }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 203 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "step"; }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 204 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "to"; }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 205 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "nan"; }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 206 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "inf"; }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 207 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "true"; }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 208 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "false"; }
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 209 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "abs"; }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 210 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "add"; }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 211 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "all"; }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 212 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "bitoffset"; }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 213 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "bool"; }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 214 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "bytes"; }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 215 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "byteoffset"; }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 216 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "bytesize"; }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 217 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "ceil"; }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 218 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "count"; }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 219 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "exists"; }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 220 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "filename"; }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 221 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "filesize"; }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 222 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "float"; }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 223 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "floor"; }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 224 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "goto"; }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 225 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "if"; }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 226 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "index"; }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 227 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "int"; }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 228 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "isnan"; }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 229 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "isinf"; }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 230 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "isplusinf"; }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 231 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "ismininf"; }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 232 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "length"; }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 233 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "ltrim"; }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 234 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "numelements"; }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 235 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "max"; }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 236 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "min"; }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 237 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "productclass"; }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 238 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "productformat"; }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 239 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "producttype"; }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 240 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "productversion"; }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 241 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "regex"; }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 242 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "round"; }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 243 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "rtrim"; }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 244 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "str"; }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 245 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "strtime"; }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 246 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "substr"; }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 247 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "time"; }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 248 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "trim"; }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 249 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "unboundindex"; }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 250 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = "with"; }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 256 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.stringval) = strdup((yyvsp[(1) - (1)].stringval)); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 260 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_variable_set, (yyvsp[(2) - (4)].stringval), NULL, (yyvsp[(4) - (4)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 264 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_variable_set, (yyvsp[(2) - (7)].stringval), (yyvsp[(4) - (7)].expr), (yyvsp[(7) - (7)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 268 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_sequence, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 272 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_for, (yyvsp[(2) - (8)].stringval), (yyvsp[(4) - (8)].expr), (yyvsp[(6) - (8)].expr), NULL, (yyvsp[(8) - (8)].expr));
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 276 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_for, (yyvsp[(2) - (10)].stringval), (yyvsp[(4) - (10)].expr), (yyvsp[(6) - (10)].expr), (yyvsp[(8) - (10)].expr), (yyvsp[(10) - (10)].expr));
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 280 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 284 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_with, (yyvsp[(3) - (8)].stringval), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 291 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_constant_boolean, strdup("true"), NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 295 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_constant_boolean, strdup("false"), NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 299 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_logical_and, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 303 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_logical_or, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 307 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_not, NULL, (yyvsp[(2) - (2)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 311 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_greater_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 315 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_less_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 319 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 323 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_not_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 327 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_less, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 331 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_greater, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 335 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_greater_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 339 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_greater_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 343 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_greater_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 347 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_less_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 351 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_less_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 355 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_less_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 359 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 363 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 367 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 371 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_not_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 375 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_not_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 379 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_not_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 383 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_less, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 387 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_less, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 391 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_less, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 395 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_greater, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 399 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_greater, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 403 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_greater, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 407 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 411 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_not_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 415 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_less, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 419 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_greater, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 423 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_greater_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 427 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_less_equal, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 431 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 432 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_isnan, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 436 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_isinf, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 440 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_isplusinf, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 444 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_ismininf, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 448 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_regex, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 452 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_exists, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 456 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_array_exists, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 460 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_variable_exists, (yyvsp[(4) - (7)].stringval), (yyvsp[(6) - (7)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 464 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_array_all, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 468 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_if, NULL, (yyvsp[(3) - (8)].expr), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 472 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_with, (yyvsp[(3) - (8)].stringval), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 479 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_constant_integer, (yyvsp[(1) - (1)].stringval), NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 483 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = (yyvsp[(3) - (4)].expr);
        }
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 486 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_integer, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 490 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_integer, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 494 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_integer, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 498 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_variable_value, (yyvsp[(2) - (2)].stringval), NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 502 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_variable_value, (yyvsp[(2) - (5)].stringval), (yyvsp[(4) - (5)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 506 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_index_var, (yyvsp[(1) - (1)].stringval), NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 510 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_neg, NULL, (yyvsp[(2) - (2)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 514 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.expr) = (yyvsp[(2) - (2)].expr); }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 515 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_add, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 519 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_subtract, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 523 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_multiply, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 527 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_divide, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 531 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_modulo, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 535 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_and, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 539 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_or, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 543 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 544 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_abs, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 548 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_max, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 552 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_min, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 556 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_num_elements, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 560 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_array_count, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 564 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_array_add, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 568 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_length, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 572 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_length, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 576 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_bit_size, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 580 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_byte_size, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 584 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_product_version, NULL, NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 588 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_file_size, NULL, NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 592 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_bit_offset, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 596 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_byte_offset, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 600 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_index, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 604 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_array_index, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 608 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_variable_index, (yyvsp[(4) - (7)].stringval), (yyvsp[(6) - (7)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 612 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_if, NULL, (yyvsp[(3) - (8)].expr), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 616 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_unbound_array_index, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 620 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_unbound_array_index, NULL, (yyvsp[(3) - (8)].expr), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 624 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_with, (yyvsp[(3) - (8)].stringval), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 631 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_constant_float, (yyvsp[(1) - (1)].stringval), NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 635 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_constant_float, strdup("nan"), NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 639 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_constant_float, strdup("inf"), NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 643 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = (yyvsp[(3) - (4)].expr);
        }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 646 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_float, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 650 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_float, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 654 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_float, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 658 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_neg, NULL, (yyvsp[(2) - (2)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 662 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.expr) = (yyvsp[(2) - (2)].expr); }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 663 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_add, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 667 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_add, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 671 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_add, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 675 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_subtract, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 679 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_subtract, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 683 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_subtract, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 687 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_multiply, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 691 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_multiply, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 695 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_multiply, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 699 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_divide, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 703 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_divide, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 707 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_divide, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 711 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_modulo, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 715 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_modulo, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 719 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_modulo, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 723 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_power, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 727 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_power, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 731 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_power, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 735 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_power, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 739 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 740 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_abs, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 744 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_ceil, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 748 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_floor, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 752 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_round, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 756 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_max, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 760 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_max, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 764 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_max, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 768 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_min, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 772 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_min, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 776 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_min, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 780 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_time, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 784 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_array_add, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 788 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_if, NULL, (yyvsp[(3) - (8)].expr), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 792 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_if, NULL, (yyvsp[(3) - (8)].expr), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 796 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_if, NULL, (yyvsp[(3) - (8)].expr), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 800 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_with, (yyvsp[(3) - (8)].stringval), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 807 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_constant_string, (yyvsp[(1) - (1)].stringval), NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 811 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_constant_rawstring, (yyvsp[(2) - (2)].stringval), NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 815 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = (yyvsp[(3) - (4)].expr);
        }
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 818 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_string, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 822 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_string, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 826 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_string, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 830 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_bytes, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 834 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_bytes, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 838 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_add, NULL, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 842 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_substr, NULL, (yyvsp[(3) - (8)].expr), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 846 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_ltrim, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 850 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_rtrim, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 854 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_trim, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 858 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_array_add, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 862 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_regex, NULL, (yyvsp[(3) - (8)].expr), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 866 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_regex, NULL, (yyvsp[(3) - (8)].expr), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 870 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_product_class, NULL, NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 874 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_product_format, NULL, NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 878 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_product_type, NULL, NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 882 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_filename, NULL, NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 886 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_strtime, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 890 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_strtime, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 894 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_strtime, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 898 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_strtime, NULL, (yyvsp[(3) - (6)].expr), (yyvsp[(5) - (6)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 902 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_if, NULL, (yyvsp[(3) - (8)].expr), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 906 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_with, (yyvsp[(3) - (8)].stringval), (yyvsp[(5) - (8)].expr), (yyvsp[(7) - (8)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 913 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_root, NULL, NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 920 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_here, NULL, NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 924 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_begin, NULL, NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 928 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_parent, NULL, NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 932 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_asciiline, NULL, NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 936 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_field, (yyvsp[(2) - (2)].stringval), (yyvsp[(1) - (2)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 940 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_field, NULL, (yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 944 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_parent, NULL, (yyvsp[(1) - (3)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 948 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_field, (yyvsp[(3) - (3)].stringval), (yyvsp[(1) - (3)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 952 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_field, NULL, (yyvsp[(1) - (5)].expr), (yyvsp[(4) - (5)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 956 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_array_element, NULL, NULL, (yyvsp[(2) - (3)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 960 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_array_element, NULL, (yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 964 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_attribute, (yyvsp[(2) - (2)].stringval), NULL, NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 968 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_attribute, NULL, NULL, (yyvsp[(3) - (4)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 972 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_attribute, (yyvsp[(3) - (3)].stringval), (yyvsp[(1) - (3)].expr), NULL, NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 976 "../coda/libcoda/coda-expr-parser.y"
    {
            (yyval.expr) = coda_expression_new(expr_goto_attribute, NULL, (yyvsp[(1) - (5)].expr), (yyvsp[(4) - (5)].expr), NULL, NULL);
            if ((yyval.expr) == NULL) YYERROR;
        }
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 983 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); }
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 984 "../coda/libcoda/coda-expr-parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); }
    break;



/* Line 1455 of yacc.c  */
#line 4765 "libcoda/coda-expr-parser.c"
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
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
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



/* Line 1675 of yacc.c  */
#line 987 "../coda/libcoda/coda-expr-parser.y"


/* *INDENT-ON* */

LIBCODA_API int coda_expression_from_string(const char *exprstring, coda_expression **expr)
{
    void *bufstate;

    if (exprstring == NULL)
    {
        coda_set_error(CODA_ERROR_INVALID_ARGUMENT, "invalid expression string argument (%s:%u)", __FILE__, __LINE__);
        return -1;
    }
    if (expr == NULL)
    {
        coda_set_error(CODA_ERROR_INVALID_ARGUMENT, "invalid expression argument (%s:%u)", __FILE__, __LINE__);
        return -1;
    }

    coda_errno = 0;
    parsed_expression = NULL;
    bufstate = (void *)coda_expression__scan_string(exprstring);
    if (coda_expression_parse() != 0)
    {
        if (coda_errno == 0)
        {
            coda_set_error(CODA_ERROR_EXPRESSION, NULL);
        }
        coda_expression__delete_buffer(bufstate);
        return -1;
    }
    coda_expression__delete_buffer(bufstate);
    *expr = parsed_expression;

    return 0;
}

