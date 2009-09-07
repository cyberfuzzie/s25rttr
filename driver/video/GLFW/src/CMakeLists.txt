#################################################################################
### $Id: CMakeLists.txt 5486 2009-09-07 15:41:39Z FloSoft $
#################################################################################

IF ("${COMPILEFOR}/${COMPILEARCH}" STREQUAL "linux/i686" )
	FIND_LIBRARY(GLFW_LIBRARY glfw) 
	
	IF (GLFW_LIBRARY)
		INCLUDE_DIRECTORIES(../../../src)

		ADD_LIBRARY(videoGLFW SHARED GLFW.cpp GLFW.h oem.cpp oem.h main.h ../../../../version.cpp ../../../src/DriverInterface.cpp)
	
		TARGET_LINK_LIBRARIES(videoGLFW videodrv ${GLFW_LIBRARY})
	
		ADD_CUSTOM_COMMAND(TARGET videoGLFW POST_BUILD COMMAND ln -fvs GLFW/src/libvideoGLFW${CMAKE_SHARED_LIBRARY_SUFFIX} ../../videoGLFW${CMAKE_SHARED_LIBRARY_SUFFIX})
	
		INSTALL(TARGETS videoGLFW DESTINATION share/s25rttr/driver/video)
	ELSE (GLFW_LIBRARY)
		MESSAGE(WARNING ": GLFW library not found: Not building GLFW videodriver")
	ENDIF (GLFW_LIBRARY)
ENDIF ("${COMPILEFOR}/${COMPILEARCH}" STREQUAL "linux/i686" )

#################################################################################
