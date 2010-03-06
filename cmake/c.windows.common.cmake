################################################################################
### $Id: c.windows.common.cmake 6122 2010-03-06 12:29:10Z FloSoft $
################################################################################

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

INCLUDE(cmake/windows.common.cmake)
