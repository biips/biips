#if (EXISTS ${INTER_BIN})
file(GLOB mfiles *.m)
file(GLOB bfiles *.bug)
set(test_files ${mfiles} ${bfiles})
file(COPY ${test_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#===========Ctest command
set(mtests hmm_4d_lin hmm_4d_nonlin)

foreach(_test ${mtests})
    set (MAT_COMS "${_test};exit")
    if (MATLAB)
        add_test(NAME ${_test}-test
            COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS} -r "${MAT_COMS}"
        )
    else()
        add_test(NAME ${_test}-test
            COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS} --eval "${MAT_COMS}"
        )
    endif()
    set_tests_properties(${_test}-test PROPERTIES TIMEOUT 30)
endforeach()
#endif(EXISTS ${INTER_BIN})
