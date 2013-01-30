################################################################################
### $Id: apple.local.cmake 6807 2010-10-18 14:12:04Z FloSoft $
################################################################################

# this one is important
EXECUTE_PROCESS(COMMAND "uname" OUTPUT_VARIABLE CMAKE_SYSTEM_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
EXECUTE_PROCESS(COMMAND "uname" "-m" OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR OUTPUT_STRIP_TRAILING_WHITESPACE)

IF ($ENV{CC})
	SET_CCACHE(CMAKE_C_COMPILER   $ENV{CC})
ELSE ($ENV{CC})
	SET_CCACHE(CMAKE_C_COMPILER   gcc)
ENDIF ($ENV{CC})
IF ($ENV{CXX})
	SET_CCACHE(CMAKE_CXX_COMPILER   $ENV{CXX})
ELSE ($ENV{CXX})
	SET_CCACHE(CMAKE_CXX_COMPILER   g++)
ENDIF ($ENV{CXX})

INCLUDE(cmake/apple.common.cmake)

