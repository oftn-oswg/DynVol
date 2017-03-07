#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([DynVol], [0.0.2], [root@swooshalicio.us],
                     [libdynvol], [https://github.com/oftn-oswg/DynVol])
AC_CONFIG_SRCDIR([src/dynvol_private.h])
AC_REVISION([$Revision: 0.2 $])
AC_CONFIG_AUX_DIR([autotools])
AC_CONFIG_MACRO_DIR([autotools])
AM_INIT_AUTOMAKE()

# Currently only statically links libdynvol
AC_ARG_ENABLE(
    [static-tools],
    [AS_HELP_STRING([--enable-static-tools],[when building tools, statically link libdynvol @<:@default=no@:>@])],
    [
        if test x${enableval} = "xyes"; then
            if test x${enable_static} != "xyes"; then
                AC_MSG_ERROR([Static library build disabled. Cannot build static tools.])
                enable_static_tools="no"
            fi
            enable_static_tools="yes"
        fi
    ],
    [enable_static_tools="no"]
)

# Checks for programs.
AC_PROG_CC
LT_INIT(disable-fast-install shared disable-static)

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

AM_CONDITIONAL([STATIC_TOOLS], [test "${enable_static_tools}" = "yes"])

AC_CONFIG_FILES([
    Makefile
    src/Makefile])
AC_OUTPUT
