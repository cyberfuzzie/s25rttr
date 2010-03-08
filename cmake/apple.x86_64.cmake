################################################################################
### $Id: apple.x86_64.cmake 6137 2010-03-08 20:12:52Z FloSoft $
################################################################################

# this one is important
SET(CMAKE_SYSTEM_NAME Darwin)
SET(CMAKE_SYSTEM_PROCESSOR x86_64)

SET(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "OSX-Architectures")

INCLUDE(cmake/apple.common.cmake)
