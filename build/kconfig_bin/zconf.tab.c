/* A Bison parser, made by GNU Bison 3.7.6.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30706

/* Bison version string.  */
#define YYBISON_VERSION "3.7.6"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         zconfparse
#define yylex           zconflex
#define yyerror         zconferror
#define yydebug         zconfdebug
#define yynerrs         zconfnerrs
#define yylval          zconflval
#define yychar          zconfchar

/* First part of user prologue.  */

/*
 * Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>
 * Released under the terms of the GNU GPL v2.0.
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lkc.h"

#define printd(mask, fmt...) if (cdebug & (mask)) printf(fmt)

#define PRINTD		0x0001
#define DEBUG_PARSE	0x0002

int cdebug = PRINTD;

extern int zconflex(void);
static void zconfprint(const char *err, ...);
static void zconf_error(const char *err, ...);
static void zconferror(const char *err);
static bool zconf_endtoken(const struct kconf_id *id, int starttoken, int endtoken);

struct symbol *symbol_hash[SYMBOL_HASHSIZE];

static struct menu *current_menu, *current_entry;



# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int zconfdebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    T_MAINMENU = 258,              /* T_MAINMENU  */
    T_MENU = 259,                  /* T_MENU  */
    T_ENDMENU = 260,               /* T_ENDMENU  */
    T_SOURCE = 261,                /* T_SOURCE  */
    T_CHOICE = 262,                /* T_CHOICE  */
    T_ENDCHOICE = 263,             /* T_ENDCHOICE  */
    T_COMMENT = 264,               /* T_COMMENT  */
    T_CONFIG = 265,                /* T_CONFIG  */
    T_MENUCONFIG = 266,            /* T_MENUCONFIG  */
    T_HELP = 267,                  /* T_HELP  */
    T_HELPTEXT = 268,              /* T_HELPTEXT  */
    T_IF = 269,                    /* T_IF  */
    T_ENDIF = 270,                 /* T_ENDIF  */
    T_DEPENDS = 271,               /* T_DEPENDS  */
    T_OPTIONAL = 272,              /* T_OPTIONAL  */
    T_PROMPT = 273,                /* T_PROMPT  */
    T_TYPE = 274,                  /* T_TYPE  */
    T_DEFAULT = 275,               /* T_DEFAULT  */
    T_SELECT = 276,                /* T_SELECT  */
    T_RANGE = 277,                 /* T_RANGE  */
    T_VISIBLE = 278,               /* T_VISIBLE  */
    T_OPTION = 279,                /* T_OPTION  */
    T_ON = 280,                    /* T_ON  */
    T_WORD = 281,                  /* T_WORD  */
    T_WORD_QUOTE = 282,            /* T_WORD_QUOTE  */
    T_UNEQUAL = 283,               /* T_UNEQUAL  */
    T_LESS = 284,                  /* T_LESS  */
    T_LESS_EQUAL = 285,            /* T_LESS_EQUAL  */
    T_GREATER = 286,               /* T_GREATER  */
    T_GREATER_EQUAL = 287,         /* T_GREATER_EQUAL  */
    T_CLOSE_PAREN = 288,           /* T_CLOSE_PAREN  */
    T_OPEN_PAREN = 289,            /* T_OPEN_PAREN  */
    T_EOL = 290,                   /* T_EOL  */
    T_OR = 291,                    /* T_OR  */
    T_AND = 292,                   /* T_AND  */
    T_EQUAL = 293,                 /* T_EQUAL  */
    T_NOT = 294                    /* T_NOT  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{

	char *string;
	struct file *file;
	struct symbol *symbol;
	struct expr *expr;
	struct menu *menu;
	const struct kconf_id *id;


};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE zconflval;

int zconfparse (void);


/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_T_MAINMENU = 3,                 /* T_MAINMENU  */
  YYSYMBOL_T_MENU = 4,                     /* T_MENU  */
  YYSYMBOL_T_ENDMENU = 5,                  /* T_ENDMENU  */
  YYSYMBOL_T_SOURCE = 6,                   /* T_SOURCE  */
  YYSYMBOL_T_CHOICE = 7,                   /* T_CHOICE  */
  YYSYMBOL_T_ENDCHOICE = 8,                /* T_ENDCHOICE  */
  YYSYMBOL_T_COMMENT = 9,                  /* T_COMMENT  */
  YYSYMBOL_T_CONFIG = 10,                  /* T_CONFIG  */
  YYSYMBOL_T_MENUCONFIG = 11,              /* T_MENUCONFIG  */
  YYSYMBOL_T_HELP = 12,                    /* T_HELP  */
  YYSYMBOL_T_HELPTEXT = 13,                /* T_HELPTEXT  */
  YYSYMBOL_T_IF = 14,                      /* T_IF  */
  YYSYMBOL_T_ENDIF = 15,                   /* T_ENDIF  */
  YYSYMBOL_T_DEPENDS = 16,                 /* T_DEPENDS  */
  YYSYMBOL_T_OPTIONAL = 17,                /* T_OPTIONAL  */
  YYSYMBOL_T_PROMPT = 18,                  /* T_PROMPT  */
  YYSYMBOL_T_TYPE = 19,                    /* T_TYPE  */
  YYSYMBOL_T_DEFAULT = 20,                 /* T_DEFAULT  */
  YYSYMBOL_T_SELECT = 21,                  /* T_SELECT  */
  YYSYMBOL_T_RANGE = 22,                   /* T_RANGE  */
  YYSYMBOL_T_VISIBLE = 23,                 /* T_VISIBLE  */
  YYSYMBOL_T_OPTION = 24,                  /* T_OPTION  */
  YYSYMBOL_T_ON = 25,                      /* T_ON  */
  YYSYMBOL_T_WORD = 26,                    /* T_WORD  */
  YYSYMBOL_T_WORD_QUOTE = 27,              /* T_WORD_QUOTE  */
  YYSYMBOL_T_UNEQUAL = 28,                 /* T_UNEQUAL  */
  YYSYMBOL_T_LESS = 29,                    /* T_LESS  */
  YYSYMBOL_T_LESS_EQUAL = 30,              /* T_LESS_EQUAL  */
  YYSYMBOL_T_GREATER = 31,                 /* T_GREATER  */
  YYSYMBOL_T_GREATER_EQUAL = 32,           /* T_GREATER_EQUAL  */
  YYSYMBOL_T_CLOSE_PAREN = 33,             /* T_CLOSE_PAREN  */
  YYSYMBOL_T_OPEN_PAREN = 34,              /* T_OPEN_PAREN  */
  YYSYMBOL_T_EOL = 35,                     /* T_EOL  */
  YYSYMBOL_T_OR = 36,                      /* T_OR  */
  YYSYMBOL_T_AND = 37,                     /* T_AND  */
  YYSYMBOL_T_EQUAL = 38,                   /* T_EQUAL  */
  YYSYMBOL_T_NOT = 39,                     /* T_NOT  */
  YYSYMBOL_YYACCEPT = 40,                  /* $accept  */
  YYSYMBOL_input = 41,                     /* input  */
  YYSYMBOL_start = 42,                     /* start  */
  YYSYMBOL_stmt_list = 43,                 /* stmt_list  */
  YYSYMBOL_option_name = 44,               /* option_name  */
  YYSYMBOL_common_stmt = 45,               /* common_stmt  */
  YYSYMBOL_option_error = 46,              /* option_error  */
  YYSYMBOL_config_entry_start = 47,        /* config_entry_start  */
  YYSYMBOL_config_stmt = 48,               /* config_stmt  */
  YYSYMBOL_menuconfig_entry_start = 49,    /* menuconfig_entry_start  */
  YYSYMBOL_menuconfig_stmt = 50,           /* menuconfig_stmt  */
  YYSYMBOL_config_option_list = 51,        /* config_option_list  */
  YYSYMBOL_config_option = 52,             /* config_option  */
  YYSYMBOL_symbol_option = 53,             /* symbol_option  */
  YYSYMBOL_symbol_option_list = 54,        /* symbol_option_list  */
  YYSYMBOL_symbol_option_arg = 55,         /* symbol_option_arg  */
  YYSYMBOL_choice = 56,                    /* choice  */
  YYSYMBOL_choice_entry = 57,              /* choice_entry  */
  YYSYMBOL_choice_end = 58,                /* choice_end  */
  YYSYMBOL_choice_stmt = 59,               /* choice_stmt  */
  YYSYMBOL_choice_option_list = 60,        /* choice_option_list  */
  YYSYMBOL_choice_option = 61,             /* choice_option  */
  YYSYMBOL_choice_block = 62,              /* choice_block  */
  YYSYMBOL_if_entry = 63,                  /* if_entry  */
  YYSYMBOL_if_end = 64,                    /* if_end  */
  YYSYMBOL_if_stmt = 65,                   /* if_stmt  */
  YYSYMBOL_if_block = 66,                  /* if_block  */
  YYSYMBOL_mainmenu_stmt = 67,             /* mainmenu_stmt  */
  YYSYMBOL_menu = 68,                      /* menu  */
  YYSYMBOL_menu_entry = 69,                /* menu_entry  */
  YYSYMBOL_menu_end = 70,                  /* menu_end  */
  YYSYMBOL_menu_stmt = 71,                 /* menu_stmt  */
  YYSYMBOL_menu_block = 72,                /* menu_block  */
  YYSYMBOL_source_stmt = 73,               /* source_stmt  */
  YYSYMBOL_comment = 74,                   /* comment  */
  YYSYMBOL_comment_stmt = 75,              /* comment_stmt  */
  YYSYMBOL_help_start = 76,                /* help_start  */
  YYSYMBOL_help = 77,                      /* help  */
  YYSYMBOL_depends_list = 78,              /* depends_list  */
  YYSYMBOL_depends = 79,                   /* depends  */
  YYSYMBOL_visibility_list = 80,           /* visibility_list  */
  YYSYMBOL_visible = 81,                   /* visible  */
  YYSYMBOL_prompt_stmt_opt = 82,           /* prompt_stmt_opt  */
  YYSYMBOL_prompt = 83,                    /* prompt  */
  YYSYMBOL_end = 84,                       /* end  */
  YYSYMBOL_nl = 85,                        /* nl  */
  YYSYMBOL_if_expr = 86,                   /* if_expr  */
  YYSYMBOL_expr = 87,                      /* expr  */
  YYSYMBOL_symbol = 88,                    /* symbol  */
  YYSYMBOL_word_opt = 89                   /* word_opt  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


/* Second part of user prologue.  */

