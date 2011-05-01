dnl $Id: petig.m4,v 1.79 2005/08/30 10:15:41 christof Exp $

dnl Configure paths for some libraries

AC_DEFUN([PETIG_CHECK_ECPG],
[
if test -z "$ECPG_INCLUDES"
then
  AC_MSG_CHECKING(for PostgreSQL ECPG)
  AC_ARG_WITH(postgresdir,
    [  --with-postgresdir=postgresdir  where PostgreSQL is installed ],
    petig_postgresdir=$withval,
    [petig_postgresdir=`which ecpg | sed s+/bin/ecpg++`
     if test ! -x "$ECPG" -a -x /usr/lib/postgresql/bin/ecpg
     then 
        ECPG=/usr/lib/postgresql/bin/ecpg
     fi
    ]
  )
  ECPG="$petig_postgresdir/bin/ecpg"
  if test ! -x "$ECPG" ; then
     AC_MSG_WARN([ecpg not found ($ECPG), please specify --with-postgresdir=PATH if needed])
     ECPG_LIBS=""
     ECPG_INCLUDES=""
     ECPG_LDFLAGS=""
     dnl fake it
     ECPG="/bin/touch --" 
  else     
    AC_MSG_RESULT($ECPG)
    
    AC_MSG_CHECKING(for ECPG include files)
    ECPG_PATH=`$ECPG -v 2>&1 | fgrep -v 'ecpg - ' | fgrep -v 'ecpg, the' | fgrep -v 'search starts here:' | fgrep -v 'nd of search list'`
    ECPG_PATH_OK=0
    for i in $ECPG_PATH
    do
      if test -r $i/ecpgerrno.h ; then ECPG_PATH_OK=1 ; fi
      omit=0
dnl omit these standard paths even though ecpg mentions them
      if test "$i" = "/usr/include" ; then omit=1
      elif test "$i" = "/usr/local/include" ; then omit=1
      fi
      if test $omit -eq 0
      then 
        if (echo $i | fgrep -q include )
        then
          LDIR=`echo $i | sed s+/include+/lib+`
          if test -r $LDIR/libecpg.so
          then
             ECPG_LDFLAGS="$ECPG_LDFLAGS -L$LDIR"
          elif test -r $LDIR/lib/libecpg.so
          then # this strange path is right for debian
             ECPG_LDFLAGS="$ECPG_LDFLAGS -L$LDIR/lib"
          fi
        fi
        ECPG_INCLUDES="$ECPG_INCLUDES -I$i"
      fi
    done
    if test $ECPG_PATH_OK = 0
    then
      AC_MSG_ERROR([No ecpgerrno.h found. Please report. ($ECPG_PATH)])
    else
      AC_MSG_RESULT($ECPG_INCLUDES)
    fi
    ECPG_LIBS='-lecpg -lpq -lcrypt'
    AC_CHECK_LIB(pgtypes,PGTYPESnumeric_add,[ECPG_LIBS="-lecpg -lpgtypes -lpq -lcrypt"])
  fi
  
  AC_SUBST(ECPG)
  AC_SUBST(ECPG_INCLUDES)
  ECPG_CFLAGS="$ECPG_INCLUDES"
  AC_SUBST(ECPG_CFLAGS)
  AC_SUBST(ECPG_LDFLAGS)
  AC_SUBST(ECPG_LIBS)
  ECPG_NODB_LIBS=""
  AC_SUBST(ECPG_NODB_LIBS)
fi
])

dnl this name not that consistent
AC_DEFUN([PETIG_CHECK_POSTGRES],
[ PETIG_CHECK_ECPG
])

dnl MPC_CHECK_LIB(lib name,dir name,define name,alt.lib+dir name,dep1,dep2,dir)

