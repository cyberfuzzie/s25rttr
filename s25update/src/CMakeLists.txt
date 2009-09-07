################################################################################
### $Id: CMakeLists.txt 5496 2009-09-07 17:05:19Z FloSoft $
################################################################################

INCLUDE(FindCURL)
IF (NOT CURL_FOUND)
	MESSAGE(FATAL_ERROR "Curl not found")
ENDIF (NOT CURL_FOUND)

INCLUDE(FindBZip2)
IF (NOT BZIP2_FOUND)
	MESSAGE(FATAL_ERROR "BZip2 not found")
ENDIF (NOT BZIP2_FOUND)
ADD_DEFINITIONS(${BZIP2_DEFINITIONS})

# bzip linkerbug-fix
IF ( "${COMPILEFOR}" STREQUAL "windows" )
	SET(SOURCES_BZIP ../contrib/bzip2-1.0.3/blocksort.c
	    ../contrib/bzip2-1.0.3/huffman.c ../contrib/bzip2-1.0.3/crctable.c
	    ../contrib/bzip2-1.0.3/randtable.c ../contrib/bzip2-1.0.3/compress.c
	    ../contrib/bzip2-1.0.3/decompress.c ../contrib/bzip2-1.0.3/bzlib.c)
ELSE ( "${COMPILEFOR}" STREQUAL "windows" )
	SET(SOURCES_BZIP )
ENDIF ( "${COMPILEFOR}" STREQUAL "windows" )

################################################################################

SET(s25update_SRCS main.cpp md5.cpp md5sum.cpp)

IF ( "${COMPILEFOR}" STREQUAL "windows" )
	ADD_DEFINITIONS("-DAFX_TARG_DEU")

	FIND_PROGRAM(WINDRES NAMES windres i686-mingw32-windres i586-mingw32msvc-windres DOC "path to mingw's windres executable")
	ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/s25update.res.o COMMAND ${WINDRES} -I${CMAKE_CURRENT_SOURCE_DIR}/../win32/ -i${CMAKE_CURRENT_SOURCE_DIR}/../win32/s25update.rc -o ${CMAKE_CURRENT_BINARY_DIR}/s25update.res.o)

	SET(s25update_SRCS ${s25update_SRCS} s25update.res.o)
ENDIF ( "${COMPILEFOR}" STREQUAL "windows" )

ADD_EXECUTABLE(s25update ${s25update_SRCS} ${SOURCES_BZIP})

ADD_DEFINITIONS("-DTARGET=\"${COMPILEFOR}\"")
ADD_DEFINITIONS("-DARCH=\"${COMPILEARCH}\"")

TARGET_LINK_LIBRARIES(s25update ${CURL_LIBRARIES} ${BZIP2_LIBRARIES})

IF ( "${COMPILEFOR}" STREQUAL "windows")
	ADD_CUSTOM_COMMAND(TARGET s25update POST_BUILD COMMAND if [ -f s25update.exe ] \; then cp -v s25update.exe s25update \; fi)
ENDIF ( "${COMPILEFOR}" STREQUAL "windows")

ADD_CUSTOM_COMMAND(TARGET s25update POST_BUILD COMMAND cp -v s25update ../../RTTR/)

################################################################################
