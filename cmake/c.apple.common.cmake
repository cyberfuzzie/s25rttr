################################################################################
### $Id: c.apple.common.cmake 6125 2010-03-07 18:20:41Z FloSoft $
################################################################################

# specify the cross compiler
SET(CMAKE_C_COMPILER   i686-apple-darwin10-gcc CACHE STRING "CC")
SET(CMAKE_CXX_COMPILER i686-apple-darwin10-g++ CACHE STRING "CPP")
SET(CMAKE_RANLIB i686-apple-darwin10-ranlib CACHE STRING "RANLIB" FORCE)
SET(CMAKE_LIPO i686-apple-darwin10-lipo CACHE STRING "LIPO" FORCE)

# set OSX-Version
SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.4" CACHE STRING "OSX-Target")

# set SDK
SET(CMAKE_PREFIX_PATH "/srv/buildfarm/SDKs/MacOSX10.5.sdk")

FORCE_ADD_FLAGS(CMAKE_C_FLAGS -isysroot ${CMAKE_PREFIX_PATH})
FORCE_ADD_FLAGS(CMAKE_CXX_FLAGS -isysroot ${CMAKE_PREFIX_PATH})

FORCE_ADD_FLAGS(CMAKE_EXE_LINKER_FLAGS -isysroot ${CMAKE_PREFIX_PATH})
FORCE_ADD_FLAGS(CMAKE_MODULE_LINKER_FLAGS -isysroot ${CMAKE_PREFIX_PATH})
FORCE_ADD_FLAGS(CMAKE_SHARED_LINKER_FLAGS -isysroot ${CMAKE_PREFIX_PATH})

# set root path
SET(CMAKE_FIND_ROOT_PATH "${CMAKE_PREFIX_PATH}")

INCLUDE(cmake/apple.common.cmake)
