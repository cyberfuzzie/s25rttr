################################################################################
### $Id: CMakeLists.txt 7005 2011-01-27 22:01:07Z FloSoft $
################################################################################

INCLUDE(FindOrBuildCURL)
IF (NOT CURL_FOUND)
	MESSAGE(FATAL_ERROR "Curl not found")
ENDIF (NOT CURL_FOUND)
INCLUDE_DIRECTORIES ("${CURL_INCLUDE_DIR}")

INCLUDE(FindOrBuildBZip2)
IF (NOT BZIP2_FOUND)
	MESSAGE(FATAL_ERROR "BZip2 not found")
ENDIF (NOT BZIP2_FOUND)
ADD_DEFINITIONS(${BZIP2_DEFINITIONS})
INCLUDE_DIRECTORIES ("${BZIP2_INCLUDE_DIR}")

################################################################################

SET(s25update_SRCS main.cpp md5.cpp md5sum.cpp)

ADD_EXECUTABLE(s25update ${s25update_SRCS})

ADD_DEPENDENCIES (s25update BuildContrib)

ADD_DEFINITIONS("-DTARGET=\"${COMPILEFOR}\"")
ADD_DEFINITIONS("-DARCH=\"${COMPILEARCH}\"")

TARGET_LINK_LIBRARIES(s25update ${CURL_LIBRARIES} ${BZIP2_LIBRARIES})

IF ( "${COMPILEFOR}" STREQUAL "windows")
	ADD_CUSTOM_COMMAND(TARGET s25update POST_BUILD COMMAND if [ -f s25update.exe ] \; then cp -v s25update.exe s25update \; fi)
ENDIF ( "${COMPILEFOR}" STREQUAL "windows")

INCLUDE (CMakeMacroProcessDebugInfo)
PROCESS_DEBUG_INFO (s25update ${DATADIR}/RTTR)

INSTALL(TARGETS s25update RUNTIME DESTINATION ${DATADIR}/RTTR/)

################################################################################
