################################################################################
### $Id: apple.i386.cmake 6137 2010-03-08 20:12:52Z FloSoft $
################################################################################

# this one is important
SET(CMAKE_SYSTEM_NAME Darwin)
SET(CMAKE_SYSTEM_PROCESSOR i386)

SET(CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "OSX-Architectures")

INCLUDE(cmake/apple.common.cmake)
