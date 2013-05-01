# Try to find the lua 5.2 library development files
# If this fails, build from the contrib repository

SET (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
SET (CONTRIB_LUA "${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs/lua")

INCLUDE (FindLua52)

IF (NOT LUA52_FOUND)
	IF (BUILD_CONTRIB)
		MESSAGE ("Lua 5.2 library not found - downloading and building")
		INCLUDE (CloneContrib)
                SET (LUA_INCLUDE_DIR "${CONTRIB_LUA}/include" CACHE PATH "Path to lua5.2 headers" FORCE)
                SET (LUA_LIBRARIES "${CONTRIB_LUA}/lib/liblua.a"CACHE FILEPATH "Path to lua5.2 library" FORCE)
		INCLUDE (FindLua52)
                ADD_CUSTOM_TARGET (ContribLua ${CONTRIB_LUA}/build.sh --target ${MINGW_MACHINE})
                ADD_DEPENDENCIES (BuildContrib ContribLua)
	ENDIF (BUILD_CONTRIB)
ENDIF (NOT LUA52_FOUND)