/* Include zconf.hash.c here so it can see the token constants. */
#include "zconf.hash.c"



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   298

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  50
/* YYNRULES -- Number of rules.  */
#define YYNRULES  122
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  199

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   294


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   108,   108,   108,   110,   110,   112,   114,   115,   116,
     117,   118,   119,   123,   127,   127,   127,   127,   127,   127,
     127,   127,   131,   132,   133,   134,   135,   136,   140,   141,
     147,   155,   161,   169,   179,   181,   182,   183,   184,   185,
     186,   189,   197,   203,   213,   219,   225,   228,   230,   241,
     242,   247,   256,   261,   269,   272,   274,   275,   276,   277,
     278,   281,   287,   298,   304,   314,   316,   321,   329,   337,
     340,   342,   343,   344,   349,   356,   363,   368,   376,   379,
     381,   382,   383,   386,   394,   401,   408,   414,   421,   423,
     424,   425,   428,   436,   438,   439,   442,   449,   451,   456,
     457,   460,   461,   462,   466,   467,   470,   471,   474,   475,
     476,   477,   478,   479,   480,   481,   482,   483,   484,   487,
     488,   491,   492
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "T_MAINMENU", "T_MENU",
  "T_ENDMENU", "T_SOURCE", "T_CHOICE", "T_ENDCHOICE", "T_COMMENT",
  "T_CONFIG", "T_MENUCONFIG", "T_HELP", "T_HELPTEXT", "T_IF", "T_ENDIF",
  "T_DEPENDS", "T_OPTIONAL", "T_PROMPT", "T_TYPE", "T_DEFAULT", "T_SELECT",
  "T_RANGE", "T_VISIBLE", "T_OPTION", "T_ON", "T_WORD", "T_WORD_QUOTE",
  "T_UNEQUAL", "T_LESS", "T_LESS_EQUAL", "T_GREATER", "T_GREATER_EQUAL",
  "T_CLOSE_PAREN", "T_OPEN_PAREN", "T_EOL", "T_OR", "T_AND", "T_EQUAL",
  "T_NOT", "$accept", "input", "start", "stmt_list", "option_name",
  "common_stmt", "option_error", "config_entry_start", "config_stmt",
  "menuconfig_entry_start", "menuconfig_stmt", "config_option_list",
  "config_option", "symbol_option", "symbol_option_list",
  "symbol_option_arg", "choice", "choice_entry", "choice_end",
  "choice_stmt", "choice_option_list", "choice_option", "choice_block",
  "if_entry", "if_end", "if_stmt", "if_block", "mainmenu_stmt", "menu",
  "menu_entry", "menu_end", "menu_stmt", "menu_block", "source_stmt",
  "comment", "comment_stmt", "help_start", "help", "depends_list",
  "depends", "visibility_list", "visible", "prompt_stmt_opt", "prompt",
  "end", "nl", "if_expr", "expr", "symbol", "word_opt", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294
};
#endif

