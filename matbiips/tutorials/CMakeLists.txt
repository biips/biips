file(GLOB mfiles *.m)
set(mbiips_bug hmm_4d_lin_tracking 
                hmm_4d_nonlin_tracking)
foreach(bug ${mbiips_bug})
     set (bfiles ${bugs_files} "${CMAKE_SOURCE_DIR}/examples/${bug}.bug")
endforeach()
set(GLOB bfiles 
set(test_files ${mfiles} ${bfiles})
file(COPY ${test_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

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
