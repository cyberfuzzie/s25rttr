ADD_SUBDIRECTORY( libendian ) 
ADD_SUBDIRECTORY( src ) 


########### install files ###############




#original Makefile.am contents follow:

#################################################################################
### $Id: Makefile.am 1608 2007-03-16 21:05:41Z FloSoft $
#################################################################################
#
#include $(top_srcdir)/m4/doxygen.am
#
#################################################################################
#
#.PHONY: doc docs version version.h local.h doxygen-run doxygen-doc $(DX_PS_GOAL) $(DX_PDF_GOAL)
#
## Verweis auf die Makefiles der Unterverzeichnisse
#SUBDIRS = libendian src
#
## Dokumentation erzeugen
#doc: doxygen-doc
#docs: doc
#
#mrproper: maintainer-clean
#	$(top_srcdir)/cleanup.sh $(top_srcdir)
#
#dist-hook:
#	dirs='m4 win32' ; \
#	for dir in $$dirs; do \
#		if (test -d $(srcdir)/$$dir) ; then \
#			if (test -d $(srcdir)/$$dir/.svn) || (test -d $(srcdir)/$$dir/_svn) ; then \
#				svn export --force $(srcdir)/$$dir $(distdir)/$$dir ; \
#			else \
#				cp -r $(srcdir)/$$dir $(distdir)/ ; \
#			fi \
#		fi \
#	done
#
#MOSTLYCLEANFILES = $(DX_CLEANFILES)
#EXTRA_DIST = cleanup.sh autogen.sh
