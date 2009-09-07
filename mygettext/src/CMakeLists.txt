#################################################################################
### $Id: CMakeLists.txt 5487 2009-09-07 15:44:27Z FloSoft $
#################################################################################

INCLUDE(FindIconv)

IF (NOT Iconv_FOUND)
	MESSAGE(FATAL_ERROR "libiconv not found")
ENDIF (NOT Iconv_FOUND)

INCLUDE_DIRECTORIES(../libendian/src)
LINK_DIRECTORIES(../libendian/src)

ADD_LIBRARY(mygettext mygettext.cpp mygettext.h gettext.cpp gettext.h)

TARGET_LINK_LIBRARIES(mygettext endian ${ICONV_LIBRARY})

#################################################################################
