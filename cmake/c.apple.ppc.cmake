################################################################################
### $Id: c.apple.ppc.cmake 6122 2010-03-06 12:29:10Z FloSoft $
################################################################################

# this one is important
SET(CMAKE_SYSTEM_NAME Darwin)
SET(CMAKE_SYSTEM_PROCESSOR powerpc)

SET(CMAKE_OSX_ARCHITECTURES "ppc" CACHE STRING "OSX-Architectures")

INCLUDE(cmake/c.apple.common.cmake)
