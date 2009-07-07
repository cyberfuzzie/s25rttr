# this one is important
EXECUTE_PROCESS(COMMAND "uname"
        OUTPUT_VARIABLE CMAKE_SYSTEM_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
EXECUTE_PROCESS(COMMAND "uname" "-m"
        OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR OUTPUT_STRIP_TRAILING_WHITESPACE)

SET(CMAKE_C_COMPILER   gcc)
SET(CMAKE_CXX_COMPILER g++)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=prescott -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer ")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=prescott -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer ")

# set rpath correctly - fucking CMAKE!
SET(CMAKE_C_LINK_FLAGS 	"${CMAKE_C_LINK_FLAGS} -framework OpenGL -L/opt/local/lib -L${CMAKE_SOURCE_DIR}/macos -lSDLmain")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -framework OpenGL -L/opt/local/lib -L${CMAKE_SOURCE_DIR}/macos -lSDLmain")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
