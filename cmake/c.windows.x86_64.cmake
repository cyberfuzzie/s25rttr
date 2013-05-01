################################################################################
### $Id: c.windows.x86_64.cmake 7909 2012-03-31 12:26:40Z marcus $
################################################################################

INCLUDE (FindMinGW)

# this one is important
SET(CMAKE_SYSTEM_NAME Windows)
SET(CMAKE_SYSTEM_PROCESSOR x86_64)

FindMinGW("x86_64")

# specify the cross compiler
SET_CCACHE(CMAKE_AR "${MINGW_AR}")
SET_CCACHE(CMAKE_C_COMPILER "${MINGW_CC}")
SET_CCACHE(CMAKE_CXX_COMPILER "${MINGW_CXX}")
SET_CCACHE(CMAKE_RANLIB "${MINGW_RANLIB}")
SET_CCACHE(CMAKE_RC_COMPILER "${MINGW_WINDRES}")
SET_CCACHE(WINDRES "${MINGW_WINDRES}")

# where is the target environment 
SET(CMAKE_PREFIX_PATH "${MINGW_SYSROOT}")
SET(ENV{SDLMIXER_DIR} "${CMAKE_PREFIX_PATH}")

INCLUDE(cmake/c.windows.common.cmake)
