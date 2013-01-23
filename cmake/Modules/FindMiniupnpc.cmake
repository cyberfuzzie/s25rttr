# - Try to find Miniupnpc 
# Once done this will define 
# 
#  Miniupnpc_FOUND - system has Miniupnpc 
#  MINIUPNPC_INCLUDE_DIR - the Miniupnpc include directory 
#  MINIUPNPC_LIBRARY - Link these to use Miniupnpc 
# 

SET(MINIUPNPC_DIR_SEARCH $ENV{MINIUPNPC_ROOT})

FIND_PATH(MINIUPNPC_INCLUDE_DIR miniupnpc.h 
  ${MINIUPNPC_DIR_SEARCH}/include/miniupnpc
  /usr/include/miniupnpc
  /usr/local/include/miniupnpc
) 
 
FIND_LIBRARY(MINIUPNPC_LIBRARY NAMES miniupnpc
  PATHS 
  ${MINIUPNPC_DIR_SEARCH}/lib
  /usr/${LIB_DESTINATION}
  /usr/local/${LIB_DESTINATION}) 
 
IF(MINIUPNPC_INCLUDE_DIR AND MINIUPNPC_LIBRARY) 
   SET(Miniupnpc_FOUND TRUE) 
ENDIF(MINIUPNPC_INCLUDE_DIR AND MINIUPNPC_LIBRARY) 
 
IF(Miniupnpc_FOUND) 
  IF(NOT Miniupnpc_FIND_QUIETLY) 
    MESSAGE(STATUS "Found Miniupnpc: ${MINIUPNPC_LIBRARY}") 
  ENDIF(NOT Miniupnpc_FIND_QUIETLY) 
ELSE(Miniupnpc_FOUND) 
  IF(Miniupnpc_FIND_REQUIRED) 
    MESSAGE(FATAL_ERROR "Could not find Miniupnpc") 
  ENDIF(Miniupnpc_FIND_REQUIRED) 
ENDIF(Miniupnpc_FOUND) 

MARK_AS_ADVANCED(
  MINIUPNPC_INCLUDE_DIR
  MINIUPNPC_LIBRARY
)
