CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

CMAKE_POLICY(SET CMP0015 OLD)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
INCLUDE(CMakeMacroEnsureOutOfSourceBuild)
INCLUDE(CMakeMacroSetCCache)
INCLUDE(CMakeMacroAddFlags)
INCLUDE(CMakeMacroForceAddFlags)

SET (COMPILEFOR_PLATFORM "linux.local"
	CACHE STRING "Target platform")

INCLUDE (cmake/crosscompile.cmake)

PROJECT (s25client)

ENSURE_OUT_OF_SOURCE_BUILD("${PROJECT_NAME} requires an out of source build. Please go to the build directory and run cmake there.")

# Build lobby client
SET (LOBBY_C TRUE)

SET (PREFIX "${CMAKE_INSTALL_PREFIX}")
SET (BINDIR "bin"
	CACHE PATH "Directory for binaries")
SET (DATADIR "share/s25rttr"
	CACHE PATH "Data directory")
SET (LIBDIR "${DATADIR}"
	CACHE PATH "Directory for libraries")

# add build directory to compiler search path
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR})

# set flags for compiling
ADD_DEFINITIONS (-Wall -pedantic -fno-strict-aliasing)

# write paths for components to build_paths.h
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/build_paths.h.cmake" build_paths.h)

# create necessary symlinks in build directory for executing in build dir
FOREACH (dir bin share s25rttr)
	IF (NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/${dir}")
		MESSAGE (STATUS "Creating symlink ${dir}")
		EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} -E create_symlink "." ${dir})
	ENDIF (NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/${dir}")
ENDFOREACH (dir bin share s25rttr)
IF (NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/RTTR")
	IF (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/RTTR")
		MESSAGE (STATUS "Creating symlink RTTR")
		EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/RTTR" RTTR)
	ELSE (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/RTTR")
		MESSAGE (FATAL_ERROR " 
 Directory RTTR is missing")
	ENDIF (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/RTTR")	
ENDIF (NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/RTTR")
IF (NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/S2")
	IF (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/S2")
		MESSAGE (STATUS "Creating symlink S2")
		EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/S2" S2)
	ELSE (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/S2")
		MESSAGE (WARNING " 
 Directory S2 is missing
 Direct debugging from this directory will not work")
	ENDIF (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/S2")	
ENDIF (NOT IS_DIRECTORY "${CMAKE_BINARY_DIR}/S2")

# build version program and update version header file
ADD_SUBDIRECTORY (version)
ADD_CUSTOM_TARGET (updateversion ALL
				  COMMAND "${CMAKE_BINARY_DIR}/version/src/version" "${CMAKE_SOURCE_DIR}"
				  DEPENDS version
				  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

IF ( "${COMPILEFOR}" STREQUAL "windows" )
	INSTALL (PROGRAMS release/bin/rttr.bat
		DESTINATION ${BINDIR})
ELSE ( "${COMPILEFOR}" STREQUAL "windows" )
	INSTALL (PROGRAMS release/bin/rttr.sh
		DESTINATION ${BINDIR})
ENDIF ( "${COMPILEFOR}" STREQUAL "windows" )
INSTALL (DIRECTORY RTTR
	DESTINATION ${DATADIR})
INSTALL (DIRECTORY
	DESTINATION ${DATADIR}/S2)
FILE (WRITE "${CMAKE_BINARY_DIR}/S2/put your S2-Installation in here")
INSTALL (FILES "S2/put your S2-Installation in here" DESTINATION ${DATADIR}/S2)


ADD_SUBDIRECTORY(driver)
ADD_SUBDIRECTORY(libendian)
ADD_SUBDIRECTORY(mygettext)
ADD_SUBDIRECTORY(libsiedler2)
ADD_SUBDIRECTORY(libutil)
ADD_SUBDIRECTORY(liblobby)
ADD_SUBDIRECTORY(s-c)
ADD_SUBDIRECTORY(s25update)
ADD_SUBDIRECTORY(src)

