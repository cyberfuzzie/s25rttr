#################################################################################
### $Id: CMakeLists.txt 5098 2009-06-24 17:09:39Z FloSoft $
#################################################################################

INCLUDE(FindSDL)

IF (SDL_FOUND)
	INCLUDE_DIRECTORIES(../../../src)

	ADD_LIBRARY(videoSDL SHARED SDL.cpp SDL.h main.h oem.cpp oem.h ../../../../version.cpp ../../../src/DriverInterface.cpp)

	TARGET_LINK_LIBRARIES(videoSDL videodrv ${SDL_LIBRARY})

	IF ( "${COMPILEFOR}" STREQUAL "win32")
		ADD_DEFINITIONS(-DBUILD_DLL)
		ADD_CUSTOM_COMMAND(TARGET videoSDL POST_BUILD COMMAND if [ -f libvideoSDL.so ] \; then cp -v libvideoSDL.so videoSDL.dll \; else cp -v libvideoSDL.dll videoSDL.dll \; fi )
	ENDIF ( "${COMPILEFOR}" STREQUAL "win32")

	ADD_CUSTOM_COMMAND(TARGET videoSDL POST_BUILD COMMAND ln -fvs SDL/src/libvideoSDL${CMAKE_SHARED_LIBRARY_SUFFIX} ../../videoSDL${CMAKE_SHARED_LIBRARY_SUFFIX})
	
	INSTALL(TARGETS videoSDL DESTINATION lib)
ELSE (SDL_FOUND)
	MESSAGE(WARNING ": SDL library not found: Not building SDL videodriver")
ENDIF (SDL_FOUND)

#################################################################################
