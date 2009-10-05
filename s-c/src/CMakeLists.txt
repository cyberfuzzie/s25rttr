#################################################################################
### $Id: CMakeLists.txt 5593 2009-10-05 16:05:20Z FloSoft $
#################################################################################

ADD_EXECUTABLE(sound-convert getopt.cpp getopt_long.cpp main.cpp tempname.cpp)
		
IF ( "${COMPILEFOR}" STREQUAL "windows")
	ADD_CUSTOM_COMMAND(TARGET sound-convert POST_BUILD COMMAND if [ -f sound-convert.exe ] \; then cp -v sound-convert.exe sound-convert \; fi)
ENDIF ( "${COMPILEFOR}" STREQUAL "windows")
		
ADD_CUSTOM_COMMAND(TARGET sound-convert POST_BUILD COMMAND cp -v sound-convert ../../RTTR/sound-convert)
		
INSTALL(TARGETS sound-convert DESTINATION ${DATADIR}/RTTR)

#################################################################################
