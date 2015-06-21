
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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

/* Line 1676 of yacc.c  */
#line 96 "../coda/libcoda/coda-expr-parser.y"

    char *stringval;
    struct coda_expression_struct *expr;



/* Line 1676 of yacc.c  */
#line 201 "libcoda/coda-expr-parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


