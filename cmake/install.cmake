###############################################################################
## $Id$
###############################################################################

# run install script
EXECUTE_PROCESS(
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../build/install.sh" "--prefix=${PREFIX}" "--iprefix=${CMAKE_INSTALL_PREFIX}" "--bindir=${BINDIR}" "--datadir=${DATADIR}"
  RESULT_VARIABLE AD_HDM_RV
  )
IF(NOT "${AD_HDM_RV}" STREQUAL "0")
        MESSAGE(FATAL_ERROR "ERROR: Preinstallscript failed: Maybe you need administrative privileges?")
ENDIF(NOT "${AD_HDM_RV}" STREQUAL "0")

###############################################################################
