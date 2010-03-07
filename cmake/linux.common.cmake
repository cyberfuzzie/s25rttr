################################################################################
### $Id: linux.common.cmake 6125 2010-03-07 18:20:41Z FloSoft $
################################################################################

# set compiler flags
ADD_FLAGS(CMAKE_C_FLAGS -ffast-math -mmmx -msse -mfpmath=sse)
ADD_FLAGS(CMAKE_CXX_FLAGS -ffast-math -mmmx -msse -mfpmath=sse)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
