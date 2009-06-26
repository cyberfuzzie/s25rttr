#################################################################################
### $Id: CMakeLists.txt 5113 2009-06-26 08:23:54Z FloSoft $
#################################################################################

FIND_LIBRARY(GLFW_LIBRARY glfw) 

IF (GLFW_LIBRARY)
	INCLUDE_DIRECTORIES(../../../src)

	ADD_LIBRARY(videoGLFW SHARED GLFW.cpp GLFW.h oem.cpp oem.h main.h ../../../../version.cpp ../../../src/DriverInterface.cpp)

	TARGET_LINK_LIBRARIES(videoGLFW videodrv ${GLFW_LIBRARY})

	IF ( "${COMPILEFOR}" STREQUAL "win32")
		ADD_DEFINITIONS(-DBUILD_DLL)
		ADD_CUSTOM_COMMAND(TARGET videoGLFW POST_BUILD COMMAND cp -v libvideoGLFW.so videoGLFW.dll)
	ENDIF ( "${COMPILEFOR}" STREQUAL "win32")

	ADD_CUSTOM_COMMAND(TARGET videoGLFW POST_BUILD COMMAND ln -fvs GLFW/src/libvideoGLFW${CMAKE_SHARED_LIBRARY_SUFFIX} ../../videoGLFW${CMAKE_SHARED_LIBRARY_SUFFIX})

	INSTALL(TARGETS videoGLFW DESTINATION lib)
ELSE (GLFW_LIBRARY)
	MESSAGE(WARNING ": GLFW library not found: Not building GLFW videodriver")
ENDIF (GLFW_LIBRARY)

#################################################################################
