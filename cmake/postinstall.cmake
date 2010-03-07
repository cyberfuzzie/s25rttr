###############################################################################
## $Id: postinstall.cmake 6125 2010-03-07 18:20:41Z FloSoft $
###############################################################################

MESSAGE(STATUS "Running postinstall script")

# run install script
EXECUTE_PROCESS(
	COMMAND bash "postinstall.sh"
	RESULT_VARIABLE POSTINSTALL_RESULT
)

IF(NOT "${POSTINSTALL_RESULT}" STREQUAL "0")
	MESSAGE(FATAL_ERROR "ERROR: Postinstallscript failed: Maybe you need administrative privileges?")
ENDIF(NOT "${POSTINSTALL_RESULT}" STREQUAL "0")

MESSAGE(STATUS "Done")

###############################################################################
