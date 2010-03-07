################################################################################
### $Id: linux.local.cmake 6125 2010-03-07 18:20:41Z FloSoft $
################################################################################

# this one is important
EXECUTE_PROCESS(COMMAND "uname"
	OUTPUT_VARIABLE CMAKE_SYSTEM_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
EXECUTE_PROCESS(COMMAND "uname" "-m"
	OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR OUTPUT_STRIP_TRAILING_WHITESPACE)

# specify the compiler
SET(CMAKE_C_COMPILER   gcc)
SET(CMAKE_CXX_COMPILER g++)

# set compiler flags for "native"
ADD_FLAGS(CMAKE_C_FLAGS -mtune=native)
ADD_FLAGS(CMAKE_CXX_FLAGS -mtune=native)

INCLUDE(cmake/linux.common.cmake)
