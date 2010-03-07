################################################################################
### $Id: c.linux.x86_64.cmake 6125 2010-03-07 18:20:41Z FloSoft $
################################################################################

# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR x86_64)

# specify the cross compiler
SET(CMAKE_C_COMPILER   x86_64-pc-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER x86_64-pc-linux-gnu-g++)
SET(CMAKE_RANLIB x86_64-pc-linux-gnu-ranlib)

# set search prefix
SET(CMAKE_PREFIX_PATH "/usr/x86_64-pc-linux-gnu")

INCLUDE(cmake/c.linux.common.cmake)