AC_DEFUN([MPC_CHECK_LIB],
[
dnl only if not already checked
if test -z "$$3_INCLUDES"
then
  _mpc_dir="$7"
  if test -z "$_mpc_dir" ; then _mpc_dir=.. ; fi
  dnl dependancies
  ifelse($5,,,[if test -z "$$5_INCLUDES"
    then
      PETIG_CHECK_$5([$_mpc_dir])
    fi
  ])
  ifelse($6,,,[if test -z "$$6_INCLUDES"
    then
      PETIG_CHECK_$6([$_mpc_dir])
    fi
  ])
  
  AC_MSG_CHECKING(for $1 library)
  if test -r "$_mpc_dir/$2/lib$1.a"
  then
    TEMP=`cd $_mpc_dir/$2 ; pwd` 
    $3_INCLUDES="-I$TEMP"
    $3_LDFLAGS=""
    $3_LIBS="$TEMP/lib$1.a"
    $3_DIR="$TEMP"
    AC_MSG_RESULT($$3_DIR)
  elif test -r "$_mpc_dir/$2/src/lib$1.a"
  then 
    TEMP=`cd $_mpc_dir/$2/src ; pwd` 
    $3_INCLUDES="-I$TEMP"
    $3_LDFLAGS=""
    $3_LIBS="$TEMP/lib$1.a"
    $3_DIR="$TEMP"
    AC_MSG_RESULT($$3_DIR)
  dnl libtool library
  elif test -r "$_mpc_dir/$2/src/lib$1.la"
  then 
    TEMP=`cd $_mpc_dir/$2/src ; pwd` 
    $3_INCLUDES="-I$TEMP"
    $3_LDFLAGS="-L$TEMP"
    $3_LIBS="-l$1"
    $3_DIR="$TEMP"
    AC_MSG_RESULT($$3_DIR)
  elif test -n "$4" -a -r "$_mpc_dir/$4/lib$4.a" -o -r "$_mpc_dir/$4/lib$4.la"
  then 
    TEMP=`cd $_mpc_dir/$4 ; pwd` 
    $3_INCLUDES="-I$TEMP"
    $3_LDFLAGS="-L$TEMP"
    $3_LIBS="-l$4"
    $3_DIR="$TEMP"
    AC_MSG_RESULT($$3_DIR)
  elif test -n "$4" -a -r "$_mpc_dir/$4/src/lib$4.a" -o -r "$_mpc_dir/$4/src/lib$4.la"
  then 
    TEMP=`cd $_mpc_dir/$4/src ; pwd` 
    $3_INCLUDES="-I$TEMP"
    $3_LDFLAGS="-L$TEMP"
    $3_LIBS="-l$4"
    $3_DIR="$TEMP"
    AC_MSG_RESULT($$3_DIR)
  else
    if test "x$prefix" = "xNONE" 
    then mytmp="$ac_default_prefix"
    else mytmp="$prefix"
    fi
    ifelse($4,,AC_MSG_ERROR([not found]),[
      if test -d "$mytmp/include/$4" -a -r "$mytmp/lib/lib$4.a"
      then
        $3_INCLUDES="-I$mytmp/include/$4"
        $3_LDFLAGS="-L$mytmp/lib"
        $3_LIBS="-l$4"
	$3_DIR="$mytmp"
        AC_MSG_RESULT($$3_DIR)
      else 
        AC_MSG_ERROR([not found])
      fi
    ])
  fi
  $3_NODB_LIBS="$$3_LIBS"

  dnl dependancies
  ifelse($5,,,[
    $3_INCLUDES="$$5_INCLUDES $$3_INCLUDES"
    $3_LIBS="$$3_LIBS $$5_LIBS"
    $3_NODB_LIBS="$$3_NODB_LIBS $$5_NODB_LIBS"
    $3_LDFLAGS="$$3_LDFLAGS $$5_LDFLAGS" 
  ])
  ifelse($6,,,[
    $3_INCLUDES="$$6_INCLUDES $$3_INCLUDES"
    $3_LIBS="$$3_LIBS $$6_LIBS"
    $3_NODB_LIBS="$$3_NODB_LIBS $$6_NODB_LIBS"
    $3_LDFLAGS="$$3_LDFLAGS $$6_LDFLAGS"
  ])

  $3_CFLAGS=$$3_INCLUDES
  AC_SUBST($3_INCLUDES)
  AC_SUBST($3_CFLAGS)
  AC_SUBST($3_LIBS)
  AC_SUBST($3_NODB_LIBS)
  AC_SUBST($3_LDFLAGS)
fi
])

AC_DEFUN([PETIG_CHECK_GTKMM2],
[
PKG_CHECK_MODULES(GTKMM2,[gtkmm-2.4 >= 2.4.0],
	[PKG_CHECK_MODULES(SIGCDUMMY,[sigc++-2.0 >= 2.2.0],GTKMM_SIGC_VERSION=0x220,GTKMM_SIGC_VERSION=0x200)],
	[PKG_CHECK_MODULES(GTKMM2,[gtkmm-2.0 >= 1.3.20],GTKMM_SIGC_VERSION=0x120)])
GTKMM2_CFLAGS="$GTKMM2_CFLAGS"
AC_SUBST(GTKMM2_CFLAGS)
GTKMM2_INCLUDES="$GTKMM2_CFLAGS"
AC_SUBST(GTKMM2_INCLUDES)
GTKMM2_NODB_LIBS="$GTKMM2_LIBS"
AC_SUBST(GTKMM2_NODB_LIBS)
])

AC_DEFUN([MPC_CHECK_BASE_SIGC],
[ if test -z "$MPC_SIGC_VERSION"
  then
   old_cxxflags="$CXXFLAGS"
   CXXFLAGS="$MPC_BASE_INCLUDES $CXXFLAGS"
   AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
#include <ManuProCConfig.h>
#if MPC_SIGC_VERSION != $1
#error MPC_SIGC_VERSION not $1
#endif
		])],[MPC_SIGC_VERSION=$1],[])
   CXXFLAGS="$old_cxxflags"
  fi
])

