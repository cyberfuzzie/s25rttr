################################################################################
### $Id: CMakeLists.txt 5172 2009-07-02 20:35:05Z FloSoft $
################################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
CMAKE_POLICY(SET CMP0003 OLD)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

INCLUDE(MacroEnsureOutOfSourceBuild)

macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build. Please go to the build directory and run './cmake.sh [options]' there.")

################################################################################

INCLUDE(cmake/crosscompile.cmake)

################################################################################

PROJECT(s25client)

SET(PREFIX  CACHE STRING "Path prefix")
SET(BINDIR  CACHE PATH "Directory for the binaries")
SET(DATADIR CACHE PATH "Data directory")

IF (PREFIX AND NOT DATADIR)
	SET(DATADIR ${PREFIX}/share)
ENDIF (PREFIX AND NOT DATADIR)

# Build lobby client
SET (LOBBY_C TRUE)

################################################################################

# can't compile win32 with those pedantic flags ;)

INCLUDE(CheckCXXCompilerFlag)

IF (NOT "${COMPILEFOR}" STREQUAL "win32" )

	CHECK_CXX_COMPILER_FLAG("-Wall -Werror -pedantic" WERROR_FLAGS)
	IF (WERROR_FLAGS)
		ADD_DEFINITIONS(-Wall -Werror -pedantic)
	ENDIF (WERROR_FLAGS)

ELSE (NOT "${COMPILEFOR}" STREQUAL "win32" )

	CHECK_CXX_COMPILER_FLAG("-w" WNO_FLAGS)
	IF (WNO_FLAGS)
		ADD_DEFINITIONS(-w)
	ENDIF (WNO_FLAGS)
	
	ADD_DEFINITIONS(-D_WIN32)

ENDIF (NOT "${COMPILEFOR}" STREQUAL "win32" )

################################################################################

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/local.h.cmake
               ${CMAKE_CURRENT_SOURCE_DIR}/local.h)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake
               ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

################################################################################

ADD_SUBDIRECTORY(version)

ADD_CUSTOM_TARGET(updateversion ALL
                  COMMAND ${CMAKE_BINARY_DIR}/version/src/version
                  DEPENDS version
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

ADD_CUSTOM_TARGET(mrproper ./cleanup.sh)

################################################################################

INCLUDE(cmake/crosscompile.cmake)

################################################################################

ADD_SUBDIRECTORY(driver)
ADD_SUBDIRECTORY(mygettext)
ADD_SUBDIRECTORY(libsiedler2)
ADD_SUBDIRECTORY(libutil)
ADD_SUBDIRECTORY(liblobby)
ADD_SUBDIRECTORY(s-c)
ADD_SUBDIRECTORY(s25update)
ADD_SUBDIRECTORY(src)

################################################################################

