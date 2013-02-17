#################################################################################
### $Id: CMakeLists.txt 6461 2010-05-31 11:46:20Z FloSoft $
#################################################################################

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

ADD_EXECUTABLE(s-c_resample resample.c filterkit.c resamplesubs.c sndlibextra.c)

TARGET_LINK_LIBRARIES(s-c_resample resample-sndlib-20 m)

IF ( "${COMPILEFOR}" STREQUAL "windows")
	ADD_CUSTOM_COMMAND(TARGET s-c_resample POST_BUILD COMMAND if [ -f s-c_resample.exe ] \; then cp -v s-c_resample.exe s-c_resample \; fi)
ENDIF ( "${COMPILEFOR}" STREQUAL "windows")
		
INCLUDE (CMakeMacroProcessDebugInfo)
PROCESS_DEBUG_INFO (s-c_resample ${DATADIR}/RTTR)

INSTALL(TARGETS s-c_resample DESTINATION ${DATADIR}/RTTR)

#################################################################################
