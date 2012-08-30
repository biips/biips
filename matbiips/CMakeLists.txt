add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(matlab)
#===========Ctest command
if (BUILD_MATBIIPS)
    file(GLOB m_files "${CMAKE_BINARY_DIR}/matbiips/tests/*.m")
    foreach(mf ${m_files})
    	get_filename_component(dir ${mf} PATH)
		if (MATLAB)
			get_filename_component(name ${mf} NAME_WE)
			add_test(NAME ${name}-test
				WORKING_DIRECTORY ${dir}
				COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS} -r "${name}")
		else()
			get_filename_component(name ${mf} NAME)
			add_test(NAME ${name}-test
				WORKING_DIRECTORY ${dir}
				COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS} ${name})
		endif()
    endforeach(mf in GLOB)	
endif (BUILD_MATBIIPS)
