## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I src/tools

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure \
                       ltmain.sh acinclude.m4 Makefile



maintainer-clean: maintainer-clean-recursive
	-rm -f $(am__CONFIG_DISTCLEAN_FILES)
	-rm -rf $(top_srcdir)/autom4te.cache
	-rm -f Makefile
	-rm -rf $(top_srcdir)/src/tools