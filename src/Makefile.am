## Process this file with automake to produce Makefile.in

AM_CFLAGS := $(GLIB_CFLAGS) $(GIO_LIBS) $(AM_FLAGS)

lib_LTLIBRARIES = libdynvol.la
libdynvol_la_SOURCES = err.c dynvol.c util.c io.c logging.c
libdynvol_la_LDFLAGS = -version-info 0:2:0
libdynvol_la_LIBADD = $(GLIB_LIBS) $(GIO_LIBS)
libdynvol_la_CFLAGS = -DG_LOG_DOMAIN=\"DynVol\" $(AM_CFLAGS)
include_HEADERS = dynvol.h

bin_PROGRAMS = vtTest vtTest_static vtList vtList_static

vtTest_SOURCES = test.c logging.c
vtTest_LDADD = libdynvol.la $(GLIB_LIBS) $(GIO_LIBS)
vtTest_CFLAGS = $(AM_CFLAGS)

vtTest_static_SOURCES = test.c logging.c
vtTest_static_LDADD = libdynvol.la $(GLIB_LIBS) $(GIO_LIBS)
vtTest_static_LDFLAGS = -static
vtTest_static_CFLAGS = $(AM_CFLAGS)

vtList_SOURCES = vtList.c logging.c
vtList_LDADD = libdynvol.la $(GLIB_LIBS)
vtList_CFLAGS = $(AM_CFLAGS)

vtList_static_SOURCES = vtList.c logging.c
vtList_static_LDADD = libdynvol.la $(GLIB_LIBS)
vtList_static_static_LDFLAGS = -static
vtList_static_CFLAGS = $(AM_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in Makefile