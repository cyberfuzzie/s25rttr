################################################################################
### $Id: CMakeLists.txt 4512 2009-02-04 08:34:58Z FloSoft $
################################################################################

INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-fPIC PIC_FLAG)
IF (PIC_FLAG)
    ADD_DEFINITIONS(-fPIC)
ENDIF (PIC_FLAG)

################################################################################

SET(SOURCES_LOBBY LobbyMessage.cpp          LobbyMessage.h
                  LobbyMessages.cpp         LobbyMessages.h
                  LobbyMessageInterface.cpp LobbyMessageInterface.h
                  LobbyInterface.cpp        LobbyInterface.h
                  LobbyPlayerInfo.cpp       LobbyPlayerInfo.h
                  LobbyPlayerList.cpp       LobbyPlayerList.h
                  LobbyServerInfo.cpp       LobbyServerInfo.h
                  LobbyServerList.cpp       LobbyServerList.h
   )

################################################################################

INCLUDE_DIRECTORIES("../../" "../../libutil/src/" "../../mygettext/src/")

################################################################################

IF (LOBBY_C)

    ADD_LIBRARY(lobby_c STATIC ${SOURCES_LOBBY} LobbyClient.cpp LobbyClient.h)
    TARGET_LINK_LIBRARIES(lobby_c util mygettext)
    
    INSTALL(TARGETS lobby_c DESTINATION lib)

ENDIF (LOBBY_C)

################################################################################

IF (LOBBY_S)

    ADD_LIBRARY(lobby_s STATIC ${SOURCES_LOBBY} LobbyServer.cpp LobbyServer.h  LobbyPlayer.cpp LobbyPlayer.h MySQL.cpp MySQL.h)
    TARGET_LINK_LIBRARIES(lobby_s util mygettext mysqlclient)

    INSTALL(TARGETS lobby_s DESTINATION lib)

ENDIF (LOBBY_S)

################################################################################
