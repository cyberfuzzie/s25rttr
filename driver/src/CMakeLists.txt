################################################################################
### $Id: CMakeLists.txt 6372 2010-04-30 17:45:33Z FloSoft $
################################################################################

IF ( NOT "${COMPILEFOR}" STREQUAL "windows" )
	IF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "x86_64" )
		ADD_DEFINITIONS(-fPIC)
	ENDIF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "x86_64" )
ENDIF ( NOT "${COMPILEFOR}" STREQUAL "windows" )

################################################################################

ADD_LIBRARY(videodrv STATIC VideoDriver.cpp VideoDriver.h VideoInterface.cpp VideoInterface.h ../../build_version.cpp)
ADD_LIBRARY(audiodrv STATIC AudioDriver.cpp AudioDriver.h AudioInterface.cpp AudioInterface.h Sound.cpp Sound.h Interface.h ../../build_version.cpp)

################################################################################
