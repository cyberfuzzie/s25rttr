################################################################################
### $Id: c.apple.x86_64.cmake 6122 2010-03-06 12:29:10Z FloSoft $
################################################################################

# this one is important
SET(CMAKE_SYSTEM_NAME Darwin)
SET(CMAKE_SYSTEM_PROCESSOR x86_64)

SET(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "OSX-Architectures")

INCLUDE(cmake/c.apple.common.cmake)
