## Process this file with automake to produce Makefile.in

AM_CFLAGS := $(GLIB_CFLAGS) $(GIO_LIBS) $(AM_FLAGS)

lib_LTLIBRARIES = libdynvol.la
libdynvol_la_SOURCES = err.c dynvol.c util.c io.c
libdynvol_la_LDFLAGS = -version-info 0:2:0
libdynvol_la_LIBADD = $(GLIB_LIBS) $(GIO_LIBS)
libdynvol_la_CFLAGS = -DG_LOG_DOMAIN=\"DynVol\" $(AM_CFLAGS)
include_HEADERS = dynvol.h

bin_PROGRAMS = test test_static

test_SOURCES = test.c
test_LDADD = libdynvol.la $(GLIB_LIBS) $(GIO_LIBS)
test_CFLAGS = $(AM_CFLAGS)

test_static_SOURCES = test.c
test_static_LDADD = libdynvol.la $(GLIB_LIBS) $(GIO_LIBS)
test_static_LDFLAGS = -static
test_static_CFLAGS = $(AM_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in Makefile