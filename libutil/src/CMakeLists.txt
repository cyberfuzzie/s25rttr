#################################################################################
### $Id: CMakeLists.txt 7521 2011-09-08 20:45:55Z FloSoft $
#################################################################################

INCLUDE(FindOrBuildMiniupnpc)
IF (NOT Miniupnpc_FOUND)
	MESSAGE(FATAL_ERROR "miniupnpc not found")
ENDIF (NOT Miniupnpc_FOUND)

INCLUDE_DIRECTORIES("../../" "../../libendian/src/")
INCLUDE_DIRECTORIES(${MINIUPNPC_INCLUDE_DIR})
LINK_DIRECTORIES("../../libendian/src")

#################################################################################

ADD_LIBRARY(s25util STATIC Log.cpp Socket.cpp SocketSet.cpp MyTime.cpp error.cpp MessageQueue.cpp MessageInterface.cpp Message.cpp Serializer.cpp files.cpp UPnP.cpp BinaryFile.h BinaryFile.cpp)

ADD_DEPENDENCIES (s25util BuildContrib)

IF ( "${COMPILEFOR}" STREQUAL "windows" )
	TARGET_LINK_LIBRARIES(s25util ${MINIUPNPC_LIBRARY} iphlpapi ws2_32 ${TARGET_LINK_LIBRARIES} )
ELSE ( "${COMPILEFOR}" STREQUAL "windows" )
	TARGET_LINK_LIBRARIES(s25util ${MINIUPNPC_LIBRARY} ${TARGET_LINK_LIBRARIES} )
ENDIF ( "${COMPILEFOR}" STREQUAL "windows" )

#################################################################################
