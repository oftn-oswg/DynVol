## Process this file with automake to produce Makefile.in

AM_CFLAGS := $(GLIB_CFLAGS) $(GIO_LIBS) $(AM_FLAGS)

lib_LTLIBRARIES = libdynvol.la

# qtcreator is weird about headers in autotools projects so we do this to force
# it to list them.
includes_SOURCES    = dynvol.h \
                      vol_err.h \
                      vol_err_defs.h
privheaders_SOURCES = dynvol_private.h \
                      io.h \
                      logging.h \
                      util.h
pkginclude_HEADERS  = $(includes_SOURCES)
noinst_HEADERS      = $(privheaders_SOURCES)

libdynvol_la_SOURCES = dynvol.c \
                       util.c \
                       io.c \
                       logging.c \
                       vol_err.c
libdynvol_la_LDFLAGS = -version-info 0:2:0
libdynvol_la_LIBADD  = $(GLIB_LIBS) $(GIO_LIBS)
libdynvol_la_CFLAGS  = -DG_LOG_DOMAIN=\"DynVol\" $(AM_CFLAGS)

if STATIC_TOOLS
TOOLS_LDFLAGS = -static $(AM_LDFLAGS)
else
TOOLS_LDFLAGS = $(AM_LDFLAGS)
endif

bin_PROGRAMS = vtTest \
               vtList

vtTest_SOURCES = test.c \
                 logging.c
vtTest_LDADD   = libdynvol.la $(GLIB_LIBS) $(GIO_LIBS)
vtTest_LDFLAGS = $(TOOLS_LDFLAGS)
vtTest_CFLAGS  = $(AM_CFLAGS)

vtList_SOURCES = vtList.c \
                 logging.c
vtList_LDADD   = libdynvol.la $(GLIB_LIBS) $(GIO_LIBS)
vtList_LDFLAGS = $(TOOLS_LDFLAGS)
vtList_CFLAGS  = $(AM_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in \
                       Makefile
