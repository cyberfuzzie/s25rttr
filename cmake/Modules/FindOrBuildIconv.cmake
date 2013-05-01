# Try to find the Iconv library development files
# If this fails, build from the contrib repository

SET (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
SET (CONTRIB_ICONV "${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs/libiconv")

INCLUDE (FindIconv)

IF (NOT Iconv_FOUND)
	IF (BUILD_CONTRIB)
		MESSAGE ("Iconv library not found - downloading and building")
		INCLUDE (CloneContrib)
                SET (ICONV_INCLUDE_DIR "${CONTRIB_ICONV}/include" CACHE PATH "Path to iconv headers" FORCE)
                SET (ICONV_LIBRARY "${CONTRIB_ICONV}/lib/libiconv.dll.a" CACHE FILEPATH "Path to iconv library" FORCE)
                INCLUDE (FindIconv)
                ADD_CUSTOM_TARGET (ContribIconv ${CONTRIB_ICONV}/build.sh --target ${MINGW_MACHINE})
                ADD_DEPENDENCIES (BuildContrib ContribIconv)
	ENDIF (BUILD_CONTRIB)
ENDIF (NOT Iconv_FOUND)

