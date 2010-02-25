#################################################################################
### $Id: CMakeLists.txt 6086 2010-02-25 08:51:56Z FloSoft $
#################################################################################

IF ( NOT "${COMPILEFOR}" STREQUAL "windows" )
	IF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "x86_64" )
		ADD_DEFINITIONS(-fPIC)
	ENDIF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "x86_64" )
ENDIF ( NOT "${COMPILEFOR}" STREQUAL "windows" )

#################################################################################

ADD_LIBRARY(endian STATIC libendian.cpp read_char.cpp read_int.cpp
            read_short.cpp swap.cpp write_char.cpp write_int.cpp
            write_short.cpp)

#INSTALL(TARGETS endian DESTINATION lib)
#INSTALL(FILES DESTINATION /include/libsiedler2/libendian)

#################################################################################
