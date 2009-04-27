#################################################################################
### $Id$
#################################################################################

FIND_LIBRARY(GLFW_LIBRARY glfw) 

IF (GLFW_LIBRARY)
	INCLUDE_DIRECTORIES(../../../src)

	ADD_LIBRARY(videoGLFW SHARED GLFW.cpp GLFW.h oem.cpp oem.h main.h ../../../../version.cpp ../../../src/DriverInterface.cpp)
	TARGET_LINK_LIBRARIES(videoGLFW videodrv ${GLFW_LIBRARY})

	IF ( "${COMPILEFOR}" STREQUAL "win32")
		ADD_DEFINITIONS(-DBUILD_DLL)
		ADD_CUSTOM_COMMAND(TARGET videoGLFW POST_BUILD COMMAND cp -v libvideoGLFW.so videoGLFW.dll)
		ADD_CUSTOM_COMMAND(TARGET videoGLFW POST_BUILD COMMAND ln -fvs GLFW/src/videoGLFW.dll ../../videoGLFW.so)
	ELSE ( "${COMPILEFOR}" STREQUAL "win32")
		ADD_CUSTOM_COMMAND(TARGET videoGLFW POST_BUILD COMMAND ln -fvs GLFW/src/libvideoGLFW.so ../../GLFW.so)
	ENDIF ( "${COMPILEFOR}" STREQUAL "win32")

	INSTALL(TARGETS videoGLFW DESTINATION lib)
ELSE (GLFW_LIBRARY)
	MESSAGE(WARNING "Not building GLFW")	
ENDIF (GLFW_LIBRARY)

#################################################################################
