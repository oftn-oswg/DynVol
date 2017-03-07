## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.13 foreign

ACLOCAL_AMFLAGS = -I autotools

SUBDIRS = src

docs_SOURCES = README.md \
               LICENSE.md
scripts_SOURCES = autogen.sh
nodist_noinst_vcs_SOURCES = .gitignore

dist_doc_DATA = $(docs_SOURCES)
noinst_SCRIPTS = $(scripts_SOURCES)
nodist_noinst_DATA = $(nodist_noinst_vcs_SOURCES)

MAINTAINERCLEANFILES = Makefile.in \
                       aclocal.m4 \
                       configure \
                       ltmain.sh \
                       acinclude.m4 \
                       Makefile

maintainer-clean: maintainer-clean-recursive
	-rm -f $(am__CONFIG_DISTCLEAN_FILES)
	-rm -rf $(top_srcdir)/autom4te.cache
	-rm -f Makefile
	-rm -rf $(top_srcdir)/autotools
