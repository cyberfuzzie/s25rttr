# Try to find the CURL library development files
# If this fails, build from the contrib repository

SET (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
SET (CONTRIB_CURL "${CMAKE_SOURCE_DIR}/contrib/s25rttr-contrib-libs/curl")

INCLUDE (FindCURL)

IF (NOT CURL_FOUND)
	IF (BUILD_CONTRIB)
		MESSAGE ("CURL library not found - downloading and building")
		INCLUDE (CloneContrib)
                SET (CURL_INCLUDE_DIR "${CONTRIB_CURL}/include" CACHE PATH "Path to Curl headers" FORCE)
                SET (CURL_LIBRARY "${CONTRIB_CURL}/lib/libcurl.dll.a" CACHE FILEPATH "Path to Curl library" FORCE)
		INCLUDE (FindCURL)
                ADD_CUSTOM_TARGET (ContribCurl ${CONTRIB_CURL}/build.sh --target ${MINGW_MACHINE})
                ADD_DEPENDENCIES (BuildContrib ContribCurl)
	ENDIF (BUILD_CONTRIB)
ENDIF (NOT CURL_FOUND)

