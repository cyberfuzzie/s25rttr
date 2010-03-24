################################################################################
### $Id: apple.i386.cmake 6180 2010-03-24 19:25:09Z FloSoft $
################################################################################

# this one is important
SET(CMAKE_SYSTEM_NAME Darwin)
SET(CMAKE_SYSTEM_PROCESSOR i386)

SET(CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "OSX-Architectures")

SET(CMAKE_C_COMPILER   gcc)
SET(CMAKE_CXX_COMPILER g++)

INCLUDE(cmake/apple.common.cmake)
