# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR i686)

# specify the cross compiler
SET(CMAKE_C_COMPILER   i486-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER i486-linux-gnu-g++)
SET(CMAKE_RANLIB i486-linux-gnu-ranlib)

# set compiler flags for i686
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=generic -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
