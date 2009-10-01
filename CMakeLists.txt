################################################################################
### $Id: CMakeLists.txt 5574 2009-10-01 10:57:54Z FloSoft $
################################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
CMAKE_POLICY(SET CMP0003 OLD)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

################################################################################

INCLUDE(cmake/crosscompile.cmake)

################################################################################

PROJECT(s25client)

SET(PREFIX  CACHE STRING "Path prefix")
SET(BINDIR  CACHE STRING "Directory for the binaries")
SET(DATADIR CACHE STRING "Data directory")

IF (PREFIX AND NOT DATADIR)
	SET(DATADIR ${PREFIX}/share/s25rttr)
ENDIF (PREFIX AND NOT DATADIR)

# ensure out of source build
INCLUDE(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build. Please go to the build directory and run './cmake.sh [options]' there.")

# Build lobby client
SET (LOBBY_C TRUE)

################################################################################

INCLUDE(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-Wall -Werror -pedantic -Wno-long-long -fno-strict-aliasing" WERROR_FLAGS)
IF (WERROR_FLAGS)
	IF (NOT "${COMPILEFOR}" STREQUAL "windows" )
		ADD_DEFINITIONS(-Wall -Werror -pedantic -fno-strict-aliasing)
	ELSE (NOT "${COMPILEFOR}" STREQUAL "windows" )
		ADD_DEFINITIONS(-Wall -pedantic -Wno-long-long) # win32 cross build with -Werror does not work
	ENDIF (NOT "${COMPILEFOR}" STREQUAL "windows" )
ENDIF (WERROR_FLAGS)

IF ("${COMPILEFOR}" STREQUAL "linux" )
	CHECK_CXX_COMPILER_FLAG("-fPIC" FPIC_FLAGS)
	IF (FPIC_FLAGS)
		IF ("${COMPILEARCH}" STREQUAL "amd64" )
			ADD_DEFINITIONS(-fPIC)
		ENDIF ("${COMPILEARCH}" STREQUAL "amd64" )
	ENDIF (FPIC_FLAGS)
ENDIF ("${COMPILEFOR}" STREQUAL "linux" )

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

ADD_CUSTOM_TARGET(mrproper
		  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build/cleanup.sh
		  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


# preinstall script
INSTALL(SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/cmake/install.cmake)

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

