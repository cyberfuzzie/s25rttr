#################################################################################
### $Id: CMakeLists.txt 5113 2009-06-26 08:23:54Z FloSoft $
#################################################################################

INCLUDE(../../cmake/FindIconv.cmake)

IF (NOT Iconv_FOUND)
	MESSAGE(FATAL_ERROR "libiconv not found")
ENDIF (NOT Iconv_FOUND)

INCLUDE_DIRECTORIES(../libendian/src)
LINK_DIRECTORIES(../libendian/src)

ADD_LIBRARY(mygettext mygettext.cpp mygettext.h gettext.cpp gettext.h)

TARGET_LINK_LIBRARIES(mygettext endian ${ICONV_LIBRARY})

INSTALL(TARGETS mygettext DESTINATION lib)
