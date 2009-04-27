ADD_SUBDIRECTORY( src ) 
ADD_SUBDIRECTORY( video ) 
ADD_SUBDIRECTORY( audio ) 


########### install files ###############




#original Makefile.am contents follow:

#################################################################################
### $Id: Makefile.am 1573 2007-03-05 15:09:06Z FloSoft $
#################################################################################
#
## Verweis auf die Makefiles der Unterverzeichnisse
#SUBDIRS = src video audio
#
#.PHONY: mrproper
#
#mrproper:
#	sh $(srcdir)/cleanup.sh
