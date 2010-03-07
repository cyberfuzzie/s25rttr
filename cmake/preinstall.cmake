###############################################################################
## $Id: preinstall.cmake 6125 2010-03-07 18:20:41Z FloSoft $
###############################################################################

MESSAGE(STATUS "Running preinstall script")

# run install script
EXECUTE_PROCESS(
	COMMAND bash "preinstall.sh"
	RESULT_VARIABLE PREINSTALL_RESULT
)

IF(NOT "${PREINSTALL_RESULT}" STREQUAL "0")
	MESSAGE(FATAL_ERROR "ERROR: Preinstallscript failed: Maybe you need administrative privileges?")
ENDIF(NOT "${PREINSTALL_RESULT}" STREQUAL "0")

MESSAGE(STATUS "Done")

###############################################################################
