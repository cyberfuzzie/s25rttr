################################################################################
### $Id: windows.common.cmake 6205 2010-03-28 20:35:03Z FloSoft $
################################################################################

# set compiler flags
FORCE_ADD_FLAGS(CMAKE_C_FLAGS -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer)
FORCE_ADD_FLAGS(CMAKE_CXX_FLAGS -malign-double -ffast-math -mmmx -msse -mfpmath=sse -fomit-frame-pointer)

# bugfix for cygwin
#ADD_DEFINITIONS(-D_WIN32 -D__USE_W32_SOCKETS)

FORCE_ADD_FLAGS(CMAKE_C_FLAGS -D_WIN32 -D__USE_W32_SOCKETS)
FORCE_ADD_FLAGS(CMAKE_CXX_FLAGS -D_WIN32 -D__USE_W32_SOCKETS)
