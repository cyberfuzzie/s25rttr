#################################################################################
### $Id: CMakeLists.txt 6076 2010-02-23 16:37:00Z FloSoft $
#################################################################################

INCLUDE(FindSDL)

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

	INCLUDE_DIRECTORIES(../../../src)

	ADD_LIBRARY(videoSDL SHARED SDL.cpp SDL.h main.h oem.cpp oem.h ../../../../version.cpp ../../../src/DriverInterface.cpp)

	# Let the user choose the flags used for this directory.
	SET(videoSDL_C_FLAGS "-I${SDL_INCLUDE_DIR}" CACHE STRING "CFLAGS to use for videoSDL")
	SET(videoSDL_CXX_FLAGS "-I${SDL_INCLUDE_DIR}" CACHE STRING "CXXFLAGS to use for videoSDL")
	
	# Add the user's chosen flags to the build for this directory.
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${videoSDL_C_FLAGS}")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${videoSDL_CXX_FLAGS}")

	TARGET_LINK_LIBRARIES(videoSDL videodrv ${SDL_LIBRARY})

	IF ( "${COMPILEFOR}" STREQUAL "windows")
		ADD_DEFINITIONS(-DBUILD_DLL)
	ENDIF ( "${COMPILEFOR}" STREQUAL "windows")

	ADD_CUSTOM_COMMAND(TARGET videoSDL POST_BUILD COMMAND ln -fvs SDL/src/${CMAKE_SHARED_LIBRARY_PREFIX}videoSDL${CMAKE_SHARED_LIBRARY_SUFFIX} ../../videoSDL${CMAKE_SHARED_LIBRARY_SUFFIX})
	
	INSTALL(TARGETS videoSDL 
		RUNTIME DESTINATION ${LIBDIR}/driver/video 
		LIBRARY DESTINATION ${LIBDIR}/driver/video
		ARCHIVE DESTINATION ${LIBDIR}/driver/video
	)
ELSE (SDL_FOUND)
	MESSAGE(WARNING ": SDL library not found: Not building SDL videodriver")
ENDIF (SDL_FOUND)

#################################################################################
