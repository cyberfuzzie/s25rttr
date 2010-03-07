#################################################################################
### $Id: CMakeMacroEnsureOutOfSourceBuild.cmake 6125 2010-03-07 18:20:41Z FloSoft $
#################################################################################

MACRO (ENSURE_OUT_OF_SOURCE_BUILD error)
	STRING(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" source)

	IF (source)
		MESSAGE(SEND_ERROR "${error}")
		MESSAGE(FATAL_ERROR "Remove the file CMakeCache.txt in ${CMAKE_SOURCE_DIR} first.")
	ENDIF (source)
ENDMACRO (ENSURE_OUT_OF_SOURCE_BUILD)
