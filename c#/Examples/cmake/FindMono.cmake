# This tries to find the Mono C# compiler, used on OSX and Linux.

# Unix includes OSX.
if (UNIX)
	find_program(MONO_EXECUTABLE "mono")
	find_program(MCS_EXECUTABLE "mcs")
	find_program(GACUTIL_EXECUTABLE "gacutil")
else ()
	# Win32
	set(MONO_SEARCH_DIRS
		"C:\\Program Files\\Mono"
		"C:\\Program Files (x86)\\Mono"
		"C:\\Mono")
	
	find_path(MONO_DIRECTORY
		NAMES "mono.bat" "mcs.bat"
		PATH_SUFFIXES "bin"
		PATHS ${MONO_SEARCH_DIRS})
	
	find_program(MONO_EXECUTABLE "mono" PATHS ${MONO_DIRECTORY})
	find_program(MCS_EXECUTABLE "mcs" PATHS ${MONO_DIRECTORY})
	find_program(GACUTIL_EXECUTABLE "gacutil" PATHS ${MONO_DIRECTORY})
	
endif ()

set(MONO_FOUND FALSE CACHE INTERNAL "")

if(MONO_EXECUTABLE AND MCS_EXECUTABLE AND GACUTIL_EXECUTABLE)
	set(MONO_FOUND TRUE CACHE INTERNAL "")

	# Default GAC is located in <prefix>/lib/mono/gac
	if (UNIX)
		find_path(MONO_GAC_PREFIX lib/mono/gac
			PATHS "/usr;/usr/local")
	else ()
		find_path(MONO_GAC_PREFIX lib/mono/gac
			PATHS ${MONO_DIRECTORY})
	endif ()

	set(MONO_GAC_PREFIX "${MONO_GAC_PREFIX}" CACHE PATH "Mono GAC prefix")
	set(MONO_GAC_DIR "${MONO_GAC_PREFIX}/lib/mono" CACHE PATH "Mono GAC directory")

	execute_process(COMMAND ${MCS_EXECUTABLE} --version OUTPUT_VARIABLE MONO_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(REGEX REPLACE ".*version ([^ ]+)" "\\1" MONO_VERSION "${MONO_VERSION}")
endif ()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
	Mono
	REQUIRED_VARS MONO_EXECUTABLE MCS_EXECUTABLE GACUTIL_EXECUTABLE MONO_GAC_PREFIX MONO_GAC_DIR MONO_VERSION
	VERSION_VAR MONO_VERSION)

mark_as_advanced(
	MONO_EXECUTABLE MCS_EXECUTABLE GACUTIL_EXECUTABLE MONO_GAC_PREFIX MONO_GAC_DIR MONO_VERSION)
