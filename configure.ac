# $Id: configure.in,v 1.39 2006/08/03 11:18:28 christof Exp $

AC_PREREQ(2.50)
AC_INIT(ManuProC_Base,0.8.0, [manuproc-developer-de@berlios.de])
AC_CONFIG_AUX_DIR(.)

MPC_BASE_MAJOR_VERSION=`echo $PACKAGE_VERSION | sed -e 's/^\(.*\)\..*\..*$/\1/'`
MPC_BASE_MINOR_VERSION=`echo $PACKAGE_VERSION | sed -e 's/^.*\.\(.*\)\..*$/\1/'`
MPC_BASE_MICRO_VERSION=`echo $PACKAGE_VERSION | sed -e 's/^.*\..*\.\(.*\)$/\1/'`
AC_SUBST(MPC_BASE_MAJOR_VERSION)
AC_SUBST(MPC_BASE_MINOR_VERSION)
AC_SUBST(MPC_BASE_MICRO_VERSION)

AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS(config.h src/ManuProCConfig.h)
AC_PREFIX_DEFAULT(/usr/local)

AC_PROG_CXX
AM_PROG_LIBTOOL

if echo __MINGW32__ | $CPP - - | fgrep -q __MINGW32__
then mingw=false
else mingw=true 
fi
AM_CONDITIONAL(WINDOWS, test x$mingw = xtrue)

dnl *************************************************
dnl gettext support
dnl *************************************************

GETTEXT_PACKAGE=ManuProC_Base
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE",
		[the gettext domain])

if test x$mingw = xtrue ; then
  AM_GLIB_GNU_GETTEXT([])
else
  AM_GNU_GETTEXT([external])
fi

dnl Set PACKAGE_LOCALE_DIR in config.h.
if test -z "${DATADIRNAME}"
then DATADIRNAME=share
fi
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/${DATADIRNAME}/locale",
		[the location of locale files])
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/${DATADIRNAME}/locale",
		[the location of locale files])
fi

AC_LANG_CPLUSPLUS

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(CXXFLAGS)

if test -z "$mpc_sigc_version"
then 
  PKG_CHECK_MODULES(SIGC,[sigc++-2.0 >= 2.2.0],[
		AC_SUBST(SIGC_CFLAGS)
		AC_SUBST(SIGC_LIBS)
		mpc_sigc_version=0x220
		SIGC_MAJOR=2],[false])
fi
if test -z "$mpc_sigc_version"
then 
  PKG_CHECK_MODULES(SIGC,[sigc++-2.0 >= 1.9.15],[
		AC_SUBST(SIGC_CFLAGS)
		AC_SUBST(SIGC_LIBS)
		mpc_sigc_version=0x200
		SIGC_MAJOR=2],[false])
fi

if test \! -z "$mpc_sigc_version"
then 
  AC_DEFINE_UNQUOTED(MPC_SIGC_VERSION,$mpc_sigc_version,[which SigC++ version to use (e.g. 0x220)])
fi
AC_SUBST(SIGC_MAJOR)

AC_ARG_ENABLE(sqlite, [  --enable-sqlite	prefer SQLite over PostgreSQL],
	[prefer_sqlite=0 
         block_sqlite=0
         if test "$enableval" = "no" 
         then block_sqlite=1
         else prefer_sqlite=1
         fi],
	[prefer_sqlite=0 ; block_sqlite=0 ])

