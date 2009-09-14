#################################################################################
### $Id: CMakeLists.txt 5524 2009-09-14 16:59:56Z FloSoft $
#################################################################################

INCLUDE(FindSndfile)
INCLUDE(FindSamplerate)

INCLUDE(CheckCXXCompilerFlag)

IF (SNDFILE_FOUND)
	IF (SAMPLERATE_FOUND)

		# Let the user choose the flags used for this directory.
		SET(SC_C_FLAGS "-mno-align-double -I${SNDFILE_INCLUDE_DIR} -I${SAMPLERATE_INCLUDE_DIR}" CACHE STRING "CFLAGS to use for sound-convert")
		SET(SC_CXX_FLAGS "-mno-align-double -I${SNDFILE_INCLUDE_DIR} -I${SAMPLERATE_INCLUDE_DIR}" CACHE STRING "CXXFLAGS to use for sound-convert")

		CHECK_CXX_COMPILER_FLAG("-mno-align-double" NOALIGN_FLAGS)
		IF (NOALIGN_FLAGS)
			SET(SC_C_FLAGS "${SC_C_FLAGS} -mno-align-double")
			SET(SC_CXX_FLAGS "${SC_CXX_FLAGS} -mno-align-double")
		ENDIF (NOALIGN_FLAGS)
		
		# Add the user's chosen flags to the build for this directory.
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SC_C_FLAGS}")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SC_CXX_FLAGS}")
		
		#################################################################################
		
		#LINK_DIRECTORIES(../libsiedler2/src)
		
		ADD_EXECUTABLE(sound-convert getopt.cpp getopt_long.cpp main.cpp sample_rate_convert.cpp tempname.cpp)
		
		TARGET_LINK_LIBRARIES(sound-convert siedler2 ${SAMPLERATE_LIBRARY} ${SNDFILE_LIBRARY})
		
		IF ( "${COMPILEFOR}" STREQUAL "windows")
			ADD_CUSTOM_COMMAND(TARGET sound-convert POST_BUILD COMMAND if [ -f sound-convert.exe ] \; then cp -v sound-convert.exe sound-convert \; fi)
		ENDIF ( "${COMPILEFOR}" STREQUAL "windows")
		
		IF ( "${COMPILEFOR}" STREQUAL "apple")
			SET_TARGET_PROPERTIES(sound-convert PROPERTIES LINK_FLAGS "-framework sndfile -framework samplerate")
		ENDIF ( "${COMPILEFOR}" STREQUAL "apple")

		ADD_CUSTOM_COMMAND(TARGET sound-convert POST_BUILD COMMAND cp -v sound-convert ../../RTTR/sound-convert)
		
		INSTALL(TARGETS sound-convert DESTINATION share/s25rttr/RTTR)

	ELSE (SAMPLERATE_FOUND)
		MESSAGE(WARNING ": libsamplerate not found: Will not build sound-convert binary")
		MESSAGE(WARNING ": You won't have correct sound effects without this!")
	ENDIF (SAMPLERATE_FOUND)
ELSE (SNDFILE_FOUND)
	MESSAGE(WARNING ": libsndfile not found: Will not build sound-convert binary")
	MESSAGE(WARNING ": You won't have correct sound effects without this!")
ENDIF (SNDFILE_FOUND)

#################################################################################
