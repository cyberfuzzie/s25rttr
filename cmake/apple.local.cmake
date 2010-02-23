# this one is important
EXECUTE_PROCESS(COMMAND "uname"
        OUTPUT_VARIABLE CMAKE_SYSTEM_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
EXECUTE_PROCESS(COMMAND "uname" "-m"
        OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR OUTPUT_STRIP_TRAILING_WHITESPACE)

SET(CMAKE_C_COMPILER   gcc)
SET(CMAKE_CXX_COMPILER g++)

# set specific compiler flags
IF ( "${COMPILEARCH}" STREQUAL "ppc" )
	# for powerpc
	SET(APPLE_FLAGS "-faltivec")
ELSE( "${COMPILEARCH}" STREQUAL "ppc" )
	# for intel
	SET(APPLE_FLAGS "-mtune=prescott -mmmx -mfpmath=sse -malign-double")
ENDIF ( "${COMPILEARCH}" STREQUAL "ppc" )

SET(CMAKE_OSX_ARCHITECTURES "i686" CACHE STRING "OSX-Architectures")
SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.5" CACHE STRING "OSX-Target")

# set compiler flags
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${APPLE_FLAGS} -ffast-math -msse -fomit-frame-pointer"  CACHE STRING "CFLAGS")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${APPLE_FLAGS} -ffast-math -msse -fomit-frame-pointer"  CACHE STRING "CXXFLAGS")

# set linker flags
SET(CMAKE_C_LINK_FLAGS 	"${CMAKE_C_LINK_FLAGS} -framework OpenGL -L/opt/local/lib -L${CMAKE_SOURCE_DIR}/macos -lSDLmain" CACHE STRING "LDFLAGS (C)")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -framework OpenGL -L/opt/local/lib -L${CMAKE_SOURCE_DIR}/macos -lSDLmain" CACHE STRING "LDFLAGS (C++)")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

