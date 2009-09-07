#################################################################################
### $Id: crosscompile.cmake 5486 2009-09-07 15:41:39Z FloSoft $
#################################################################################

# read host compiler machine triplet

IF(NOT COMPILEFOR_PLATFORM)
	SET(COMPILEFOR_PLATFORM "local")
ENDIF(NOT COMPILEFOR_PLATFORM)

MESSAGE(STATUS "Checking for ${COMPILEFOR_PLATFORM}")

INCLUDE(cmake/${COMPILEFOR_PLATFORM}.cmake OPTIONAL RESULT_VARIABLE FOUND_A)
IF(FOUND_A)
	MESSAGE(STATUS "Reading specifications from ${FOUND_A}")
ENDIF(FOUND_A)

INCLUDE(cmake/${COMPILEFOR_PLATFORM}.local.cmake OPTIONAL RESULT_VARIABLE FOUND_B)
IF(FOUND_B)
	MESSAGE(STATUS "Reading specifications from ${FOUND_A}")
ENDIF(FOUND_B)

IF (NOT FOUND_A AND NOT FOUND_B)
	MESSAGE(FATAL_ERROR " Platform specific include file(s) not found")
ENDIF (NOT FOUND_A AND NOT FOUND_B)

#################################################################################

EXECUTE_PROCESS(
  COMMAND "gcc" "-dumpmachine"
  RESULT_VARIABLE AD_HDM_RV
  ERROR_VARIABLE AD_HDM_EV
  OUTPUT_VARIABLE AD_HDM_OV
  )
IF(NOT "${AD_HDM_RV}" STREQUAL "0")
	MESSAGE(FATAL_ERROR "ERROR: gcc -dumpmachine failed... AD_HDM_RV='${AD_HDM_RV}' AD_HDM_EV='${AD_HDM_EV}'")
ENDIF(NOT "${AD_HDM_RV}" STREQUAL "0")

#################################################################################

# read target C compiler machine triplet

EXECUTE_PROCESS(
  COMMAND "${CMAKE_C_COMPILER}" "-dumpmachine"
  RESULT_VARIABLE AD_CDM_RV
  ERROR_VARIABLE AD_CDM_EV
  OUTPUT_VARIABLE AD_CDM_OV
  )
IF(NOT "${AD_CDM_RV}" STREQUAL "0")
  MESSAGE(FATAL_ERROR "ERROR: ${CMAKE_C_COMPILER} -dumpmachine failed... AD_CDM_RV='${AD_CDM_RV}' AD_CDM_EV='${AD_CDM_EV}'")
ENDIF(NOT "${AD_CDM_RV}" STREQUAL "0")

#################################################################################

# read target C++ compiler machine triplet

EXECUTE_PROCESS(
  COMMAND "${CMAKE_CXX_COMPILER}" "-dumpmachine"
  RESULT_VARIABLE AD_CXXDM_RV
  ERROR_VARIABLE AD_CXXDM_EV
  OUTPUT_VARIABLE AD_CXXDM_OV
  )
IF(NOT "${AD_CXXDM_RV}" STREQUAL "0")
	MESSAGE(FATAL_ERROR "ERROR: ${CMAKE_CXX_COMPILER} -dumpmachine failed... AD_CXXDM_RV='${AD_CXXDM_RV}' AD_CXXDM_EV='${AD_CXXDM_EV}'")
ENDIF(NOT "${AD_CXXDM_RV}" STREQUAL "0")

#################################################################################

# check if target compiler triplets match

IF(NOT "${AD_CDM_OV}" STREQUAL "${AD_CXXDM_OV}")
	MESSAGE(FATAL_ERROR "ERROR: Your C and C++ Compiler do not match: ${AD_CDM_OV} != $AD_CXXDM_OV}!")
ENDIF(NOT "${AD_CDM_OV}" STREQUAL "${AD_CXXDM_OV}")

#################################################################################

# do we crosscompile? if so, set a flag

IF (NOT "${AD_HDM_OV}" STREQUAL "${AD_CDM_OV}")
	SET(CROSSCOMPILE "1")
	MESSAGE(STATUS "Configuring for cross-compiling to ${AD_CDM_OV}")
ELSE (NOT "${AD_HDM_OV}" STREQUAL "${AD_CDM_OV}")
	SET(CROSSCOMPILE "0")
ENDIF (NOT "${AD_HDM_OV}" STREQUAL "${AD_CDM_OV}")

#################################################################################

# set specific flags for target platform

IF ( "${AD_CDM_OV}" MATCHES "linux" )
	# Linux spezifische Parameter
	SET(COMPILEFOR "linux")
	
	IF ( "${AD_CDM_OV}" MATCHES "x86_64" )
		SET(COMPILEARCH "amd64")
	ELSE ( "${AD_CDM_OV}" MATCHES "x86_64" )
		SET(COMPILEARCH "i686")
	ENDIF ( "${AD_CDM_OV}" MATCHES "x86_64" )
ENDIF ( "${AD_CDM_OV}" MATCHES "linux" )

IF ( "${AD_CDM_OV}" MATCHES "apple" )
	# Apple spezifische Parameter
	SET(COMPILEFOR "apple")

	IF ( "${AD_CDM_OV}" MATCHES "powerpc" )
		SET(COMPILEARCH "ppc")
	ELSE ( "${AD_CDM_OV}" MATCHES "powerpc" )
		SET(COMPILEARCH "i686")
	ENDIF ( "${AD_CDM_OV}" MATCHES "powerpc" )
ENDIF ( "${AD_CDM_OV}" MATCHES "apple" )

IF ( "${AD_CDM_OV}" MATCHES "mingw" )
	# Cygwin/Mingw spezifische Parameter
	SET(COMPILEFOR "windows")

	SET(COMPILEARCH "i686")
ENDIF ( "${AD_CDM_OV}" MATCHES "mingw" )

#################################################################################

SET(CROSSCOMPILE "${CROSSCOMPILE}" CACHE INT "Are we cross compiling?")
SET(COMPILEFOR "${COMPILEFOR}" CACHE STRING "Target Platform")
SET(COMPILEARCH "${COMPILEARCH}" CACHE STRING "Target Archictecure")

#################################################################################

IF(NOT COMPILEFOR_PLATFORM)
	IF ( "${COMPILEFOR}" STREQUAL "apple" )
		SET(COMPILEFOR_PLATFORM "local")
	ELSE ( "${COMPILEFOR}" STREQUAL "apple" )
		SET(COMPILEFOR_PLATFORM "apple.local")
	ENDIF ( "${COMPILEFOR}" STREQUAL "apple" )
ENDIF(NOT COMPILEFOR_PLATFORM)

MESSAGE(STATUS "Compiling for ${COMPILEFOR}/${COMPILEARCH}")

INCLUDE(cmake/${COMPILEFOR}.cmake OPTIONAL RESULT_VARIABLE FOUND_A)
IF(FOUND_A)
	MESSAGE(STATUS "Reading specifications from ${FOUND_A}")
ENDIF(FOUND_A)

INCLUDE(cmake/${COMPILEFOR}.${COMPILEARCH}.cmake OPTIONAL RESULT_VARIABLE FOUND_B)
IF(FOUND_B)
	MESSAGE(STATUS "Reading specifications from ${FOUND_B}")
ENDIF(FOUND_B)

IF (NOT FOUND_A AND NOT FOUND_B)
	MESSAGE(FATAL_ERROR " Architecture specific include file not found")
ENDIF (NOT FOUND_A AND NOT FOUND_B)

#################################################################################


