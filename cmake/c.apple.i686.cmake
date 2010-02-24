# this one is important
SET(CMAKE_SYSTEM_NAME Darwin)
SET(CMAKE_SYSTEM_PROCESSOR i686)

# specify the cross compiler
SET(CMAKE_C_COMPILER   i686-apple-darwin10-gcc)
SET(CMAKE_CXX_COMPILER i686-apple-darwin10-g++)
SET(CMAKE_RANLIB i686-apple-darwin10-ranlib)

# set SDK
SET(CMAKE_PREFIX_PATH "/srv/buildfarm/SDKs/MacOSX10.6.sdk")

# set root path
SET(CMAKE_FIND_ROOT_PATH "${CMAKE_PREFIX_PATH}")

# set compiler flags for i686
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=prescott -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer ")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=prescott -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer ")

# set linker flags
SET(CMAKE_C_LINK_FLAGS 	"${CMAKE_C_LINK_FLAGS} -framework OpenGL -L${CMAKE_SOURCE_DIR}/macos -lSDLmain")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -framework OpenGL -L${CMAKE_SOURCE_DIR}/macos -lSDLmain")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
