#################################################################################
### $Id: CMakeLists.txt 5214 2009-07-07 13:35:10Z FloSoft $
#################################################################################

FIND_LIBRARY(SNDFILE_LIBRARY sndfile)
FIND_LIBRARY(SAMPLERATE_LIBRARY samplerate)

IF (NOT SNDFILE_LIBRARY)
	MESSAGE(WARNING ": libsndfile not found: Will not build sound-convert binary")
	MESSAGE(WARNING ": You won't have correct sound effects without this!")
ENDIF (NOT SNDFILE_LIBRARY)

IF (NOT SAMPLERATE_LIBRARY)
	MESSAGE(WARNING ": libsamplerate not found: Will not build sound-convert binary")
	MESSAGE(WARNING ": You won't have correct sound effects without this!")
ENDIF (NOT SAMPLERATE_LIBRARY)

IF (SNDFILE_LIBRARY)
	IF (SAMPLERATE_LIBRARY)

		# Let the user choose the flags used for this directory.
		SET(SC_C_FLAGS "-mno-align-double -I${SNDFILE_INCLUDE_DIR} -I${SAMPLERATE_INCLUDE_DIR}" CACHE STRING "CFLAGS to use for sound-convert")
		SET(SC_CXX_FLAGS "-mno-align-double -I${SNDFILE_INCLUDE_DIR} -I${SAMPLERATE_INCLUDE_DIR}" CACHE STRING "CXXFLAGS to use for sound-convert")
		
		# Add the user's chosen flags to the build for this directory.
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SC_C_FLAGS}")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SC_CXX_FLAGS}")
		
		#################################################################################
		
		#LINK_DIRECTORIES(../libsiedler2/src)
		
		ADD_EXECUTABLE(sound-convert getopt.cpp getopt_long.cpp main.cpp sample_rate_convert.cpp tempname.cpp)
		
		TARGET_LINK_LIBRARIES(sound-convert siedler2 ${SAMPLERATE_LIBRARY} ${SNDFILE_LIBRARY})
		
		IF ( "${COMPILEFOR}" STREQUAL "win32")
			ADD_CUSTOM_COMMAND(TARGET sound-convert POST_BUILD COMMAND if [ -f sound-convert.exe ] \; then cp -v sound-convert.exe sound-convert \; fi)
		ENDIF ( "${COMPILEFOR}" STREQUAL "win32")
		
		ADD_CUSTOM_COMMAND(TARGET sound-convert POST_BUILD COMMAND cp -v sound-convert ../../RTTR/sound-convert)
		
		INSTALL(TARGETS sound-convert DESTINATION bin)

	ENDIF (SAMPLERATE_LIBRARY)
ENDIF (SNDFILE_LIBRARY)

#################################################################################
