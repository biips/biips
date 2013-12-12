file(GLOB mfiles *.m)
set(mbiips_bug funcmat 
                hmm_4d_lin_tracking 
                hmm_4d_nonlin_tracking
                hmm_1d_lin) 
foreach(bug ${mbiips_bug})
     set (bugs_files ${bugs_files} "${CMAKE_SOURCE_DIR}/examples/${bug}.bug")
endforeach()
set(test_files ${mfiles} ${bugs_files})
file(COPY ${test_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(mtests cono console test_macros test_smc funcmat_test hmm_4d_lin hmm_4d_nonlin)
foreach(_test ${mtests})
    set (MAT_COMS "${_test};exit")
    if (MATLAB)
        add_test(NAME ${_test}-matlab
            COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS} -r "${MAT_COMS}"
        )
        else(MATLAB) # Octave
        add_test(NAME ${_test}-matlab
            COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS} --eval "${MAT_COMS}"
        )
    endif()
    set_tests_properties(${_test}-matlab PROPERTIES TIMEOUT 30)
endforeach()
