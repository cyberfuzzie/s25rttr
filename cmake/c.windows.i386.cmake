################################################################################
### $Id: c.windows.i386.cmake 6121 2010-03-06 12:27:48Z FloSoft $
################################################################################

# this one is important
SET(CMAKE_SYSTEM_NAME Windows)
SET(CMAKE_SYSTEM_PROCESSOR i686)

# specify the cross compiler
SET(CMAKE_C_COMPILER   i586-mingw32msvc-gcc)
SET(CMAKE_CXX_COMPILER i586-mingw32msvc-g++)
SET(CMAKE_RANLIB i586-mingw32msvc-ranlib)

# find AR program
FIND_PROGRAM(WINDOWS_AR NAMES i586-mingw32msvc-ar i686-mingw32-ar ar DOC "path to mingw's ar executable")
SET(CMAKE_AR "${WINDOWS_AR}" CACHE PATH "the AR" FORCE)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH "/usr/i586-mingw32msvc")

# set linker flags
SET(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-rpath,${CMAKE_FIND_ROOT_PATH} -L${CMAKE_FIND_ROOT_PATH}/lib  -Wl,-rpath-link,${CMAKE_FIND_ROOT_PATH}")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,-rpath,${CMAKE_FIND_ROOT_PATH} -L${CMAKE_FIND_ROOT_PATH}/lib  -Wl,-rpath-link,${CMAKE_FIND_ROOT_PATH}")

INCLUDE(cmake/c.windows.common.cmake)
