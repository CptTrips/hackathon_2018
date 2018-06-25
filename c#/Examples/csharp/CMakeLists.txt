include(BuildCSharp)

if (NOT CSHARP_FOUND)
	return()
endif()

set(cs_examples
	"CSharp_AmplitudeModulation_Focus"
	"CSharp_AmplitudeModulation_MultiDeviceFocus"
	"CSharp_TimepointStreaming_Focus"
)

set(leap_csharp_examples
	"CSharp_Button_Click"
)

add_custom_target(copy_csharp_ultrahaptics_libs
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ULTRAHAPTICS_CSHARP_40}" "${CMAKE_CURRENT_BINARY_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ULTRAHAPTICS_CSHARP_RUNTIME}"  "${CMAKE_CURRENT_BINARY_DIR}"
)
if (WIN32)
	# Windows requires the runtime library to be found in the current directory
	add_custom_command(TARGET copy_csharp_ultrahaptics_libs POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ULTRAHAPTICS_RUNTIME_LIBRARY}" "${CMAKE_CURRENT_BINARY_DIR}"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ULTRAHAPTICS_LIBUSB_DLL}" "${CMAKE_CURRENT_BINARY_DIR}"
	)
endif()

foreach (example IN LISTS cs_examples)
	add_csharp_executable(${example} "${ULTRAHAPTICS_CSHARP_40}" ${example}.cs)
	add_dependencies(${example} copy_csharp_ultrahaptics_libs)
endforeach ()

if (LEAP_FOUND)
	add_custom_target(copy_csharp_leap_libs
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${LEAP_CSHARP_DLL}" "${CMAKE_CURRENT_BINARY_DIR}"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${LEAP_CSHARP_LIBRARY}" "${CMAKE_CURRENT_BINARY_DIR}")
	foreach (example IN LISTS leap_csharp_examples)
		add_csharp_executable(${example} "${ULTRAHAPTICS_CSHARP_40};${LEAP_CSHARP_DLL}" ${example}.cs)

		add_dependencies(${example} copy_csharp_ultrahaptics_libs copy_csharp_leap_libs)
	endforeach()
endif()
