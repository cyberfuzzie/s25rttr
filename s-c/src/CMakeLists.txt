#################################################################################
### $Id: CMakeLists.txt 5099 2009-06-24 17:10:59Z FloSoft $
#################################################################################

FIND_LIBRARY(SAMPLERATE_LIBRARY samplerate)
IF (NOT SAMPLERATE_LIBRARY)
	MESSAGE(FATAL_ERROR "libsamplerate not found")
ENDIF (NOT SAMPLERATE_LIBRARY)

FIND_LIBRARY(SNDFILE_LIBRARY sndfile)
IF (NOT SNDFILE_LIBRARY)
	MESSAGE(FATAL_ERROR "libsndfile not found")
ENDIF (NOT SNDFILE_LIBRARY)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-align-double")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-align-double")

#################################################################################

#LINK_DIRECTORIES(../libsiedler2/src)

ADD_EXECUTABLE(sound-convert getopt.cpp getopt_long.cpp main.cpp
               sample_rate_convert.cpp tempname.cpp)

TARGET_LINK_LIBRARIES(sound-convert siedler2 ${SAMPLERATE_LIBRARY}
                      ${SNDFILE_LIBRARY})

IF ( "${COMPILEFOR}" STREQUAL "win32")
	ADD_CUSTOM_COMMAND(TARGET sound-convert POST_BUILD COMMAND if [ -f sound-convert.exe ] \; then cp -v sound-convert.exe sound-convert \; fi)
ENDIF ( "${COMPILEFOR}" STREQUAL "win32")

ADD_CUSTOM_COMMAND(TARGET sound-convert POST_BUILD COMMAND cp -v sound-convert ../../RTTR/sound-convert)

INSTALL(TARGETS sound-convert DESTINATION bin)

#################################################################################
