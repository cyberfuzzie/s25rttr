################################################################################
### $Id: c.apple.common.cmake 6122 2010-03-06 12:29:10Z FloSoft $
################################################################################

# specify the cross compiler
SET(CMAKE_C_COMPILER   i686-apple-darwin10-gcc CACHE STRING "CC")
SET(CMAKE_CXX_COMPILER i686-apple-darwin10-g++ CACHE STRING "CPP")
SET(CMAKE_RANLIB i686-apple-darwin10-ranlib CACHE STRING "RANLIB" FORCE)
SET(CMAKE_LIPO i686-apple-darwin10-lipo CACHE STRING "LIPO" FORCE)

# set OSX-Version
SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.4" CACHE STRING "OSX-Target")

# set SDK
SET(CMAKE_PREFIX_PATH "/srv/buildfarm/SDKs/MacOSX10.4u.sdk")

# set root path
SET(CMAKE_FIND_ROOT_PATH "${CMAKE_PREFIX_PATH}")
