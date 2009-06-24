# this one is important
SET(CMAKE_SYSTEM_NAME Darwin)
SET(CMAKE_SYSTEM_PROCESSOR i686)

# specify the cross compiler
SET(CMAKE_C_COMPILER   i686-apple-darwin9-gcc)
SET(CMAKE_CXX_COMPILER i686-apple-darwin9-g++)
SET(CMAKE_RANLIB i686-apple-darwin9-ranlib)

SET(CMAKE_PREFIX_PATH "/usr/i686-apple-darwin9")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I/System/Library/Frameworks/SDL.framework/Headers -I/System/Library/Frameworks/SDL_mixer.framework/Headers -mtune=prescott -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer ")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/System/Library/Frameworks/SDL.framework/Headers -I/System/Library/Frameworks/SDL_mixer.framework/Headers -mtune=prescott -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer ")

# set rpath correctly - fucking CMAKE!
SET(CMAKE_C_LINK_FLAGS 	"${CMAKE_C_LINK_FLAGS} -L/usr/i686-apple-darwin9/lib -framework OpenGL -framework SDL -framework SDL_mixer")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L/usr/i686-apple-darwin9/lib -framework OpenGL -framework SDL -framework SDL_mixer")

SET(CMAKE_FIND_ROOT_PATH "/usr/i686-apple-darwin9")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
