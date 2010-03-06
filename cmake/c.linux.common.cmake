################################################################################
### $Id: CMakeLists.txt 6087 2010-02-25 09:01:36Z FloSoft $
################################################################################

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# set compiler flags
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=generic")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic")

INCLUDE(cmake/linux.common.cmake)