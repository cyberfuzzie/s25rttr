#################################################################################
### $Id: CMakeLists.txt 5593 2009-10-05 16:05:20Z FloSoft $
#################################################################################

ADD_EXECUTABLE(s-c_resample resample.c filterkit.c resamplesubs.c sndlibextra.c windowfilter.c)

TARGET_LINK_LIBRARIES(s-c_resample resample-sndlib-20)

IF ( "${COMPILEFOR}" STREQUAL "windows")
	ADD_CUSTOM_COMMAND(TARGET sound-convert POST_BUILD COMMAND if [ -f s-c_resample.exe ] \; then cp -v s-c_resample.exe s-c_resample \; fi)
ENDIF ( "${COMPILEFOR}" STREQUAL "windows")
		
ADD_CUSTOM_COMMAND(TARGET s-c_resample POST_BUILD COMMAND cp -v s-c_resample ../../RTTR/s-c_resample)
		
INSTALL(TARGETS s-c_resample DESTINATION ${DATADIR}/RTTR)

#################################################################################
