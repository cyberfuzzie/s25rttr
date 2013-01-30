################################################################################
### $Id: linux.local.cmake 6807 2010-10-18 14:12:04Z FloSoft $
################################################################################

# this one is important
EXECUTE_PROCESS(COMMAND "uname"
	OUTPUT_VARIABLE CMAKE_SYSTEM_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
EXECUTE_PROCESS(COMMAND "uname" "-m"
	OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR OUTPUT_STRIP_TRAILING_WHITESPACE)

# specify the compiler
IF (NOT $ENV{CC} STREQUAL "")
	SET_CCACHE(CMAKE_C_COMPILER   $ENV{CC})
ELSE (NOT $ENV{CC} STREQUAL "")
	SET_CCACHE(CMAKE_C_COMPILER   gcc)
ENDIF (NOT $ENV{CC} STREQUAL "")
IF (NOT $ENV{CXX} STREQUAL "")
	SET_CCACHE(CMAKE_CXX_COMPILER   $ENV{CXX})
ELSE (NOT $ENV{CXX} STREQUAL "")
	SET_CCACHE(CMAKE_CXX_COMPILER   g++)
ENDIF (NOT $ENV{CXX} STREQUAL "")

# set compiler flags for "native"
ADD_FLAGS(CMAKE_C_FLAGS -mtune=native)
ADD_FLAGS(CMAKE_CXX_FLAGS -mtune=native)

INCLUDE(cmake/linux.common.cmake)
