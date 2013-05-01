# Clone the contrib libraries repository into
# contrib/s25rttr-contrib-libs, if this folder
# does not already exist.
# As this is not for development purposes, do
# not clone any history, but only the HEAD

IF (NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs)
    EXECUTE_PROCESS (COMMAND git clone --depth 0
                     git://github.com/S25RTTR-Aux/s25rttr-contrib-libs.git
                     "${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs")
ELSE (NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs)
    EXECUTE_PROCESS (COMMAND git log -n 1 --pretty=format:%H
                     WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs"
                     OUTPUT_VARIABLE contribOldHash)
    EXECUTE_PROCESS (COMMAND git fetch --depth 0
                     WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs")
    EXECUTE_PROCESS (COMMAND git checkout .
                     WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs")
    EXECUTE_PROCESS (COMMAND git log -n 1 --pretty=format:%H
                     WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs"
                     OUTPUT_VARIABLE contribNewHash)
    IF (NOT "${contribOldHash}" STREQUAL "${contribNewHash}")
        EXECUTE_PROCESS (COMMAND git clean -d -f
                         WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs")
    ENDIF (NOT "${contribOldHash}" STREQUAL "${contribNewHash}")
ENDIF (NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs)

