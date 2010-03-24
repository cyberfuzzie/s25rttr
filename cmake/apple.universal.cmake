################################################################################
### $Id: apple.universal.cmake 6187 2010-03-24 20:42:06Z FloSoft $
################################################################################

# this one is important
SET(CMAKE_SYSTEM_NAME Darwin)
#SET(CMAKE_SYSTEM_PROCESSOR universal)

SET(CMAKE_C_COMPILER   gcc)
SET(CMAKE_CXX_COMPILER g++)

INCLUDE(cmake/apple.common.cmake)

# disable cmake's own -arch parameter generation
SET(CMAKE_OSX_ARCHITECTURES "" CACHE STRING "" FORCE)

