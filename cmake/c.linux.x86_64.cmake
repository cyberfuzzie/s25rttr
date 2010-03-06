################################################################################
### $Id: c.linux.x86_64.cmake 6121 2010-03-06 12:27:48Z FloSoft $
################################################################################

# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR x86_64)

# specify the cross compiler
SET(CMAKE_C_COMPILER   x86_64-pc-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER x86_64-pc-linux-gnu-g++)
SET(CMAKE_RANLIB x86_64-pc-linux-gnu-ranlib)

# set search prefix
SET(CMAKE_PREFIX_PATH "/usr/x86_64-pc-linux-gnu")

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH "/usr/x86_64-pc-linux-gnu")

# set linker flags
SET(CMAKE_C_LINK_FLAGS 	"${CMAKE_C_LINK_FLAGS} -Wl,-rpath,${CMAKE_FIND_ROOT_PATH} -L${CMAKE_FIND_ROOT_PATH}/lib -L${CMAKE_FIND_ROOT_PATH}/lib64 -L/lib64 -L/usr/lib64 -Wl,-rpath-link,${CMAKE_FIND_ROOT_PATH}")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,-rpath,${CMAKE_FIND_ROOT_PATH} -L${CMAKE_FIND_ROOT_PATH}/lib -L${CMAKE_FIND_ROOT_PATH}/lib64 -L/lib64 -L/usr/lib64 -Wl,-rpath-link,${CMAKE_FIND_ROOT_PATH}")

INCLUDE(cmake/c.linux.common.cmake)
