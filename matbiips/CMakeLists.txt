add_subdirectory(src)
add_subdirectory(tests)
#===========Ctest command
if (BUILD_MATBIIPS)
    file(GLOB m_files "${CMAKE_BINARY_DIR}/matbiips/tests/*.m")
    foreach(mf ${m_files})
    	get_filename_component(nom ${mf} NAME)
    	get_filename_component(direc ${mf} PATH)
	if (MATLAB)
	add_test(NAME ${nom}-test WORKING_DIRECTORY 
    		${direc} COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS} "<"
      		 ${nom})
        else(MATLAB)
	add_test(NAME ${nom}-test WORKING_DIRECTORY 
    		${direc} COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS} 
      		 ${nom})
        endif(MATLAB)
    endforeach(mf in GLOB)	
endif (BUILD_MATBIIPS)