#define YYPACT_NINF (-91)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-86)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      19,    37,   -91,    13,   -91,    79,   -91,    20,   -91,   -91,
     -16,   -91,    21,    37,    25,    37,    41,    36,    37,    78,
      83,    31,    56,   -91,   -91,   -91,   -91,   -91,   -91,   -91,
     -91,   116,   -91,   127,   -91,   -91,   -91,   -91,   -91,   -91,
     -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,
     -91,   147,   -91,   -91,   105,   -91,   109,   -91,   111,   -91,
     114,   -91,   136,   137,   142,   -91,   -91,    31,    31,    76,
     254,   -91,   143,   146,    27,   115,   207,   258,   243,   -14,
     243,   179,   -91,   -91,   -91,   -91,   -91,   -91,    -7,   -91,
      31,    31,   105,    51,    51,    51,    51,    51,    51,   -91,
     -91,   156,   168,   181,    37,    37,    31,   178,    51,   -91,
     206,   -91,   -91,   -91,   -91,   196,   -91,   -91,   175,    37,
      37,   185,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,
     -91,   -91,   -91,   -91,   -91,   214,   -91,   230,   -91,   -91,
     -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   183,   -91,
     -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,    31,   214,
     194,   214,    45,   214,    51,    26,   195,   -91,   -91,   214,
     197,   214,    31,   -91,   139,   208,   -91,   -91,   220,   224,
     214,   222,   -91,   -91,   226,   -91,   227,   123,   -91,   -91,
     -91,   -91,   235,    37,   -91,   -91,   -91,   -91,   -91
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       6,     0,   104,     0,     3,     0,     6,     6,    99,   100,
       0,     1,     0,     0,     0,     0,   121,     0,     0,     0,
       0,     0,     0,    14,    18,    15,    16,    20,    17,    19,
      21,     0,    22,     0,     7,    34,    25,    34,    26,    55,
      65,     8,    70,    23,    93,    79,     9,    27,    88,    24,
      10,     0,   105,     2,    74,    13,     0,   101,     0,   122,
       0,   102,     0,     0,     0,   119,   120,     0,     0,     0,
     108,   103,     0,     0,     0,     0,     0,     0,     0,    88,
       0,     0,    75,    83,    51,    84,    30,    32,     0,   116,
       0,     0,    67,     0,     0,     0,     0,     0,     0,    11,
      12,     0,     0,     0,     0,    97,     0,     0,     0,    47,
       0,    40,    39,    35,    36,     0,    38,    37,     0,     0,
      97,     0,    59,    60,    56,    58,    57,    66,    54,    53,
      71,    73,    69,    72,    68,   106,    95,     0,    94,    80,
      82,    78,    81,    77,    90,    91,    89,   115,   117,   118,
     114,   109,   110,   111,   112,   113,    29,    86,     0,   106,
       0,   106,   106,   106,     0,     0,     0,    87,    63,   106,
       0,   106,     0,    96,     0,     0,    41,    98,     0,     0,
     106,    49,    46,    28,     0,    62,     0,   107,    92,    42,
      43,    44,     0,     0,    48,    61,    64,    45,    50
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -91,   -91,   264,   268,   -91,    30,   -65,   -91,   -91,   -91,
     -91,   238,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   -12,
     -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,
     -91,    -5,   -91,   -91,   -91,   -91,   -91,   200,   209,   -61,
     -91,   -91,   170,    -1,    65,     0,   118,   -66,   -90,   -91
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     3,     4,     5,    33,    34,   112,    35,    36,    37,
      38,    74,   113,   114,   165,   194,    39,    40,   128,    41,
      76,   124,    77,    42,   132,    43,    78,     6,    44,    45,
     141,    46,    80,    47,    48,    49,   115,   116,    81,   117,
      79,   138,   160,   161,    50,     7,   173,    69,    70,    60
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      10,    88,    89,   150,   151,   152,   153,   154,   155,   135,
      54,   123,    56,    11,    58,   126,   145,    62,   164,     2,
     146,   136,     1,     1,   148,   149,   147,   -31,   101,    90,
      91,   -31,   -31,   -31,   -31,   -31,   -31,   -31,   -31,   102,
     162,   -31,   -31,   103,   -31,   104,   105,   106,   107,   108,
     -31,   109,   181,   110,     2,    52,    55,    65,    66,   172,
      57,   182,   111,     8,     9,    67,   131,    59,   140,    92,
      68,    61,   145,   133,   180,   142,   146,    65,    66,    -5,
      12,    90,    91,    13,    14,    15,    16,    17,    18,    19,
      20,    71,   174,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,   159,    63,    31,   187,   127,   130,    64,
     139,     2,    90,    91,    32,   -33,   101,    72,   169,   -33,
     -33,   -33,   -33,   -33,   -33,   -33,   -33,   102,    73,   -33,
     -33,   103,   -33,   104,   105,   106,   107,   108,   -33,   109,
      52,   110,   129,   134,    82,   143,    83,    -4,    12,    84,
     111,    13,    14,    15,    16,    17,    18,    19,    20,    90,
      91,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    85,    86,    31,   188,    90,    91,    87,    99,   -85,
     101,   100,    32,   -85,   -85,   -85,   -85,   -85,   -85,   -85,
     -85,   156,   198,   -85,   -85,   103,   -85,   -85,   -85,   -85,
     -85,   -85,   -85,   157,   163,   110,   158,   166,   101,   167,
     168,   171,   -52,   -52,   144,   -52,   -52,   -52,   -52,   102,
      91,   -52,   -52,   103,   118,   119,   120,   121,   172,   176,
     183,   101,   185,   110,   -76,   -76,   -76,   -76,   -76,   -76,
     -76,   -76,   122,   189,   -76,   -76,   103,    13,    14,    15,
      16,    17,    18,    19,    20,   190,   110,    21,    22,   191,
     193,   195,   196,    14,    15,   144,    17,    18,    19,    20,
     197,    53,    21,    22,    51,    75,   125,   175,    32,   177,
     178,   179,    93,    94,    95,    96,    97,   184,   137,   186,
     170,     0,    98,    32,     0,     0,     0,     0,   192
};

