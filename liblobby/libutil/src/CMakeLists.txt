#################################################################################
### $Id: CMakeLists.txt 4639 2009-03-27 18:29:31Z FloSoft $
#################################################################################

IF ( "${CROSSCOMPILE}" STREQUAL "1" )
    ADD_DEFINITIONS(-fPIC)
ELSE ( "${CROSSCOMPILE}" STREQUAL "1" )
    INCLUDE(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(-fPIC PIC_FLAG)
    IF (PIC_FLAG)
        ADD_DEFINITIONS(-fPIC)
    ENDIF (PIC_FLAG)
ENDIF ( "${CROSSCOMPILE}" STREQUAL "1" )

#################################################################################

INCLUDE_DIRECTORIES("../../")

#################################################################################

ADD_LIBRARY(s25util STATIC Log.cpp Socket.cpp SocketSet.cpp MyTime.cpp error.cpp MessageQueue.cpp SerializableArray.cpp Messages.cpp MessageInterface.cpp Message.cpp)

IF ( "${COMPILEFOR}" STREQUAL "win32" )
    ADD_DEFINITIONS("-D_WIN32_WINNT=0x0501")
	
    TARGET_LINK_LIBRARIES(s25util ws2_32 ${TARGET_LINK_LIBRARIES} )
ENDIF ( "${COMPILEFOR}" STREQUAL "win32" )

INSTALL(TARGETS s25util DESTINATION lib)
INSTALL(FILES DESTINATION /include/libs25util)

#################################################################################
