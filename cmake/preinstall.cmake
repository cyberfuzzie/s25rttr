###############################################################################
## $Id: preinstall.cmake 5627 2009-10-12 17:17:06Z FloSoft $
###############################################################################

MESSAGE(STATUS "Running preinstall script")

# run install script
EXECUTE_PROCESS(
  COMMAND bash "preinstall.sh"
  RESULT_VARIABLE AD_HDM_RV
  )
IF(NOT "${AD_HDM_RV}" STREQUAL "0")
        MESSAGE(FATAL_ERROR "ERROR: Preinstallscript failed: Maybe you need administrative privileges?")
ENDIF(NOT "${AD_HDM_RV}" STREQUAL "0")

MESSAGE(STATUS "Done")

###############################################################################
