################################################################################
### $Id: apple.x86_64.cmake 6321 2010-04-13 16:08:47Z FloSoft $
################################################################################

# this one is important
SET(CMAKE_SYSTEM_NAME Darwin)
SET(CMAKE_SYSTEM_PROCESSOR x86_64)

SET_CCACHE(CMAKE_C_COMPILER   gcc)
SET_CCACHE(CMAKE_CXX_COMPILER g++)

SET(COMPILEARCH "x86_64")
SET(CMAKE_OSX_ARCHITECTURES "${COMPILEARCH}")

INCLUDE(cmake/apple.common.cmake)

# disable cmake's own -arch parameter generation
SET(CMAKE_OSX_ARCHITECTURES "" CACHE STRING "" FORCE)

