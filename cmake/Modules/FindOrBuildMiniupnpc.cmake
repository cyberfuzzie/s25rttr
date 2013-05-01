# Try to find the miniupnpc library development files
# If this fails, build from the contrib repository

SET (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
SET (CONTRIB_MINIUPNPC "${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs/miniupnpc")

INCLUDE (FindMiniupnpc)

IF (NOT Miniupnpc_FOUND)
	IF (BUILD_CONTRIB)
		MESSAGE ("Miniupnpc library not found - downloading and building")
		INCLUDE (CloneContrib)
                SET (MINIUPNPC_INCLUDE_DIR "${CONTRIB_MINIUPNPC}/include" CACHE PATH "Path to miniupnpc headers" FORCE)
                SET (MINIUPNPC_LIBRARY "${CONTRIB_MINIUPNPC}/lib/miniupnpc.lib" CACHE FILEPATH "Path to miniupnpc library" FORCE)
		INCLUDE (FindMiniupnpc)
                ADD_CUSTOM_TARGET (ContribMiniupnpc ${CONTRIB_MINIUPNPC}/build.sh --target ${MINGW_MACHINE})
                ADD_DEPENDENCIES (BuildContrib ContribMiniupnpc)
	ENDIF (BUILD_CONTRIB)
ENDIF (NOT Miniupnpc_FOUND)

