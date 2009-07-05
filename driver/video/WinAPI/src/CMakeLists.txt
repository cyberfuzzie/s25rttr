#################################################################################
### $Id: CMakeLists.txt 5194 2009-07-05 09:26:54Z FloSoft $
#################################################################################

FIND_LIBRARY(GDI32_LIBRARY    gdi32)
FIND_LIBRARY(OPENGL32_LIBRARY opengl32)
FIND_LIBRARY(USER32_LIBRARY   user32)

# bug: cmake cross detection's not implemented, so he never finds anything!

#IF (GDI32_LIBRARY AND OPENGL_LIBRARY AND USER32_LIBRARY)
	INCLUDE_DIRECTORIES(../../../src)

	ADD_LIBRARY(videoWinAPI SHARED WinAPI.cpp WinAPI.h main.h ../../../../version.cpp ../../../src/DriverInterface.cpp)

	TARGET_LINK_LIBRARIES(videoWinAPI videodrv gdi32 opengl32 user32) 
	# ${GDI32_LIBRARY} ${OPENGL32_LIBRARY} ${USER32_LIBRARY})

	IF ( "${COMPILEFOR}" STREQUAL "win32")
		ADD_DEFINITIONS(-DBUILD_DLL)
		ADD_CUSTOM_COMMAND(TARGET videoWinAPI POST_BUILD COMMAND if [ -f libvideoWinAPI.so ] \; then cp -v libvideoWinAPI.so videoWinAPI.dll \; else cp -v libvideoWinAPI.dll videoWinAPI.dll \; fi )
	ENDIF ( "${COMPILEFOR}" STREQUAL "win32")

	ADD_CUSTOM_COMMAND(TARGET videoWinAPI POST_BUILD COMMAND ln -fvs SDL/src/libvideoWinAPI${CMAKE_SHARED_LIBRARY_SUFFIX} ../../videoWinAPI${CMAKE_SHARED_LIBRARY_SUFFIX})

	INSTALL(TARGETS videoWinAPI DESTINATION lib)
#ENDIF (GDI32_LIBRARY AND OPENGL_LIBRARY AND USER32_LIBRARY)

#################################################################################
