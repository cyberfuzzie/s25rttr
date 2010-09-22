################################################################################
### $Id: CMakeMacroAddFlags.cmake 6129 2010-03-07 19:10:49Z FloSoft $
################################################################################

MACRO(SET_CCACHE var file)
	IF(EXISTS /usr/lib/ccache_lipo/${file})
		SET(${var} /usr/lib/ccache_lipo/${file})
	ELSE(EXISTS /usr/lib/ccache_lipo/${file})
		IF(EXISTS /usr/lib/ccache/${file})
			SET(${var} /usr/lib/ccache/${file})
		ELSE(EXISTS /usr/lib/ccache/${file})
			IF(EXISTS /opt/local/libexec/ccache/${file})
				SET(${var} /opt/local/libexec/ccache/${file})
			ELSE(EXISTS /opt/local/libexec/ccache/${file})
				SET(${var} ${file})
			ENDIF(EXISTS /opt/local/libexec/ccache/${file})
		ENDIF(EXISTS /usr/lib/ccache/${file})
	ENDIF(EXISTS /usr/lib/ccache_lipo/${file})
	
	MESSAGE(STATUS "Set ${var} to ${${var}}")
ENDMACRO(SET_CCACHE)