AC_DEFUN([PETIG_CHECK_COMMONXX],
[
 MPC_CHECK_LIB(common++,c++,COMMONXX,,MPC_BASE,,[$1])
 dnl commonxx needs to override dependencies (for ManuProCConfig.h)
 COMMONXX_INCLUDES="-I$COMMONXX_DIR $COMMONXX_INCLUDES"
])

AC_DEFUN([MPC_CHECK_BASE],
[
MPC_CHECK_LIB(ManuProC_Base,ManuProC_Base,MPC_BASE,,,,[$1])
# check which sigc was used to configure ManuProC_Base
AC_MSG_CHECKING(which sigc++ was used to configure ManuProC_Base)
MPC_CHECK_BASE_SIGC(0x220)
MPC_CHECK_BASE_SIGC(0x200)
MPC_CHECK_BASE_SIGC(0x120)
AC_MSG_RESULT($MPC_SIGC_VERSION)
  
if test "$MPC_SIGC_VERSION" = 0x120
then
   PKG_CHECK_MODULES(SIGC,[sigc++-1.2 >= 1.2.0])
fi
if test "$MPC_SIGC_VERSION" = 0x200
then
   PKG_CHECK_MODULES(SIGC,[sigc++-2.0 >= 1.9.15])
fi
if test "$MPC_SIGC_VERSION" = 0x220
then
   PKG_CHECK_MODULES(SIGC,[sigc++-2.0 >= 2.2.0])
fi
MPC_BASE_INCLUDES="$MPC_BASE_INCLUDES $SIGC_CFLAGS"
MPC_BASE_LIBS="$MPC_BASE_LIBS $SIGC_LIBS"

AC_MSG_CHECKING(for which database to use)
# check wether SQLite or PostgreSQL
old_cxxflags="$CXXFLAGS"
CXXFLAGS="$MPC_BASE_INCLUDES $CXXFLAGS"
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM([
#include <ManuProCConfig.h>
#ifndef MPC_SQLITE
#error not SQLITE
#endif
		])],[MPC_SQLITE=1],[])
CXXFLAGS="$old_cxxflags"

if test -z "$MPC_SQLITE"
then
	AC_MSG_RESULT("PostgreSQL") 
	PETIG_CHECK_ECPG
	MPC_BASE_LDFLAGS="$MPC_BASE_LDFLAGS $ECPG_LDFLAGS"
	MPC_BASE_INCLUDES="$MPC_BASE_INCLUDES $ECPG_INCLUDES"
	MPC_BASE_LIBS="$MPC_BASE_LIBS $ECPG_LIBS"
else
	AC_MSG_RESULT("SQLite") 
	MPC_BASE_LDFLAGS="$MPC_BASE_LDFLAGS -lsqlite3"
fi

])

AC_DEFUN([PETIG_CHECK_BASE],[ MPC_CHECK_BASE([$1])])
AC_DEFUN([PETIG_CHECK_MPC_BASE],[ MPC_CHECK_BASE([$1])])

AC_DEFUN([MPC_CHECK_SIGC_MATCH],
[
if test "$MPC_SIGC_VERSION" != "$GTKMM_SIGC_VERSION"
then AC_MSG_ERROR([ManuProC_Base was configured with different sigc++ ($MPC_SIGC_VERSION) version than gtkmm ($GTKMM_SIGC_VERSION)])
fi
])

AC_DEFUN([PETIG_CHECK_COMMONGTK2],
[
MPC_CHECK_LIB(GtkmmAddons,GtkmmAddons,COMMONGTK2,gtk2,GTKMM2,,[$1])
])

AC_DEFUN([MPC_CHECK_WIDGETS],
[
MPC_CHECK_LIB(ManuProC_Widgets,ManuProC_Widgets,MPC_WIDGETS,,MPC_BASE,COMMONGTK2,[$1])
MPC_CHECK_SIGC_MATCH
])

AC_DEFUN([PETIG_CHECK_WIDGETS],[ MPC_CHECK_WIDGETS([$1])])
AC_DEFUN([PETIG_CHECK_MPC_WIDGETS],[ MPC_CHECK_WIDGETS([$1])])

AC_DEFUN([MPC_CHECK_KOMPONENTEN2],
[
MPC_CHECK_LIB(Komponenten,Komponenten2,KOMPONENTEN2,Komponenten,MPC_WIDGETS,COMMONXX,[$1])
dnl MPC_CHECK_SIGC_MATCH
])

AC_DEFUN([PETIG_CHECK_KOMPONENTEN2],[ MPC_CHECK_KOMPONENTEN2([$1])])
AC_DEFUN([MPC_CHECK_COMMONXX], [PETIG_CHECK_COMMONXX([$1])])
