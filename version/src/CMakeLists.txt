################################################################################
### $Id: CMakeLists.txt 6093 2010-02-26 07:17:24Z FloSoft $
################################################################################

IF ("${CROSSCOMPILE}" STREQUAL "1" )
	MESSAGE(STATUS "version: Reverting to host compiler to avoid \"cannot execute binary file\"")
	SET(CMAKE_C_COMPILER "gcc")
	SET(CMAKE_CXX_COMPILER "g++")
	SET(CMAKE_C_FLAGS "")
	SET(CMAKE_CXX_FLAGS "")
	SET(CMAKE_C_LINK_FLAGS "")
	SET(CMAKE_CXX_LINK_FLAGS "-lz")
	SET(CMAKE_CXX_STANDARD_LIBRARIES "")
	
	SET(CMAKE_EXECUTABLE_SUFFIX "")
	
	SET(CMAKE_GNULD_IMAGE_VERSION "")
	
	SET(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")
	SET(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER>  <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")

ENDIF ("${CROSSCOMPILE}" STREQUAL "1" )

################################################################################

SET(version_SRCS main.cpp)

ADD_EXECUTABLE(version ${version_SRCS})

################################################################################
