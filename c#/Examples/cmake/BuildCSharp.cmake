set(CSHARP_FOUND OFF)

if(WIN32)
	find_package(CSC QUIET)
	if (NOT CSC40_EXECUTABLE)
		return()
	endif()

	set(CSHARP_FOUND ON)
	set(CSHARP_COMPILER "${CSC40_EXECUTABLE}")
	if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		set(cs_platform "x64")
	else ()
		set(cs_platform "x86")
	endif ()
	set(CSHARP_COMPILER_ARGUMENTS -unsafe -platform:${cs_platform})
else()
	find_package(Mono QUIET)
	if(NOT MCS_EXECUTABLE)
		return()
	endif()

	set(CSHARP_FOUND ON)
	set(CSHARP_COMPILER "${MCS_EXECUTABLE}")
	set(CSHARP_COMPILER_ARGUMENTS -t:exe -sdk:4)
endif()

function(add_csharp_executable target references)
	set(_sources "${ARGN}")
	set(_executable ${target}.exe)

	add_custom_target(${target} ALL
		DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${_executable}"
		SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${_sources}"
	)

	# macro arguments are not variables and cannot be used as such -> reasign to new var
	set(_references ${references})
	set(refs "")
	foreach(r IN LISTS _references)
		list(APPEND refs "-r:${r}")
		get_filename_component(base_lib "${r}" NAME_WE)
		if(NOT TARGET copy_${base_lib})
			add_custom_target(copy_${base_lib}
				COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${r}" "${CMAKE_CURRENT_BINARY_DIR}"
			)
		endif()
		add_dependencies(${target} copy_${base_lib})
	endforeach()

	file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${_sources}" _nsources)
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_executable}
		COMMENT "Building ${_sources} with ${CSHARP_COMPILER}"
		COMMAND "${CSHARP_COMPILER}" ${CSHARP_COMPILER_ARGUMENTS} ${refs} -out:${_executable} "${_nsources}"
		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${_sources}"
		COMMAND_EXPAND_LISTS
	)
endfunction()
