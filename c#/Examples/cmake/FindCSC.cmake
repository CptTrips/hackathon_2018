# This finds the Microsoft C# compiler.

function (find_csc name_suffix version)
	get_filename_component(dotnet_path "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\.NETFramework;InstallRoot]" PATH)
	find_program("CSC${name_suffix}_EXECUTABLE" NAMES "csc" "csc.exe" PATHS "${dotnet_path}/Framework/v${version}" NO_DEFAULT_PATH)
	find_program("CSC${name_suffix}_EXECUTABLE" NAMES "csc")

	if (CSC${name_suffix}_EXECUTABLE)
		file(TO_NATIVE_PATH "${CSC${name_suffix}_EXECUTABLE}" "CSC${name_suffix}_EXECUTABLE")
		message(STATUS "Found CSC: ${CSC${name_suffix}_EXECUTABLE} (found version: \"${version}\")")
	else ()
		message(STATUS "Couldn't find CSC version ${version}. Set CSC${name_suffix}_EXECUTABLE to a valid path.")
	endif ()
endfunction ()

# Note that single-digit names (e.g. "CSC2_EXECUTABLE" should not be used)
# CMake seems to have internal variables using these names, which are cached and override these
find_csc("35" "3.5")
find_csc("40" "4.0.30319")
