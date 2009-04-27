IF ( "${CROSSCOMPILE}" STREQUAL "1" )
    ADD_DEFINITIONS(-fPIC)
ELSE ( "${CROSSCOMPILE}" STREQUAL "1" )
    EXEC_PROGRAM("uname -m" OUTPUT_VARIABLE ARCH)
    IF ( "${ARCH}" STREQUAL "x86_64" )
        ADD_DEFINITIONS(-fPIC)
    ENDIF ( "${ARCH}" STREQUAL "x86_64" )
ENDIF ( "${CROSSCOMPILE}" STREQUAL "1" )

ADD_LIBRARY(videodrv STATIC VideoDriver.cpp VideoDriver.h VideoInterface.cpp VideoInterface.h ../../version.cpp)

INSTALL(TARGETS videodrv DESTINATION lib)

ADD_LIBRARY(audiodrv STATIC AudioDriver.cpp AudioDriver.h AudioInterface.cpp AudioInterface.h Sound.cpp Sound.h Interface.h ../../version.cpp)

INSTALL(TARGETS audiodrv DESTINATION lib)
