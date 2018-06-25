# Find Leap
#
# Finds the libraries and header files for the Leap SDK for Leap Motion's
# hand tracker.
#
# This module defines
#   LEAP_FOUND       - Leap was found
#   LEAP_INCLUDE_DIR - Directory containing LEAP header files
#   LEAP_LIBRARY     - Library name of Leap library
#
# The following `IMPORTED` target is also defined:
#   Leap::Leap       - Target for Leap library

# Don't be verbose if previously run successfully
if(LEAP_INCLUDE_DIR AND LEAP_LIBRARY)
   set(LEAP_FIND_QUIETLY TRUE)
endif(LEAP_INCLUDE_DIR AND LEAP_LIBRARY)

set(ProgramFilesX86 "ProgramFiles(x86)")
# Search for header files
find_path(LEAP_INCLUDE_DIR
    NAMES Leap.h LeapMath.h
    HINTS ${LEAP_ROOT}/include
    NO_DEFAULT_PATH
)
find_path(LEAP_INCLUDE_DIR
    NAMES Leap.h LeapMath.h
    PATHS
        "/usr/include"
        "/usr/local/include"
        "/opt/leap/include"
        "/opt/LeapSDK/include"
        "/opt/include"
        "~/usr/include"
        "~/.local/include"
        "~/LeapSDK/include"
        "$ENV{USERPROFILE}/LeapSDK/include"
        "$ENV{PROGRAMFILES}/LeapSDK/include"
        "$ENV{${ProgramFilesX86}}/LeapSDK/include"
        "${LEAP_ROOT}/include"
        "$ENV{LEAP_ROOT}/include"
)

# Search for library
if (NOT APPLE)
  if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
      set(lib_dir "x64")
  else ()
      set(lib_dir "x86")
  endif ()
endif ()

find_library(LEAP_LIBRARY
    NAMES Leap
    HINTS ${LEAP_ROOT}/lib/${lib_dir}
          ${LEAP_ROOT}/lib
    NO_DEFAULT_PATH
)

find_file(LEAP_CSHARP_LIBRARY
    NAMES libLeapCSharp.dylib libLeapCSharp.so LeapCSharp.dll 
    HINTS ${LEAP_ROOT}/lib/${lib_dir}
          ${LEAP_ROOT}/lib
    NO_DEFAULT_PATH
)

set(LEAP_LIB_PATHS
        "/usr/lib"
        "/usr/lib64"
        "/usr/local/lib"
        "/usr/local/lib64"
        "/opt/leap/lib"
        "/opt/leap/lib64"
        "/opt/LeapSDK/lib"
        "/opt/LeapSDK/lib64"
        "~/usr/lib"
        "~/usr/lib64"
        "~/.local/lib"
        "~/LeapSDK/lib/${lib_dir}"
        "~/LeapSDK/lib"
        "$ENV{USERPROFILE}/LeapSDK/lib/${lib_dir}"
        "$ENV{ProgramFiles}/LeapSDK/lib/${lib_dir}"
        "$ENV{${ProgramFilesX86}}/LeapSDK/lib/${lib_dir}"
        "$ENV{USERPROFILE}/LeapSDK/lib"
        "$ENV{ProgramFiles}/LeapSDK/lib"
        "$ENV{${ProgramFilesX86}}/LeapSDK/lib"
  )

find_library(LEAP_LIBRARY
    NAMES Leap
    PATHS ${LEAP_LIB_PATHS}
)

find_file(LEAP_CSHARP_LIBRARY
    NAMES libLeapCSharp.dylib libLeapCSharp.so LeapCSharp.dll
    PATHS ${LEAP_LIB_PATHS}
    PATH_SUFFIXES Leap
)

# Search for dll if windows
if(WIN32)
    get_filename_component(library_path "${LEAP_LIBRARY}" DIRECTORY)

    find_file(LEAP_DLL "Leap.dll"
        HINTS "${library_path}"
              "${LEAP_ROOT}/lib/${lib_dir}"
              "${LEAP_ROOT}/lib"
        NO_DEFAULT_PATH)

    find_file(LEAP_CSHARP_LIBRARY "LeapCSharp.dll"
	HINTS "${library_path}"
              "${LEAP_ROOT}/lib/${lib_dir}"
              "${LEAP_ROOT}/lib")
else ()
    set(LEAP_DLL "${LEAP_LIBRARY}")

    find_library(LEAP_CSHARP_LIBRARY
        NAMES LeapCSharp
	HINTS ${LEAP_ROOT}/lib/${lib_dir}
	      ${LEAP_ROOT}/lib)
endif()

get_filename_component(csharp_library_path "${LEAP_CSHARP_LIBRARY}" DIRECTORY)

find_file(LEAP_CSHARP_DLL "LeapCSharp.NET4.0.dll"
    HINTS "${csharp_library_path}"
          "${LEAP_ROOT}/lib/${lib_dir}"
          "${LEAP_ROOT}/lib"
    PATHS ${LEAP_LIB_PATHS}
    PATH_SUFFIXES Leap
)

set(LEAP_INCLUDE_DIR ${LEAP_INCLUDE_DIR} CACHE STRING "Directory containing LEAP header files")
set(LEAP_LIBRARY     ${LEAP_LIBRARY}     CACHE STRING "Library name of Leap library")
set(LEAP_DLL         ${LEAP_DLL}         CACHE STRING "Path to Leap runtime library")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Leap DEFAULT_MSG LEAP_LIBRARY LEAP_INCLUDE_DIR)

mark_as_advanced(LEAP_INCLUDE_DIR LEAP_LIBRARY LEAP_DLL)

if(NOT TARGET Leap::Leap)
  add_library(Leap::Leap SHARED IMPORTED)

  if(EXISTS "${LEAP_INCLUDE_DIR}")
    set_target_properties(Leap::Leap PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${LEAP_INCLUDE_DIR}")
  endif()

  if(EXISTS "${LEAP_DLL}")
    set_target_properties(Leap::Leap PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
      IMPORTED_LOCATION "${LEAP_DLL}"
      IMPORTED_IMPLIB "${LEAP_LIBRARY}")
  elseif(EXISTS "${LEAP_LIBRARY}")
    set_target_properties(Leap::Leap PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
      IMPORTED_LOCATION "${LEAP_LIBRARY}")
  endif()
endif()
