# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR x86_64)

# specify the cross compiler
SET(CMAKE_C_COMPILER   x86_64-pc-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER x86_64-pc-linux-gnu-g++)
SET(CMAKE_RANLIB x86_64-pc-linux-gnu-ranlib)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /usr/x86_64-pc-linux-gnu )

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=generic -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer ")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer ")

# set rpath correctly - fucking CMAKE!
SET(CMAKE_C_LINK_FLAGS 	"${CMAKE_C_LINK_FLAGS} -Wl,-rpath,/usr/x86_64-pc-linux-gnu -L/usr/x86_64-pc-linux-gnu/lib -L/usr/x86_64-pc-linux-gnu/lib64 -L/lib64 -L/usr/lib64 -Wl,-rpath-link,/usr/x86_64-pc-linux-gnu")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,-rpath,/usr/x86_64-pc-linux-gnu -L/usr/x86_64-pc-linux-gnu/lib -L/usr/x86_64-pc-linux-gnu/lib64 -L/lib64 -L/usr/lib64 -Wl,-rpath-link,/usr/x86_64-pc-linux-gnu")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
