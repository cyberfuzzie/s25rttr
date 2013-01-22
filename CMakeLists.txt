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
# FORCE_ADD_FLAGS(CMAKE_C_FLAGS -I${CMAKE_BINARY_DIR})
# FORCE_ADD_FLAGS(CMAKE_CXX_FLAGS -I${CMAKE_BINARY_DIR})
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR})

# CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/build_version.h.cmake" build_version.h)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/build_paths.h.cmake" build_paths.h)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/build/preinstall.sh.cmake" preinstall.sh @ONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/build/postinstall.sh.cmake" postinstall.sh @ONLY)

# create necessary symlinks in build directory
ADD_CUSTOM_TARGET (populate_builddir ALL
	COMMAND "${CMAKE_SOURCE_DIR}/populate_builddir.sh" "${CMAKE_SOURCE_DIR}")

# build version program and update version header file
ADD_SUBDIRECTORY (version)
ADD_CUSTOM_TARGET (updateversion ALL
				  COMMAND "${CMAKE_BINARY_DIR}/version/src/version" "${CMAKE_SOURCE_DIR}"
				  DEPENDS version
				  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

INSTALL(SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/preinstall.cmake")

ADD_SUBDIRECTORY(driver)
ADD_SUBDIRECTORY(libendian)
ADD_SUBDIRECTORY(mygettext)
ADD_SUBDIRECTORY(libsiedler2)
ADD_SUBDIRECTORY(libutil)
ADD_SUBDIRECTORY(liblobby)
ADD_SUBDIRECTORY(s-c)
ADD_SUBDIRECTORY(s25update)
ADD_SUBDIRECTORY(src)

INSTALL(SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/postinstall.cmake")

