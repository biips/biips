file(GLOB mfiles *.m)
set(test_files ${mfiles} hmm_1d_lin.bug hmm_4d_nonlin_tracking.bug)
copy_files_to_bindir(test_files ${CMAKE_CURRENT_BINARY_DIR})

#===========Ctest command
foreach(_mf ${mfiles})
    get_filename_component(_dir ${_mf} PATH)
    get_filename_component(_name ${_mf} NAME_WE)
    if (MATLAB)
        add_test(NAME ${_name}-test
            COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS} -r "${_name};exit"
        )
    else()
        add_test(NAME ${_name}-test
            COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS} ${_name}
        )
    endif()
endforeach()
