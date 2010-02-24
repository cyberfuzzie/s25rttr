# specify the cross compiler
SET(CMAKE_C_COMPILER   i686-apple-darwin10-gcc CACHE STRING "CC")
SET(CMAKE_CXX_COMPILER i686-apple-darwin10-g++ CACHE STRING "CPP")
SET(CMAKE_RANLIB i686-apple-darwin10-ranlib CACHE STRING "RANLIB" FORCE)
SET(CMAKE_LIPO i686-apple-darwin10-lipo CACHE STRING "LIPO" FORCE)

SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_LIPO> -create <TARGET> <LINK_FLAGS> <OBJECTS>")
SET(CMAKE_CXX_ARCHIVE_APPEND "<CMAKE_LIPO> -replace  <TARGET> <LINK_FLAGS> <OBJECTS>")

# set OSX-Version
SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.5" CACHE STRING "OSX-Target")

# set SDK
SET(CMAKE_PREFIX_PATH "/srv/buildfarm/SDKs/MacOSX10.6.sdk")

# set root path
SET(CMAKE_FIND_ROOT_PATH "${CMAKE_PREFIX_PATH}")

IF ( NOT "${COMPILEARCH}" STREQUAL "" )

	# set specific compiler and architecture flags
	IF ( "${COMPILEARCH}" STREQUAL "x86_64" AND "${CMAKE_OSX_ARCHITECTURES}" MATCHES "x86_64" )
		MESSAGE(STATUS "building universal binary with i386 x86_64")
		# for intel 32/64 universal
		SET(APPLE_FLAGS "-arch i386 -arch x86_64" CACHE STRING "Apple Flags")
		SET(APPLE_LDFLAGS "-arch i386 -arch x86_64" CACHE STRING "Apple Link Flags")
	ELSE ( "${COMPILEARCH}" STREQUAL "x86_64" AND "${CMAKE_OSX_ARCHITECTURES}" MATCHES "x86_64" )
		MESSAGE(STATUS "building non-fat binary")
		IF ( "${COMPILEARCH}" STREQUAL "i386" )
			# for intel
			SET(APPLE_FLAGS "-arch i386 -mtune=prescott -mmmx -mfpmath=sse -malign-double" CACHE STRING "Apple Flags")
			SET(APPLE_LDFLAGS "-arch i386" CACHE STRING "Apple Link Flags")
		ELSE ( "${COMPILEARCH}" STREQUAL "i386" )
			IF ( "${COMPILEARCH}" STREQUAL "ppc" )
				# for powerpc
				SET(APPLE_FLAGS "-faltivec" CACHE STRING "Apple Flags")
				SET(APPLE_LDFLAGS "" CACHE STRING "Apple Link Flags")
			ENDIF ( "${COMPILEARCH}" STREQUAL "ppc" )
		ENDIF ( "${COMPILEARCH}" STREQUAL "i386" )
	ENDIF ( "${COMPILEARCH}" STREQUAL "x86_64" AND "${CMAKE_OSX_ARCHITECTURES}" MATCHES "x86_64" )

	# set compiler flags
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${APPLE_FLAGS} -ffast-math -msse -fomit-frame-pointer"  CACHE STRING "CFLAGS" FORCE)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${APPLE_FLAGS} -ffast-math -msse -fomit-frame-pointer"  CACHE STRING "CXXFLAGS" FORCE)
	
	# set linker flags
	SET(CMAKE_C_LINK_FLAGS 	"${CMAKE_C_LINK_FLAGS} ${APPLE_LDFLAGS} -framework OpenGL -L${CMAKE_SOURCE_DIR}/macos -lSDLmain" CACHE STRING "LDFLAGS (C)")
	SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} ${APPLE_LDFLAGS} -framework OpenGL -L${CMAKE_SOURCE_DIR}/macos -lSDLmain" CACHE STRING "LDFLAGS (C++)")

ENDIF ( NOT "${COMPILEARCH}" STREQUAL "" )

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
