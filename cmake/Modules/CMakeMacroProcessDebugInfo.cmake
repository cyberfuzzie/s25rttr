# Process the Debug Info in ${target} according to Build type
# and install the debug info if it was moved to a separate file

MACRO (PROCESS_DEBUG_INFO target destination)
	# Separate Debug Info in ${target}.dbg file for RelWithDebInfo-Build
	IF ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
		ADD_CUSTOM_COMMAND (TARGET ${target}
			POST_BUILD COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --bold --blue "Separating Debug Info from ${target}"
				   COMMAND ${CMAKE_OBJCOPY} --only-keep-debug "$<TARGET_FILE:${target}>" "$<TARGET_FILE:${target}>.dbg"
				   COMMAND ${CMAKE_OBJCOPY} --strip-debug "$<TARGET_FILE:${target}>"
				   COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink="$<TARGET_FILE:${target}>.dbg" "$<TARGET_FILE:${target}>")
	ENDIF ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")

	# Remove unneeded Debug Info for Release-Build
	IF ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		ADD_CUSTOM_COMMAND (TARGET ${target}
			POST_BUILD COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --bold --blue "Stripping Debug Info from ${target}"
				   COMMAND ${CMAKE_STRIP} --strip-unneeded "$<TARGET_FILE:${target}>")
	ENDIF ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")

	# Remove all Debug Info from executables
	# and unneeded from everything else for MinSizeRel-Build
	IF ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
		GET_TARGET_PROPERTY (type ${target} TYPE)
		IF (${type} STREQUAL "EXECUTABLE")
			ADD_CUSTOM_COMMAND (TARGET ${target}
				POST_BUILD COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --bold --blue "Stripping Debug Info from ${target}"
					   COMMAND ${CMAKE_STRIP} --strip-all "$<TARGET_FILE:${target}>")
		ELSE (${type} STREQUAL "EXECUTABLE")
			ADD_CUSTOM_COMMAND (TARGET ${target}
				POST_BUILD COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --bold --blue "Stripping Debug Info from ${target}"
					   COMMAND ${CMAKE_STRIP} --strip-unneeded "$<TARGET_FILE:${target}>")
		ENDIF (${type} STREQUAL "EXECUTABLE")
	ENDIF ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")

	# Install separated Debug Info for RelWithDebInfo-Build
	GET_TARGET_PROPERTY (fullname ${target} LOCATION)
	GET_FILENAME_COMPONENT (targetFile "${fullname}" NAME)
	INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/${targetFile}.dbg
		CONFIGURATIONS RelWithDebInfo
		DESTINATION ${destination}/dbg)
ENDMACRO (PROCESS_DEBUG_INFO)
