
AC_PREREQ(2.50)
AC_INIT(ManuProC_Base,1.1.0, [manuproc-developer-de@berlios.de])
AC_CONFIG_AUX_DIR(.)

MPC_BASE_MAJOR_VERSION=`echo $PACKAGE_VERSION | sed -e 's/^\(.*\)\..*\..*$/\1/'`
MPC_BASE_MINOR_VERSION=`echo $PACKAGE_VERSION | sed -e 's/^.*\.\(.*\)\..*$/\1/'`
MPC_BASE_MICRO_VERSION=`echo $PACKAGE_VERSION | sed -e 's/^.*\..*\.\(.*\)$/\1/'`
AC_SUBST(MPC_BASE_MAJOR_VERSION)
AC_SUBST(MPC_BASE_MINOR_VERSION)
AC_SUBST(MPC_BASE_MICRO_VERSION)

AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_HEADERS(config.h src/ManuProCConfig.h)
AC_PREFIX_DEFAULT(/usr/local)

AC_PROG_CXX
AM_PROG_LIBTOOL

# add for legacy reasons.
AM_PROG_CC_C_O

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
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE",[the gettext domain])

AM_GNU_GETTEXT_VERSION([0.19.8])
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
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/${DATADIRNAME}/locale",[the location of locale files])
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/${DATADIRNAME}/locale",[the location of locale files])
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

MPC_CHECK_DB

if test ! -e NoQueryParams
then
  AC_MSG_CHECKING(for PQprepare)
  AC_COMPILE_IFELSE([AC_LANG_SOURCE([
      #include <libpq-fe.h>
      int main() {  PQprepare((PGconn*)NULL,"name","select true",0,NULL); }
    ])],
    [AC_DEFINE(HAVE_PQPREPARE, 1, [Define to 1 if PQprepare is available in libpq.]) AC_MSG_RESULT(yes)],
    [AC_MSG_RESULT(no)])
fi

AC_MSG_CHECKING([for standard database name])
DEFAULT_DB="$MPC_DEFAULT_DB"
DEFAULT_DBHOST="$MPC_DEFAULT_DBHOST"
AC_DEFINE_UNQUOTED([DEFAULT_DB], ["$DEFAULT_DB"], [default db name])
AC_DEFINE_UNQUOTED([DEFAULT_DBHOST], ["$DEFAULT_DBHOST"], [default db host])
AC_MSG_RESULT($DEFAULT_DB@$DEFAULT_DBHOST)

LIBS="$LIBS $MPC_DB_LIBS"
DATABASE_TYPE="_pg"

AC_MSG_CHECKING(check if SQLite is explicitely disabled)
AC_ARG_ENABLE(sqlite,[  --disable-sqlite       Disable SQLite support], use_sqlite="$enableval", use_sqlite="yes")
if test "$use_sqlite" != "yes" ; then
        AC_MSG_RESULT([yes])
else
        AC_MSG_RESULT([no])
fi

if test "$use_sqlite" != "no" 
then
  AC_DEFINE(MPC_SQLITE, 1, [Define to 1 if you want to use the SQLite backend])
  DATABASE_TYPE="${DATABASE_TYPE}_sqlite3"
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
if test ! -z "$LIBS"
then
  AC_DEFINE(MANUPROC_WITH_DATABASE,1,[a database is present])
fi
AM_CONDITIONAL(DATABASE, test \! -z "$LIBS" )
AC_SUBST(DATABASE_TYPE)

AC_OUTPUT(Makefile src/Makefile examples/Makefile src/libManuProC_Base.rc po/Makefile.in)
