#################################################################################
### $Id: CMakeLists.txt 5487 2009-09-07 15:44:27Z FloSoft $
#################################################################################

IF ( NOT "${COMPILEFOR}" STREQUAL "windows" )
	IF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "amd64" )
		ADD_DEFINITIONS(-fPIC)
	ENDIF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "amd64" )
ENDIF ( NOT "${COMPILEFOR}" STREQUAL "windows" )

#################################################################################

INCLUDE_DIRECTORIES("../../")

#################################################################################

ADD_LIBRARY(s25util STATIC Log.cpp Socket.cpp SocketSet.cpp MyTime.cpp error.cpp MessageQueue.cpp SerializableArray.cpp Messages.cpp MessageInterface.cpp Message.cpp Serializer.cpp files.cpp)

IF ( "${COMPILEFOR}" STREQUAL "windows" )
    ADD_DEFINITIONS("-D_WIN32_WINNT=0x0501")
    ADD_DEFINITIONS("-D__MINGW__")
    
    TARGET_LINK_LIBRARIES(s25util ws2_32 ${TARGET_LINK_LIBRARIES} )
ENDIF ( "${COMPILEFOR}" STREQUAL "windows" )

#################################################################################
