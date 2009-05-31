# this one is important
EXECUTE_PROCESS(COMMAND "uname"
	OUTPUT_VARIABLE CMAKE_SYSTEM_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
EXECUTE_PROCESS(COMMAND "uname" "-m"
	OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR OUTPUT_STRIP_TRAILING_WHITESPACE)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=native -malign-double -ffast-math -mmmx -msse -mfpmath=sse")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=native -malign-double -ffast-math -mmmx -msse -mfpmath=sse")

# specify the compiler
SET(CMAKE_C_COMPILER   gcc)
SET(CMAKE_CXX_COMPILER g++)
