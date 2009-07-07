#################################################################################
### $Id: CMakeLists.txt 5211 2009-07-07 13:14:17Z FloSoft $
#################################################################################

INCLUDE(FindSDL)
INCLUDE(FindSDL_mixer)

IF (SDL_FOUND)
	IF (SDLMIXER_FOUND)
		INCLUDE_DIRECTORIES(../../../src ../libendian/src)
		
		ADD_LIBRARY(audioSDL SHARED SDL.cpp SDL.h main.h SoundSDL_Music.cpp SoundSDL_Music.h SoundSDL_Effect.cpp SoundSDL_Effect.h ../../../../version.cpp ../../../src/DriverInterface.cpp ../../../src/tempname.cpp ../../../src/tempname.h)
	
		# Let the user choose the flags used for this directory.
		SET(audioSDL_C_FLAGS "-I${SDL_INCLUDE_DIR} -I${SDLMIXER_INCLUDE_DIR}" CACHE STRING "CFLAGS to use for audioSDL")
		SET(audioSDL_CXX_FLAGS "-I${SDL_INCLUDE_DIR} -I${SDLMIXER_INCLUDE_DIR}" CACHE STRING "CXXFLAGS to use for audioSDL")
		
		# Add the user's chosen flags to the build for this directory.
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${audioSDL_C_FLAGS}")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${audioSDL_CXX_FLAGS}")
		
		TARGET_LINK_LIBRARIES(audioSDL audiodrv endian ${SDL_LIBRARY} ${SDLMIXER_LIBRARY})
		
		IF ( "${COMPILEFOR}" STREQUAL "win32")
			ADD_DEFINITIONS(-DBUILD_DLL)
			ADD_CUSTOM_COMMAND(TARGET audioSDL POST_BUILD COMMAND if [ -f libaudioSDL.so ] \; then cp -v libaudioSDL.so audioSDL.dll \; else cp -v libaudioSDL.dll audioSDL.dll \; fi)
		ENDIF ( "${COMPILEFOR}" STREQUAL "win32")
			
		IF ( "${COMPILEFOR}" STREQUAL "macos")
			SET_TARGET_PROPERTIES(audioSDL PROPERTIES LINK_FLAGS "-framework SDL -framework SDL_mixer")
		ENDIF ( "${COMPILEFOR}" STREQUAL "macos")

		ADD_CUSTOM_COMMAND(TARGET audioSDL POST_BUILD COMMAND ln -fvs SDL/src/libaudioSDL${CMAKE_SHARED_LIBRARY_SUFFIX} ../../audioSDL${CMAKE_SHARED_LIBRARY_SUFFIX})

		INSTALL(TARGETS audioSDL DESTINATION lib)
	ELSE (SDLMIXER_FOUND)
		MESSAGE(WARNING ": SDL_mixer library not found: Not building SDL audiodriver")
	ENDIF (SDLMIXER_FOUND)
ELSE (SDL_FOUND)
	MESSAGE(WARNING ": SDL library not found: Not building SDL audiodriver")
ENDIF (SDL_FOUND)

#################################################################################
