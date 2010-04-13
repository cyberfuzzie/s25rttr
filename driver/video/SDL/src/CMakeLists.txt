#################################################################################
### $Id: CMakeLists.txt 6320 2010-04-13 15:30:16Z FloSoft $
#################################################################################

INCLUDE(FindSDL)

IF (SDL_FOUND)
	INCLUDE(CMakeMacroCorrectLib)
	
	CORRECT_LIB(SDL_LIBRARY SDL)

	INCLUDE_DIRECTORIES(../../../src)

	ADD_LIBRARY(videoSDL SHARED SDL.cpp SDL.h main.h oem.cpp oem.h ../../../../version.cpp ../../../src/DriverInterface.cpp)

	ADD_FLAGS(CMAKE_C_FLAGS -I${SDL_INCLUDE_DIR})
	ADD_FLAGS(CMAKE_CXX_FLAGS -I${SDL_INCLUDE_DIR})

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
