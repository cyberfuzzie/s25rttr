#################################################################################
### $Id: CMakeLists.txt 5616 2009-10-09 19:55:02Z FloSoft $
#################################################################################

INCLUDE(FindSDL)
INCLUDE(FindSDL_mixer)

IF (SDL_FOUND)
        IF ( "${COMPILEFOR}" STREQUAL "apple")
                SET(SPLIT "${SDL_LIBRARY}")
                INCLUDE(FindSplitPath)

                MESSAGE(STATUS "SDL-Library is a directory, searching for the Library")

                FIND_FILE(SDL
                                SDL
                                PATHS ${PATHS}
                         )
                IF(NOT SDL-NOTFOUND)
                        MESSAGE(STATUS "SDL-Library was directory, found ${SDL}")
                        SET(SDL_LIBRARY "${SDL}")
                ENDIF(NOT SDL-NOTFOUND)
        ENDIF ( "${COMPILEFOR}" STREQUAL "apple")

	IF (SDLMIXER_FOUND)

	        IF ( "${COMPILEFOR}" STREQUAL "apple")
	                SET(SPLIT "${SDLMIXER_LIBRARY}")
        	        INCLUDE(FindSplitPath)
	
        	        MESSAGE(STATUS "SDLMIXER-Library is a directory, searching for the Library")

                	FIND_FILE(SDLMIXER
                                	SDL_mixer
                        	        PATHS ${PATHS}
	                         )
        	        IF(NOT SDLMIXER-NOTFOUND)
                	        MESSAGE(STATUS "SDLMIXER-Library was directory, found ${SDLMIXER}")
                        	SET(SDLMIXER_LIBRARY "${SDLMIXER}")
	                ENDIF(NOT SDLMIXER-NOTFOUND)
        	ENDIF ( "${COMPILEFOR}" STREQUAL "apple")

		INCLUDE_DIRECTORIES(../../../src ../libendian/src)
		
		ADD_LIBRARY(audioSDL SHARED SDL.cpp SDL.h main.h SoundSDL_Music.cpp SoundSDL_Music.h SoundSDL_Effect.cpp SoundSDL_Effect.h ../../../../version.cpp ../../../src/DriverInterface.cpp ../../../src/tempname.cpp ../../../src/tempname.h)
	
		# Let the user choose the flags used for this directory.
		SET(audioSDL_C_FLAGS "-I${SDL_INCLUDE_DIR} -I${SDLMIXER_INCLUDE_DIR}" CACHE STRING "CFLAGS to use for audioSDL")
		SET(audioSDL_CXX_FLAGS "-I${SDL_INCLUDE_DIR} -I${SDLMIXER_INCLUDE_DIR}" CACHE STRING "CXXFLAGS to use for audioSDL")
		
		# Add the user's chosen flags to the build for this directory.
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${audioSDL_C_FLAGS}")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${audioSDL_CXX_FLAGS}")
		
		TARGET_LINK_LIBRARIES(audioSDL audiodrv endian ${SDL_LIBRARY} ${SDLMIXER_LIBRARY})
		
		IF ( "${COMPILEFOR}" STREQUAL "windows")
			ADD_DEFINITIONS(-DBUILD_DLL)
			ADD_CUSTOM_COMMAND(TARGET audioSDL POST_BUILD COMMAND if [ -f libaudioSDL.so ] \; then cp -v libaudioSDL.so audioSDL.dll \; else cp -v libaudioSDL.dll audioSDL.dll \; fi)
		ENDIF ( "${COMPILEFOR}" STREQUAL "windows")
			
		IF ( "${COMPILEFOR}" STREQUAL "apple")
			SET_TARGET_PROPERTIES(audioSDL PROPERTIES LINK_FLAGS "-framework SDL -framework SDL_mixer")
		ENDIF ( "${COMPILEFOR}" STREQUAL "apple")

		ADD_CUSTOM_COMMAND(TARGET audioSDL POST_BUILD COMMAND ln -fvs SDL/src/libaudioSDL${CMAKE_SHARED_LIBRARY_SUFFIX} ../../audioSDL${CMAKE_SHARED_LIBRARY_SUFFIX})

		INSTALL(TARGETS audioSDL DESTINATION ${DATADIR}/driver/audio)
	ELSE (SDLMIXER_FOUND)
		MESSAGE(WARNING ": SDL_mixer library not found: Not building SDL audiodriver")
	ENDIF (SDLMIXER_FOUND)
ELSE (SDL_FOUND)
	MESSAGE(WARNING ": SDL library not found: Not building SDL audiodriver")
ENDIF (SDL_FOUND)

#################################################################################