if test $prefer_sqlite -eq 0
then
 if test "${with_postgresdir}" != "no"
 then
  ECPG=`which ecpg`
  if test "${with_postgresdir+set}" = "set" -o ! -z "$ECPG"
  then
   PETIG_CHECK_POSTGRES
   
   AC_MSG_CHECKING(for ecpg version)
   tmp_CXXFLAGS="$CXXFLAGS"
   tmp_LIBS="$LIBS"
   tmp_LDFLAGS="$LDFLAGS"
   CXXFLAGS="$CXXFLAGS $ECPG_INCLUDES"
   LIBS="$LIBS $ECPG_LIBS"
   LDFLAGS="$LDFLAGS $ECPG_LDFLAGS"
   AC_COMPILE_IFELSE([
#include <ecpgtype.h>
#include <ecpglib.h>
int main() {  ECPGdo(20, "test", "select true",ECPGt_EOIT, ECPGt_EORT); }
	],[AC_DEFINE(OLD_ECPG, 1, [Define to 1 if you use ecpg 2.9.])
		AC_MSG_RESULT([old (-2.9.0)])],
	[AC_MSG_RESULT([new (3.0.0+)])])
    if test ! -e NoQueryParams
    then
      AC_MSG_CHECKING(for PQprepare)
      AC_COMPILE_IFELSE([
      #include <libpq-fe.h>
      int main() {  PQprepare((PGconn*)NULL,"name","select true",0,NULL); }
            ],[AC_DEFINE(HAVE_PQPREPARE, 1, [Define to 1 if PQprepare is available in libpq.])
                    AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)])
    fi

   AC_MSG_CHECKING(for libecpg 6+)
   AC_TRY_LINK_FUNC([ECPGdescriptor_lvalue], [ac_no_descriptor_lvalue=no
		], [ac_no_descriptor_lvalue=yes; AC_DEFINE(HAVE_LIBECPG6_UP, 1, [Define to 1 if using 8.3+ ecpg (no ECPGdescriptor_lvalue).])])
   AC_MSG_RESULT($ac_no_descriptor_lvalue)
   CXXFLAGS="$tmp_CXXFLAGS"
   LDFLAGS="$tmp_LDFLAGS"
   LIBS="tmp_LIBS"

   AC_MSG_CHECKING(for standard database name)
   DEFAULT_DB="$USER"
   DEFAULT_DBHOST="localhost"
   AC_DEFINE_UNQUOTED(DEFAULT_DB, "$DEFAULT_DB" )
   AC_DEFINE_UNQUOTED(DEFAULT_DBHOST, "$DEFAULT_DBHOST" )
   AC_MSG_RESULT($DEFAULT_DB@$DEFAULT_DBHOST)

   LIBS="$LIBS $ECPG_LIBS"
   DATABASE_TYPE="_pg"
  fi
 fi
fi

if test -z "$ECPG_INCLUDES"
then
 AC_COMPILE_IFELSE([#include <sqlite3.h>],[prefer_sqlite=1],[])
fi

if test "$prefer_sqlite" -eq 1 -a "$block_sqlite" -eq 0
then
  AC_DEFINE(MPC_SQLITE, 1, [Define to 1 if you want to use the SQLite backend])
dnl dann brauchst du aber das !!! CP  
#  PETIG_CHECK_POSTGRES
  ECPG_LIBS=-lsqlite3
  AC_SUBST(ECPG_LIBS)
  ECPG="/bin/touch --"
  AC_SUBST(ECPG)
  ECPG_INCLUDES=""
  ECPG_LDFLAGS=""
  AC_SUBST(ECPG_INCLUDES)
  ECPG_CFLAGS="$ECPG_INCLUDES"
  AC_SUBST(ECPG_CFLAGS)
  AC_SUBST(ECPG_LDFLAGS)
  DATABASE_TYPE="_sqlite3"
   AC_MSG_CHECKING(for standard SQLite database name)
   DEFAULT_DB="$USER"
   AC_DEFINE_UNQUOTED(DEFAULT_DB, "$DEFAULT_DB" )
   AC_DEFINE_UNQUOTED(DEFAULT_DBHOST, "$DEFAULT_DBHOST" )
   AC_MSG_RESULT($DEFAULT_DB)
fi

AC_MSG_CHECKING(for handle debug code)
AC_ARG_ENABLE(handledebug,[  --enable-handledebug	enable Handle Content debugging code (slower, recompile everything if you switch this!)],
	handledebug_exts="$enableval", handledebug_exts=no)
if test "$handledebug_exts" != "no" ; then
	AC_MSG_RESULT([enabled])
	AC_DEFINE(DEBUG_HANDLE_CONTENT,1,[whether handle content tracing is enabled])
else
	AC_MSG_RESULT([disabled])
fi

# perhaps deprecated
if test ! -z "$ECPG_LIBS"
then
  AC_DEFINE(MANUPROC_WITH_DATABASE,1,[a database is present])
fi
AM_CONDITIONAL(DATABASE, test \! -z "$ECPG_LIBS" )
AC_SUBST(DATABASE_TYPE)

AC_OUTPUT(Makefile src/Makefile examples/Makefile src/libManuProC_Base.rc po/Makefile.in)