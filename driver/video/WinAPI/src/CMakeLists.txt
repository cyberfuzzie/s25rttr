#################################################################################
### $Id: CMakeLists.txt 5627 2009-10-12 17:17:06Z FloSoft $
#################################################################################

IF ("${COMPILEFOR}" STREQUAL "windows" )
	FIND_LIBRARY(GDI32_LIBRARY    gdi32)
	FIND_LIBRARY(OPENGL32_LIBRARY opengl32)
	FIND_LIBRARY(USER32_LIBRARY   user32)
	
	# bug: cmake cross detection's not implemented, so he never finds anything!
	
	#IF (GDI32_LIBRARY AND OPENGL_LIBRARY AND USER32_LIBRARY)
		INCLUDE_DIRECTORIES(../../../src)
	
		ADD_LIBRARY(videoWinAPI SHARED WinAPI.cpp WinAPI.h main.h ../../../../version.cpp ../../../src/DriverInterface.cpp)
	
		TARGET_LINK_LIBRARIES(videoWinAPI videodrv gdi32 opengl32 user32) 
		# ${GDI32_LIBRARY} ${OPENGL32_LIBRARY} ${USER32_LIBRARY})
	
		IF ( "${COMPILEFOR}" STREQUAL "windows")
			ADD_DEFINITIONS(-DBUILD_DLL)
		ENDIF ( "${COMPILEFOR}" STREQUAL "windows")
		
		ADD_CUSTOM_COMMAND(TARGET videoWinAPI POST_BUILD COMMAND ln -fvs WinAPI/src/${CMAKE_SHARED_LIBRARY_PREFIX}videoWinAPI${CMAKE_SHARED_LIBRARY_SUFFIX} ../../videoWinAPI${CMAKE_SHARED_LIBRARY_SUFFIX})

		INSTALL(TARGETS videoWinAPI DESTINATION ${LIBDIR}/driver/video)
	#ENDIF (GDI32_LIBRARY AND OPENGL_LIBRARY AND USER32_LIBRARY)
ENDIF ("${COMPILEFOR}" STREQUAL "windows" )

#################################################################################
