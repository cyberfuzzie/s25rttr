################################################################################
### $Id: CMakeLists.txt 5486 2009-09-07 15:41:39Z FloSoft $
################################################################################

IF ( NOT "${COMPILEFOR}" STREQUAL "windows" )
	IF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "amd64" )
		ADD_DEFINITIONS(-fPIC)
	ENDIF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "amd64" )
ENDIF ( NOT "${COMPILEFOR}" STREQUAL "windows" )

################################################################################

ADD_LIBRARY(videodrv STATIC VideoDriver.cpp VideoDriver.h VideoInterface.cpp VideoInterface.h ../../version.cpp)
ADD_LIBRARY(audiodrv STATIC AudioDriver.cpp AudioDriver.h AudioInterface.cpp AudioInterface.h Sound.cpp Sound.h Interface.h ../../version.cpp)

################################################################################
