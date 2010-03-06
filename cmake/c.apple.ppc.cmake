################################################################################
### $Id: CMakeLists.txt 6087 2010-02-25 09:01:36Z FloSoft $
################################################################################

# this one is important
SET(CMAKE_SYSTEM_NAME Darwin)
SET(CMAKE_SYSTEM_PROCESSOR powerpc)

SET(CMAKE_OSX_ARCHITECTURES "ppc" CACHE STRING "OSX-Architectures")

INCLUDE(cmake/c.apple.common.cmake)
