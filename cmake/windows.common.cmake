################################################################################
### $Id: windows.common.cmake 6122 2010-03-06 12:29:10Z FloSoft $
################################################################################

# set compiler flags for "native"
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer")

# bugfix for cygwin
ADD_DEFINITIONS(-D_WIN32 -D__USE_W32_SOCKETS)
