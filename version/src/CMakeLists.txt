################################################################################
### $Id: CMakeLists.txt 5100 2009-06-24 17:11:59Z FloSoft $
################################################################################

IF ("${CROSSCOMPILE}" STREQUAL "1" )
	MESSAGE(STATUS "version: Reverting to host compiler to avoid \"cannot execute binary file\"")
	SET(CMAKE_C_COMPILER "gcc")
	SET(CMAKE_CXX_COMPILER "g++")
	SET(CMAKE_C_LINK_FLAGS "")
	SET(CMAKE_CXX_LINK_FLAGS "-lz")
	
	SET(CMAKE_EXECUTABLE_SUFFIX "")
	
	SET(CMAKE_GNULD_IMAGE_VERSION "")
	
	SET(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")
	SET(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER>  <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")

ENDIF ("${CROSSCOMPILE}" STREQUAL "1" )

################################################################################

SET(version_SRCS main.cpp)

ADD_EXECUTABLE(version ${version_SRCS})

################################################################################
