#################################################################################
### $Id: CMakeLists.txt 2904 2007-11-06 15:28:50Z FloSoft $
#################################################################################

IF ("${COMPILEFOR}" STREQUAL "win32" )
	ADD_SUBDIRECTORY( WinAPI ) 
ELSE ("${COMPILEFOR}" STREQUAL "win32" )
	IF (NOT "${COMPILEFOR}" STREQUAL "64bit" )
		ADD_SUBDIRECTORY( GLFW ) 
	ENDIF (NOT "${COMPILEFOR}" STREQUAL "64bit" )
ENDIF ("${COMPILEFOR}" STREQUAL "win32" )

ADD_SUBDIRECTORY( SDL ) 

#################################################################################
