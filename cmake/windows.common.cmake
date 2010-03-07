################################################################################
### $Id: windows.common.cmake 6125 2010-03-07 18:20:41Z FloSoft $
################################################################################

# set compiler flags
FORCE_ADD_FLAGS(CMAKE_C_FLAGS -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer)
FORCE_ADD_FLAGS(CMAKE_CXX_FLAGS -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer)

# bugfix for cygwin
ADD_DEFINITIONS(-D_WIN32 -D__USE_W32_SOCKETS)
