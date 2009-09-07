#################################################################################
### $Id: CMakeLists.txt 5486 2009-09-07 15:41:39Z FloSoft $
#################################################################################

INCLUDE(FindSDL)

IF (SDL_FOUND)
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
		ADD_CUSTOM_COMMAND(TARGET videoSDL POST_BUILD COMMAND if [ -f libvideoSDL.so ] \; then cp -v libvideoSDL.so videoSDL.dll \; else cp -v libvideoSDL.dll videoSDL.dll \; fi )
	ENDIF ( "${COMPILEFOR}" STREQUAL "windows")

	IF ( "${COMPILEFOR}" STREQUAL "apple")
		SET_TARGET_PROPERTIES(videoSDL PROPERTIES LINK_FLAGS "-framework SDL")
	ENDIF ( "${COMPILEFOR}" STREQUAL "apple")

	ADD_CUSTOM_COMMAND(TARGET videoSDL POST_BUILD COMMAND ln -fvs SDL/src/libvideoSDL${CMAKE_SHARED_LIBRARY_SUFFIX} ../../videoSDL${CMAKE_SHARED_LIBRARY_SUFFIX})
	
	INSTALL(TARGETS videoSDL DESTINATION share/s25rttr/driver/video)
ELSE (SDL_FOUND)
	MESSAGE(WARNING ": SDL library not found: Not building SDL videodriver")
ENDIF (SDL_FOUND)

#################################################################################
