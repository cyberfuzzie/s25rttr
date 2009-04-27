# this one is important
SET(CMAKE_SYSTEM_NAME Windows)
SET(CMAKE_SYSTEM_PROCESSOR i686)

# specify the cross compiler
SET(CMAKE_C_COMPILER   i586-mingw32msvc-gcc)
SET(CMAKE_CXX_COMPILER i586-mingw32msvc-g++)
SET(CMAKE_RANLIB i586-mingw32msvc-ranlib)
SET(CMAKE_AR i586-mingw32msvc-ar)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /usr/i586-mingw32msvc )

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=pentium -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer ")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=pentium -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer ")

# set rpath correctly - fucking CMAKE!
SET(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-rpath,/usr/i586-mingw32msvc -L/usr/i586-mingw32msvc/lib  -Wl,-rpath-link,/usr/i586-mingw32msvc")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,-rpath,/usr/i586-mingw32msvc -L/usr/i586-mingw32msvc/lib  -Wl,-rpath-link,/usr/i586-mingw32msvc")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