static const yytype_int16 yycheck[] =
{
       1,    67,    68,    93,    94,    95,    96,    97,    98,    23,
      10,    76,    13,     0,    15,    76,    81,    18,   108,    35,
      81,    35,     3,     3,    90,    91,    33,     0,     1,    36,
      37,     4,     5,     6,     7,     8,     9,    10,    11,    12,
     106,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    26,    26,    35,    35,    35,    26,    27,    14,
      35,    35,    35,    26,    27,    34,    78,    26,    80,    69,
      39,    35,   137,    78,   164,    80,   137,    26,    27,     0,
       1,    36,    37,     4,     5,     6,     7,     8,     9,    10,
      11,    35,   158,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,   104,    26,    26,   172,    77,    78,    26,
      80,    35,    36,    37,    35,     0,     1,     1,   119,     4,
       5,     6,     7,     8,     9,    10,    11,    12,     1,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      35,    26,    77,    78,    35,    80,    35,     0,     1,    35,
      35,     4,     5,     6,     7,     8,     9,    10,    11,    36,
      37,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    35,    35,    26,    35,    36,    37,    35,    35,     0,
       1,    35,    35,     4,     5,     6,     7,     8,     9,    10,
      11,    35,   193,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    35,    26,    26,    25,     1,     1,    13,
      35,    26,     5,     6,    35,     8,     9,    10,    11,    12,
      37,    14,    15,    16,    17,    18,    19,    20,    14,    35,
      35,     1,    35,    26,     4,     5,     6,     7,     8,     9,
      10,    11,    35,    35,    14,    15,    16,     4,     5,     6,
       7,     8,     9,    10,    11,    35,    26,    14,    15,    35,
      38,    35,    35,     5,     6,    35,     8,     9,    10,    11,
      35,     7,    14,    15,     6,    37,    76,   159,    35,   161,
     162,   163,    28,    29,    30,    31,    32,   169,    79,   171,
     120,    -1,    38,    35,    -1,    -1,    -1,    -1,   180
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    35,    41,    42,    43,    67,    85,    26,    27,
      83,     0,     1,     4,     5,     6,     7,     8,     9,    10,
      11,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    26,    35,    44,    45,    47,    48,    49,    50,    56,
      57,    59,    63,    65,    68,    69,    71,    73,    74,    75,
      84,    43,    35,    42,    85,    35,    83,    35,    83,    26,
      89,    35,    83,    26,    26,    26,    27,    34,    39,    87,
      88,    35,     1,     1,    51,    51,    60,    62,    66,    80,
      72,    78,    35,    35,    35,    35,    35,    35,    87,    87,
      36,    37,    85,    28,    29,    30,    31,    32,    38,    35,
      35,     1,    12,    16,    18,    19,    20,    21,    22,    24,
      26,    35,    46,    52,    53,    76,    77,    79,    17,    18,
      19,    20,    35,    46,    61,    77,    79,    45,    58,    84,
      45,    59,    64,    71,    84,    23,    35,    78,    81,    45,
      59,    70,    71,    84,    35,    46,    79,    33,    87,    87,
      88,    88,    88,    88,    88,    88,    35,    35,    25,    83,
      82,    83,    87,    26,    88,    54,     1,    13,    35,    83,
      82,    26,    14,    86,    87,    86,    35,    86,    86,    86,
      88,    26,    35,    35,    86,    35,    86,    87,    35,    35,
      35,    35,    86,    38,    55,    35,    35,    35,    83
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    40,    41,    41,    42,    42,    43,    43,    43,    43,
      43,    43,    43,    43,    44,    44,    44,    44,    44,    44,
      44,    44,    45,    45,    45,    45,    45,    45,    46,    46,
      47,    48,    49,    50,    51,    51,    51,    51,    51,    51,
      51,    52,    52,    52,    52,    52,    53,    54,    54,    55,
      55,    56,    57,    58,    59,    60,    60,    60,    60,    60,
      60,    61,    61,    61,    61,    62,    62,    63,    64,    65,
      66,    66,    66,    66,    67,    68,    69,    70,    71,    72,
      72,    72,    72,    73,    74,    75,    76,    77,    78,    78,
      78,    78,    79,    80,    80,    80,    81,    82,    82,    83,
      83,    84,    84,    84,    85,    85,    86,    86,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    88,
      88,    89,    89
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     2,     1,     0,     2,     2,     2,
       2,     4,     4,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       3,     2,     3,     2,     0,     2,     2,     2,     2,     2,
       2,     3,     4,     4,     4,     5,     3,     0,     3,     0,
       2,     3,     2,     1,     3,     0,     2,     2,     2,     2,
       2,     4,     3,     2,     4,     0,     2,     3,     1,     3,
       0,     2,     2,     2,     3,     3,     3,     1,     3,     0,
       2,     2,     2,     3,     3,     2,     2,     2,     0,     2,
       2,     2,     4,     0,     2,     2,     2,     0,     2,     1,
       1,     2,     2,     2,     1,     2,     0,     2,     1,     3,
       3,     3,     3,     3,     3,     3,     2,     3,     3,     1,
       1,     0,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_choice_entry: /* choice_entry  */
            {
	fprintf(stderr, "%s:%d: missing end statement for this entry\n",
		((*yyvaluep).menu)->file->name, ((*yyvaluep).menu)->lineno);
	if (current_menu == ((*yyvaluep).menu))
		menu_end_menu();
}
        break;

    case YYSYMBOL_if_entry: /* if_entry  */
            {
	fprintf(stderr, "%s:%d: missing end statement for this entry\n",
		((*yyvaluep).menu)->file->name, ((*yyvaluep).menu)->lineno);
	if (current_menu == ((*yyvaluep).menu))
		menu_end_menu();
}
        break;

    case YYSYMBOL_menu_entry: /* menu_entry  */
            {
	fprintf(stderr, "%s:%d: missing end statement for this entry\n",
		((*yyvaluep).menu)->file->name, ((*yyvaluep).menu)->lineno);
	if (current_menu == ((*yyvaluep).menu))
		menu_end_menu();
}
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 10: /* stmt_list: stmt_list end  */
                                        { zconf_error("unexpected end statement"); }
    break;

  case 11: /* stmt_list: stmt_list T_WORD error T_EOL  */
                                        { zconf_error("unknown statement \"%s\"", (yyvsp[-2].string)); }
    break;

  case 12: /* stmt_list: stmt_list option_name error T_EOL  */
{
	zconf_error("unexpected option \"%s\"", kconf_id_strings + (yyvsp[-2].id)->name);
}
    break;

  case 13: /* stmt_list: stmt_list error T_EOL  */
                                        { zconf_error("invalid statement"); }
    break;

  case 28: /* option_error: T_WORD error T_EOL  */
                                        { zconf_error("unknown option \"%s\"", (yyvsp[-2].string)); }
    break;

  case 29: /* option_error: error T_EOL  */
                                        { zconf_error("invalid option"); }
    break;

  case 30: /* config_entry_start: T_CONFIG T_WORD T_EOL  */
{
	struct symbol *sym = sym_lookup((yyvsp[-1].string), 0);
	sym->flags |= SYMBOL_OPTIONAL;
	menu_add_entry(sym);
	printd(DEBUG_PARSE, "%s:%d:config %s\n", zconf_curname(), zconf_lineno(), (yyvsp[-1].string));
}
    break;

  case 31: /* config_stmt: config_entry_start config_option_list  */
{
	menu_end_entry();
	printd(DEBUG_PARSE, "%s:%d:endconfig\n", zconf_curname(), zconf_lineno());
}
    break;

  case 32: /* menuconfig_entry_start: T_MENUCONFIG T_WORD T_EOL  */
{
	struct symbol *sym = sym_lookup((yyvsp[-1].string), 0);
	sym->flags |= SYMBOL_OPTIONAL;
	menu_add_entry(sym);
	printd(DEBUG_PARSE, "%s:%d:menuconfig %s\n", zconf_curname(), zconf_lineno(), (yyvsp[-1].string));
}
    break;

  case 33: /* menuconfig_stmt: menuconfig_entry_start config_option_list  */
{
	if (current_entry->prompt)
		current_entry->prompt->type = P_MENU;
	else
		zconfprint("warning: menuconfig statement without prompt");
	menu_end_entry();
	printd(DEBUG_PARSE, "%s:%d:endconfig\n", zconf_curname(), zconf_lineno());
}
    break;

  case 41: /* config_option: T_TYPE prompt_stmt_opt T_EOL  */
{
	menu_set_type((yyvsp[-2].id)->stype);
	printd(DEBUG_PARSE, "%s:%d:type(%u)\n",
		zconf_curname(), zconf_lineno(),
		(yyvsp[-2].id)->stype);
}
    break;

  case 42: /* config_option: T_PROMPT prompt if_expr T_EOL  */
{
	menu_add_prompt(P_PROMPT, (yyvsp[-2].string), (yyvsp[-1].expr));
	printd(DEBUG_PARSE, "%s:%d:prompt\n", zconf_curname(), zconf_lineno());
}
    break;

  case 43: /* config_option: T_DEFAULT expr if_expr T_EOL  */
{
	menu_add_expr(P_DEFAULT, (yyvsp[-2].expr), (yyvsp[-1].expr));
	if ((yyvsp[-3].id)->stype != S_UNKNOWN)
		menu_set_type((yyvsp[-3].id)->stype);
	printd(DEBUG_PARSE, "%s:%d:default(%u)\n",
		zconf_curname(), zconf_lineno(),
		(yyvsp[-3].id)->stype);
}
    break;

  case 44: /* config_option: T_SELECT T_WORD if_expr T_EOL  */
{
	menu_add_symbol(P_SELECT, sym_lookup((yyvsp[-2].string), 0), (yyvsp[-1].expr));
	printd(DEBUG_PARSE, "%s:%d:select\n", zconf_curname(), zconf_lineno());
}
    break;

  case 45: /* config_option: T_RANGE symbol symbol if_expr T_EOL  */
{
	menu_add_expr(P_RANGE, expr_alloc_comp(E_RANGE,(yyvsp[-3].symbol), (yyvsp[-2].symbol)), (yyvsp[-1].expr));
	printd(DEBUG_PARSE, "%s:%d:range\n", zconf_curname(), zconf_lineno());
}
    break;

  case 48: /* symbol_option_list: symbol_option_list T_WORD symbol_option_arg  */
{
	const struct kconf_id *id = kconf_id_lookup((yyvsp[-1].string), strlen((yyvsp[-1].string)));
	if (id && id->flags & TF_OPTION)
		menu_add_option(id->token, (yyvsp[0].string));
	else
		zconfprint("warning: ignoring unknown option %s", (yyvsp[-1].string));
	free((yyvsp[-1].string));
}
    break;

  case 49: /* symbol_option_arg: %empty  */
                                { (yyval.string) = NULL; }
    break;

  case 50: /* symbol_option_arg: T_EQUAL prompt  */
                                { (yyval.string) = (yyvsp[0].string); }
    break;

  case 51: /* choice: T_CHOICE word_opt T_EOL  */
{
	struct symbol *sym = sym_lookup((yyvsp[-1].string), SYMBOL_CHOICE);
	sym->flags |= SYMBOL_AUTO;
	menu_add_entry(sym);
	menu_add_expr(P_CHOICE, NULL, NULL);
	printd(DEBUG_PARSE, "%s:%d:choice\n", zconf_curname(), zconf_lineno());
}
    break;

  case 52: /* choice_entry: choice choice_option_list  */
{
	(yyval.menu) = menu_add_menu();
}
    break;

  case 53: /* choice_end: end  */
{
	if (zconf_endtoken((yyvsp[0].id), T_CHOICE, T_ENDCHOICE)) {
		menu_end_menu();
		printd(DEBUG_PARSE, "%s:%d:endchoice\n", zconf_curname(), zconf_lineno());
	}
}
    break;

  case 61: /* choice_option: T_PROMPT prompt if_expr T_EOL  */
{
	menu_add_prompt(P_PROMPT, (yyvsp[-2].string), (yyvsp[-1].expr));
	printd(DEBUG_PARSE, "%s:%d:prompt\n", zconf_curname(), zconf_lineno());
}
    break;

  case 62: /* choice_option: T_TYPE prompt_stmt_opt T_EOL  */
{
	if ((yyvsp[-2].id)->stype == S_BOOLEAN || (yyvsp[-2].id)->stype == S_TRISTATE) {
		menu_set_type((yyvsp[-2].id)->stype);
		printd(DEBUG_PARSE, "%s:%d:type(%u)\n",
			zconf_curname(), zconf_lineno(),
			(yyvsp[-2].id)->stype);
	} else
		YYERROR;
}
    break;

  case 63: /* choice_option: T_OPTIONAL T_EOL  */
{
	current_entry->sym->flags |= SYMBOL_OPTIONAL;
	printd(DEBUG_PARSE, "%s:%d:optional\n", zconf_curname(), zconf_lineno());
}
    break;

  case 64: /* choice_option: T_DEFAULT T_WORD if_expr T_EOL  */
{
	if ((yyvsp[-3].id)->stype == S_UNKNOWN) {
		menu_add_symbol(P_DEFAULT, sym_lookup((yyvsp[-2].string), 0), (yyvsp[-1].expr));
		printd(DEBUG_PARSE, "%s:%d:default\n",
			zconf_curname(), zconf_lineno());
	} else
		YYERROR;
}
    break;

  case 67: /* if_entry: T_IF expr nl  */
{
	printd(DEBUG_PARSE, "%s:%d:if\n", zconf_curname(), zconf_lineno());
	menu_add_entry(NULL);
	menu_add_dep((yyvsp[-1].expr));
	(yyval.menu) = menu_add_menu();
}
    break;

  case 68: /* if_end: end  */
{
	if (zconf_endtoken((yyvsp[0].id), T_IF, T_ENDIF)) {
		menu_end_menu();
		printd(DEBUG_PARSE, "%s:%d:endif\n", zconf_curname(), zconf_lineno());
	}
}
    break;

  case 74: /* mainmenu_stmt: T_MAINMENU prompt nl  */
{
	menu_add_prompt(P_MENU, (yyvsp[-1].string), NULL);
}
    break;

  case 75: /* menu: T_MENU prompt T_EOL  */
{
	menu_add_entry(NULL);
	menu_add_prompt(P_MENU, (yyvsp[-1].string), NULL);
	printd(DEBUG_PARSE, "%s:%d:menu\n", zconf_curname(), zconf_lineno());
}
    break;

  case 76: /* menu_entry: menu visibility_list depends_list  */
{
	(yyval.menu) = menu_add_menu();
}
    break;

  case 77: /* menu_end: end  */
{
	if (zconf_endtoken((yyvsp[0].id), T_MENU, T_ENDMENU)) {
		menu_end_menu();
		printd(DEBUG_PARSE, "%s:%d:endmenu\n", zconf_curname(), zconf_lineno());
	}
}
    break;

  case 83: /* source_stmt: T_SOURCE prompt T_EOL  */
{
	printd(DEBUG_PARSE, "%s:%d:source %s\n", zconf_curname(), zconf_lineno(), (yyvsp[-1].string));
	zconf_nextfiles((yyvsp[-1].string));
}
    break;

  case 84: /* comment: T_COMMENT prompt T_EOL  */
{
	menu_add_entry(NULL);
	menu_add_prompt(P_COMMENT, (yyvsp[-1].string), NULL);
	printd(DEBUG_PARSE, "%s:%d:comment\n", zconf_curname(), zconf_lineno());
}
    break;

  case 85: /* comment_stmt: comment depends_list  */
{
	menu_end_entry();
}
    break;

  case 86: /* help_start: T_HELP T_EOL  */
{
	printd(DEBUG_PARSE, "%s:%d:help\n", zconf_curname(), zconf_lineno());
	zconf_starthelp();
}
    break;

  case 87: /* help: help_start T_HELPTEXT  */
{
	current_entry->help = (yyvsp[0].string);
}
    break;

  case 92: /* depends: T_DEPENDS T_ON expr T_EOL  */
{
	menu_add_dep((yyvsp[-1].expr));
	printd(DEBUG_PARSE, "%s:%d:depends on\n", zconf_curname(), zconf_lineno());
}
    break;

  case 96: /* visible: T_VISIBLE if_expr  */
{
	menu_add_visibility((yyvsp[0].expr));
}
    break;

  case 98: /* prompt_stmt_opt: prompt if_expr  */
{
	menu_add_prompt(P_PROMPT, (yyvsp[-1].string), (yyvsp[0].expr));
}
    break;

  case 101: /* end: T_ENDMENU T_EOL  */
                                { (yyval.id) = (yyvsp[-1].id); }
    break;

  case 102: /* end: T_ENDCHOICE T_EOL  */
                                { (yyval.id) = (yyvsp[-1].id); }
    break;

  case 103: /* end: T_ENDIF T_EOL  */
                                { (yyval.id) = (yyvsp[-1].id); }
    break;

  case 106: /* if_expr: %empty  */
                                        { (yyval.expr) = NULL; }
    break;

  case 107: /* if_expr: T_IF expr  */
                                        { (yyval.expr) = (yyvsp[0].expr); }
    break;

  case 108: /* expr: symbol  */
                                                { (yyval.expr) = expr_alloc_symbol((yyvsp[0].symbol)); }
    break;

  case 109: /* expr: symbol T_LESS symbol  */
                                                { (yyval.expr) = expr_alloc_comp(E_LTH, (yyvsp[-2].symbol), (yyvsp[0].symbol)); }
    break;

  case 110: /* expr: symbol T_LESS_EQUAL symbol  */
                                                { (yyval.expr) = expr_alloc_comp(E_LEQ, (yyvsp[-2].symbol), (yyvsp[0].symbol)); }
    break;

  case 111: /* expr: symbol T_GREATER symbol  */
                                                { (yyval.expr) = expr_alloc_comp(E_GTH, (yyvsp[-2].symbol), (yyvsp[0].symbol)); }
    break;

  case 112: /* expr: symbol T_GREATER_EQUAL symbol  */
                                                { (yyval.expr) = expr_alloc_comp(E_GEQ, (yyvsp[-2].symbol), (yyvsp[0].symbol)); }
    break;

  case 113: /* expr: symbol T_EQUAL symbol  */
                                                { (yyval.expr) = expr_alloc_comp(E_EQUAL, (yyvsp[-2].symbol), (yyvsp[0].symbol)); }
    break;

  case 114: /* expr: symbol T_UNEQUAL symbol  */
                                                { (yyval.expr) = expr_alloc_comp(E_UNEQUAL, (yyvsp[-2].symbol), (yyvsp[0].symbol)); }
    break;

  case 115: /* expr: T_OPEN_PAREN expr T_CLOSE_PAREN  */
                                                { (yyval.expr) = (yyvsp[-1].expr); }
    break;

  case 116: /* expr: T_NOT expr  */
                                                { (yyval.expr) = expr_alloc_one(E_NOT, (yyvsp[0].expr)); }
    break;

  case 117: /* expr: expr T_OR expr  */
                                                { (yyval.expr) = expr_alloc_two(E_OR, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 118: /* expr: expr T_AND expr  */
                                                { (yyval.expr) = expr_alloc_two(E_AND, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 119: /* symbol: T_WORD  */
                        { (yyval.symbol) = sym_lookup((yyvsp[0].string), 0); free((yyvsp[0].string)); }
    break;

  case 120: /* symbol: T_WORD_QUOTE  */
                        { (yyval.symbol) = sym_lookup((yyvsp[0].string), SYMBOL_CONST); free((yyvsp[0].string)); }
    break;

  case 121: /* word_opt: %empty  */
                                        { (yyval.string) = NULL; }
    break;



      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

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


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}



void conf_parse(const char *name)
{
	struct symbol *sym;
	int i;

	zconf_initscan(name);

	sym_init();
	_menu_init();
	rootmenu.prompt = menu_add_prompt(P_MENU, "Linux Kernel Configuration", NULL);

	if (getenv("ZCONF_DEBUG"))
		zconfdebug = 1;
	zconfparse();
	if (zconfnerrs)
		exit(1);
	if (!modules_sym)
		modules_sym = sym_find( "n" );

	rootmenu.prompt->text = _(rootmenu.prompt->text);
	rootmenu.prompt->text = sym_expand_string_value(rootmenu.prompt->text);

	menu_finalize(&rootmenu);
	for_all_symbols(i, sym) {
		if (sym_check_deps(sym))
			zconfnerrs++;
	}
	if (zconfnerrs)
		exit(1);
	sym_set_change_count(1);
}

static const char *zconf_tokenname(int token)
{
	switch (token) {
	case T_MENU:		return "menu";
	case T_ENDMENU:		return "endmenu";
	case T_CHOICE:		return "choice";
	case T_ENDCHOICE:	return "endchoice";
	case T_IF:		return "if";
	case T_ENDIF:		return "endif";
	case T_DEPENDS:		return "depends";
	case T_VISIBLE:		return "visible";
	}
	return "<token>";
}

static bool zconf_endtoken(const struct kconf_id *id, int starttoken, int endtoken)
{
	if (id->token != endtoken) {
		zconf_error("unexpected '%s' within %s block",
			kconf_id_strings + id->name, zconf_tokenname(starttoken));
		zconfnerrs++;
		return false;
	}
#if 0
//Wildcard breaks this somehow, so disabled for now. -JD
	if (current_menu->file != current_file) {
		zconf_error("'%s' in different file than '%s'",
			kconf_id_strings + id->name, zconf_tokenname(starttoken));
		fprintf(stderr, "%s:%d: location of the '%s'\n",
			current_menu->file->name, current_menu->lineno,
			zconf_tokenname(starttoken));
		zconfnerrs++;
		return false;
	}
#endif
	return true;
}

static void zconfprint(const char *err, ...)
{
	va_list ap;

	fprintf(stderr, "%s:%d: ", zconf_curname(), zconf_lineno());
	va_start(ap, err);
	vfprintf(stderr, err, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

static void zconf_error(const char *err, ...)
{
	va_list ap;

	zconfnerrs++;
	fprintf(stderr, "%s:%d: ", zconf_curname(), zconf_lineno());
	va_start(ap, err);
	vfprintf(stderr, err, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

static void zconferror(const char *err)
{
	fprintf(stderr, "%s:%d: %s\n", zconf_curname(), zconf_lineno() + 1, err);
}

static void print_quoted_string(FILE *out, const char *str)
{
	const char *p;
	int len;

	putc('"', out);
	while ((p = strchr(str, '"'))) {
		len = p - str;
		if (len)
			fprintf(out, "%.*s", len, str);
		fputs("\\\"", out);
		str = p + 1;
	}
	fputs(str, out);
	putc('"', out);
}

static void print_symbol(FILE *out, struct menu *menu)
{
	struct symbol *sym = menu->sym;
	struct property *prop;

	if (sym_is_choice(sym))
		fprintf(out, "\nchoice\n");
	else
		fprintf(out, "\nconfig %s\n", sym->name);
	switch (sym->type) {
	case S_BOOLEAN:
		fputs("  boolean\n", out);
		break;
	case S_TRISTATE:
		fputs("  tristate\n", out);
		break;
	case S_STRING:
		fputs("  string\n", out);
		break;
	case S_INT:
		fputs("  integer\n", out);
		break;
	case S_HEX:
		fputs("  hex\n", out);
		break;
	default:
		fputs("  ???\n", out);
		break;
	}
	for (prop = sym->prop; prop; prop = prop->next) {
		if (prop->menu != menu)
			continue;
		switch (prop->type) {
		case P_PROMPT:
			fputs("  prompt ", out);
			print_quoted_string(out, prop->text);
			if (!expr_is_yes(prop->visible.expr)) {
				fputs(" if ", out);
				expr_fprint(prop->visible.expr, out);
			}
			fputc('\n', out);
			break;
		case P_DEFAULT:
			fputs( "  default ", out);
			expr_fprint(prop->expr, out);
			if (!expr_is_yes(prop->visible.expr)) {
				fputs(" if ", out);
				expr_fprint(prop->visible.expr, out);
			}
			fputc('\n', out);
			break;
		case P_CHOICE:
			fputs("  #choice value\n", out);
			break;
		case P_SELECT:
			fputs( "  select ", out);
			expr_fprint(prop->expr, out);
			fputc('\n', out);
			break;
		case P_RANGE:
			fputs( "  range ", out);
			expr_fprint(prop->expr, out);
			fputc('\n', out);
			break;
		case P_MENU:
			fputs( "  menu ", out);
			print_quoted_string(out, prop->text);
			fputc('\n', out);
			break;
		default:
			fprintf(out, "  unknown prop %d!\n", prop->type);
			break;
		}
	}
	if (menu->help) {
		int len = strlen(menu->help);
		while (menu->help[--len] == '\n')
			menu->help[len] = 0;
		fprintf(out, "  help\n%s\n", menu->help);
	}
}

void zconfdump(FILE *out)
{
	struct property *prop;
	struct symbol *sym;
	struct menu *menu;

	menu = rootmenu.list;
	while (menu) {
		if ((sym = menu->sym))
			print_symbol(out, menu);
		else if ((prop = menu->prompt)) {
			switch (prop->type) {
			case P_COMMENT:
				fputs("\ncomment ", out);
				print_quoted_string(out, prop->text);
				fputs("\n", out);
				break;
			case P_MENU:
				fputs("\nmenu ", out);
				print_quoted_string(out, prop->text);
				fputs("\n", out);
				break;
			default:
				;
			}
			if (!expr_is_yes(prop->visible.expr)) {
				fputs("  depends ", out);
				expr_fprint(prop->visible.expr, out);
				fputc('\n', out);
			}
		}

		if (menu->list)
			menu = menu->list;
		else if (menu->next)
			menu = menu->next;
		else while ((menu = menu->parent)) {
			if (menu->prompt && menu->prompt->type == P_MENU)
				fputs("\nendmenu\n", out);
			if (menu->next) {
				menu = menu->next;
				break;
			}
		}
	}
}

#include "zconf.lex.c"
#include "util.c"
#include "confdata.c"
#include "expr.c"
#include "symbol.c"
#include "menu.c"
