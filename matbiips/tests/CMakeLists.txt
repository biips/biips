file(GLOB mfiles *.m)
file(GLOB bugs_files *.bug)

set(test_files ${mfiles} ${bugs_files})
file(COPY ${test_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(mtests output console test_macros test_smc funcmat_test test_internals)
foreach(_test ${mtests})
    set (MAT_COMS "${_test};exit")
    if (MATLAB)
        add_test(NAME ${_test}-matlab
            COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS} -r "${MAT_COMS}"
        )
        set_tests_properties(${_test}-matlab PROPERTIES TIMEOUT 30)
    elseif(OCTAVE)
        add_test(NAME ${_test}-octave
            COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS} --eval "${MAT_COMS}"
        )
        set_tests_properties(${_test}-octave PROPERTIES TIMEOUT 30)
    endif()
endforeach()
