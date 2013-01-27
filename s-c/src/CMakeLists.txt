#################################################################################
### $Id: CMakeLists.txt 6942 2010-12-24 07:50:41Z FloSoft $
#################################################################################

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/libsiedler2/src")

ADD_EXECUTABLE(sound-convert getopt.cpp getopt_long.cpp main.cpp tempname.cpp)

TARGET_LINK_LIBRARIES(sound-convert siedler2)
		
IF ( "${COMPILEFOR}" STREQUAL "windows")
	ADD_CUSTOM_COMMAND(TARGET sound-convert POST_BUILD COMMAND if [ -f sound-convert.exe ] \; then cp -v sound-convert.exe sound-convert \; fi)
ENDIF ( "${COMPILEFOR}" STREQUAL "windows")
		
INCLUDE (CMakeMacroProcessDebugInfo)
PROCESS_DEBUG_INFO (sound-convert ${DATADIR}/RTTR)

INSTALL(TARGETS sound-convert DESTINATION ${DATADIR}/RTTR)

#################################################################################
