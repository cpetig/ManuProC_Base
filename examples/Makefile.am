# $Id: Makefile.am,v 1.26 2005/09/13 08:01:03 christof Exp $

if WINDOWS
WIN_LDADD=-lintl
else
WIN_LDADD=
endif

LDADD=$(top_srcdir)/src/libManuProC_Base.la @MPC_DB_LIBS@ @SIGC_LIBS@ $(WIN_LDADD)

AM_CXXFLAGS=@CXXFLAGS@ -I$(top_srcdir)/src @MPC_DB_CFLAGS@ @SIGC_CFLAGS@

AM_LDFLAGS=@LDFLAGS@

DB_PRGS=Query Query2

EXTRA_PROGRAMS=test_pgsql_conv \
	testHandles \
	testKW \
	fixedpoint \
	Zeitpunkt \
	mvc \
	trace \
	rexml \
	tagstream \
	tag_speed \
	algorithm extra_cols \
	interval \
	Query \
	datetest \
	Query2 \
	Query_test \
	md5test

test_pgsql_conv_SOURCES=test_pgsql_conv.cc
testHandles_SOURCES=testHandles.cc
testKW_SOURCES=testKW.cc
fixedpoint_SOURCES=fixedpoint.cc
Zeitpunkt_SOURCES=Zeitpunkt.cc
trace_SOURCES=trace.cc
Query2_SOURCES=Query2.cc
mvc_SOURCES=mvc.cc
rexml_SOURCES=rexml.cc
tagstream_SOURCES=tagstream.cc
tag_speed_SOURCES=tag_speed.cc
algorithm_SOURCES=algorithm.cc
extra_cols_SOURCES=extra_cols.cc
interval_SOURCES=interval.cc
Query_SOURCES=Query.cc
Query_test_SOURCES=Query_test.cc
datetest_SOURCES=datetest.cc
md5test_SOURCES=md5test.cc

tagstream.res: tagstream$(EXEEXT)
	./tagstream | sed -e 's/P0x[0-9a-fA-F]*/P*/' \
		-e 's/@0x[0-9a-fA-F]*/@*/' >tagstream.res

test_tagstream: tagstream.res
	diff -u tagstream.out tagstream.res | less

CLEANFILES= *~ $(EXTRA_PROGRAMS)

BUILT_SOURCES=$(DB_PRGS) $(B_SRCES)
