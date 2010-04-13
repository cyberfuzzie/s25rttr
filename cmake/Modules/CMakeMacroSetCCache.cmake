################################################################################
### $Id: CMakeMacroAddFlags.cmake 6129 2010-03-07 19:10:49Z FloSoft $
################################################################################

MACRO(SET_CCACHE var file)
	IF(EXISTS /usr/lib/ccache/${file})
		SET(${var} /usr/lib/ccache/${file})
	ELSE(EXISTS /usr/lib/ccache/${file})
		SET(${var} ${file})
	ENDIF(EXISTS /usr/lib/ccache/${file})
ENDMACRO(SET_CCACHE)

