#################################################################################
### $Id: CMakeLists.txt 5432 2009-08-22 11:18:14Z FloSoft $
#################################################################################

INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-fPIC PIC_FLAG)
IF (PIC_FLAG)
	ADD_DEFINITIONS(-fPIC)
ENDIF (PIC_FLAG)

#################################################################################

ADD_LIBRARY(endian STATIC libendian.cpp read_char.cpp read_int.cpp
            read_short.cpp swap.cpp write_char.cpp write_int.cpp
            write_short.cpp)

#INSTALL(TARGETS endian DESTINATION lib)
#INSTALL(FILES DESTINATION /include/libsiedler2/libendian)

#################################################################################
