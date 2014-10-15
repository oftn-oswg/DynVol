#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([DynVol], [0.0.2], [swooshycueb@tearmedia.info],
                     [libdynvol], [https://github.com/SwooshyCueb/DynVol])
AC_CONFIG_SRCDIR([src/dynvol_private.h])
AC_REVISION([$Revision: 0.2 $])
AC_CONFIG_AUX_DIR([src/tools])
AC_CONFIG_MACRO_DIR([src/tools])
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC
LT_INIT(disable-fast-install shared static)

# Checks for libraries.
PKG_CHECK_MODULES([GLIB], [glib-2.0 >=  2.32.0])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
PKG_CHECK_MODULES([GIO], [gio-2.0])
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)


# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([
    Makefile
    src/Makefile])
AC_OUTPUT
