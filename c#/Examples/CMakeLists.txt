cmake_minimum_required(VERSION 3.3)

project(UltrahapticsExamples)

if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
	message(FATAL_ERROR "Run CMake in a separate build directory to avoid source code contamination.")
endif ()

# For FindLeap.cmake
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

find_package(Ultrahaptics REQUIRED)

find_package(Leap)
if (LEAP_FOUND)
	include_directories(${LEAP_INCLUDE_DIR})
else ()
	message(WARNING "Could not find Leap SDK")
endif ()

set(ULTRAHAPTICS_CSHARP_DIR "" CACHE PATH "Ultrahaptics CSharp libraries folder")
if (NOT ULTRAHAPTICS_CSHARP_DIR STREQUAL "" AND EXISTS "${ULTRAHAPTICS_CSHARP_DIR}")
	find_file(ULTRAHAPTICS_CSHARP_RUNTIME 
		NAMES libUltrahapticsCSharp.dylib libUltrahapticsCSharp.so UltrahapticsCSharp.dll 
		PATHS "${ULTRAHAPTICS_CSHARP_DIR}")

	find_file(ULTRAHAPTICS_CSHARP_40 UltrahapticsCSharp.NET40.dll "${ULTRAHAPTICS_CSHARP_DIR}")
	if (ULTRAHAPTICS_CSHARP_RUNTIME AND ULTRAHAPTICS_CSHARP_40)
		set(ULTRAHAPTICS_CSHARP_FOUND TRUE)
	endif()
endif()

add_subdirectory(cpp)
if (ULTRAHAPTICS_CSHARP_FOUND)
	add_subdirectory(csharp)
endif()

message(STATUS "	========== Examples Configuration Summary =========\n")

if (LEAP_FOUND)
	message(STATUS "	Leap:            Enabled")
else ()
	message(STATUS "	Leap:            Not Found")
endif ()

message(STATUS "")
