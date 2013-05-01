
# if cross compiling, set library prefixes and suffixes
# necessary, because libraries are named as on the host
# has to be done after PROJECT() call
# but compiler setting has to be done before PROJECT()
IF (${CROSSCOMPILE} STREQUAL "1")
	SET(CMAKE_FIND_LIBRARY_PREFIXES ${CROSS_LIBRARY_PREFIXES})
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ${CROSS_LIBRARY_SUFFIXES})
ENDIF (${CROSSCOMPILE} STREQUAL "1")

