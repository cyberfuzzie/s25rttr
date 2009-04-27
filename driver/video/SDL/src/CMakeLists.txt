#################################################################################
### $Id: CMakeLists.txt 3392 2008-05-13 13:35:09Z FloSoft $
#################################################################################

INCLUDE(FindSDL)

IF (SDL_FOUND)
	INCLUDE_DIRECTORIES(../../../src)

	ADD_LIBRARY(videoSDL SHARED SDL.cpp SDL.h main.h oem.cpp oem.h ../../../../version.cpp ../../../src/DriverInterface.cpp)

	IF ( "${COMPILEFOR}" STREQUAL "win32")
		ADD_DEFINITIONS(-DBUILD_DLL)
		ADD_CUSTOM_COMMAND(TARGET videoSDL POST_BUILD COMMAND if [ -f libvideoSDL.so ] \; then cp -v libvideoSDL.so videoSDL.dll \; else cp -v libvideoSDL.dll videoSDL.dll \; fi )
		ADD_CUSTOM_COMMAND(TARGET videoSDL POST_BUILD COMMAND ln -fvs SDL/src/videoSDL.dll ../../videoSDL.dll)
	ELSE ( "${COMPILEFOR}" STREQUAL "win32")
		ADD_CUSTOM_COMMAND(TARGET videoSDL POST_BUILD COMMAND ln -fvs SDL/src/libvideoSDL.so ../../SDL.so)
	ENDIF ( "${COMPILEFOR}" STREQUAL "win32")

	INSTALL(TARGETS videoSDL DESTINATION lib)

	TARGET_LINK_LIBRARIES(videoSDL videodrv ${SDL_LIBRARY})
ENDIF (SDL_FOUND)

#################################################################################
