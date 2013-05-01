# Try to find the BZip2 library development files
# If this fails, build from the contrib repository

SET (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
SET (CONTRIB_BZIP2 "${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs/bzip2")

INCLUDE (FindBZip2)

IF (NOT BZIP2_FOUND)
	IF (BUILD_CONTRIB)
		MESSAGE ("BZip2 library not found - downloading and building")
		INCLUDE (CloneContrib)
		SET (BZIP2_FOUND TRUE)
                SET (BZIP2_INCLUDE_DIR "${CONTRIB_BZIP2}/include" CACHE PATH "Path to BZip2 headers" FORCE)
                SET (BZIP2_LIBRARIES "${CONTRIB_BZIP2}/lib/libbz2.a" CACHE FILEPATH "Path to BZip2 library" FORCE)
                INCLUDE (FindBZip2)
                ADD_CUSTOM_TARGET (ContribBzip2 ${CONTRIB_BZIP2}/build.sh --target ${MINGW_MACHINE})
                ADD_DEPENDENCIES (BuildContrib ContribBzip2)
	ENDIF (BUILD_CONTRIB)
ENDIF (NOT BZIP2_FOUND)

