#################################################################################
### $Id: CMakeLists.txt 5490 2009-09-07 16:12:40Z FloSoft $
#################################################################################

IF ( NOT "${COMPILEFOR}" STREQUAL "windows" )
	IF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "amd64" )
		ADD_DEFINITIONS(-fPIC)
	ENDIF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "amd64" )
ENDIF ( NOT "${COMPILEFOR}" STREQUAL "windows" )

#################################################################################

ADD_LIBRARY(endian STATIC libendian.cpp read_char.cpp read_int.cpp
            read_short.cpp swap.cpp write_char.cpp write_int.cpp
            write_short.cpp)

#INSTALL(TARGETS endian DESTINATION lib)
#INSTALL(FILES DESTINATION /include/libsiedler2/libendian)

#################################################################################
