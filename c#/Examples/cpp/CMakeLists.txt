# Find the leap package using `FindLeap.cmake`.
set(CMAKE_CXX_STANDARD 14)

# These examples do not require a Leap Motion.
set(basic_examples
	"AmplitudeModulation_Focus"
	"AmplitudeModulation_Square"
	"TimePointStreaming_Square"
	"AmplitudeModulation_MultiDevice"
	"TimePointStreaming_CachedPoint"
	"TimePointStreaming_Focus"
	"TimePointStreaming_Freeform"
	"TimePointStreaming_Circle"
	"TimePointStreaming_MultiDeviceFocus"
)

# These examples require the Leap SDK.
set(leap_examples
	"AmplitudeModulation_HandTracking"
	"AmplitudeModulation_MultiDeviceHandTracking"
	"AmplitudeModulation_MultiDeviceHandTrackingDirectional"
	"AmplitudeModulation_Dial"
	"TimePointStreaming_FreeformTracking"
	"TimePointStreaming_Forcefield"
	"TimePointStreaming_MultiPoint"
	"TimePointStreaming_Dynamic"
	"TimePointStreaming_MultiDeviceForcefield"
)

if (WIN32)
	# Windows requires the runtime library to be found in the current directory
	add_custom_target(copy_ultrahaptics_libs
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ULTRAHAPTICS_RUNTIME_LIBRARY}" "${CMAKE_CURRENT_BINARY_DIR}"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ULTRAHAPTICS_LIBUSB_DLL}" "${CMAKE_CURRENT_BINARY_DIR}"
	)
endif()

# Build the basic examples.
foreach (example IN LISTS basic_examples)
	add_executable("${example}" "${example}.cpp")
	target_link_libraries("${example}" Ultrahaptics::ultrahaptics)
	if (WIN32)
		add_dependencies(${example} copy_ultrahaptics_libs)
	endif()
endforeach ()

# Build the Leap Motion examples, if the Leap SDK was found.
if (LEAP_FOUND)
	# Copy the Leap Motion library into the examples folder for convenience.
	add_custom_target(copy_leap_libs
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different $<TARGET_FILE:Leap::Leap> "${CMAKE_CURRENT_BINARY_DIR}"
	)
	foreach (example IN LISTS leap_examples)
		add_executable("${example}" "${example}.cpp")
		target_link_libraries("${example}" Ultrahaptics::ultrahaptics Leap::Leap)
		add_dependencies(${example} copy_leap_libs)
		if (WIN32)
			add_dependencies(${example} copy_ultrahaptics_libs)
		endif()
		# When using libstdc++ we need to link with libatomic to enable the use
		# of large std::atomic's. libstdc++ is only used with GCC, or by default
		# with Clang on Linux.
		if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR (UNIX AND NOT APPLE))
			target_link_libraries("${example}" atomic)
		endif ()
	endforeach()
endif()
