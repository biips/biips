file(GLOB mfiles *.m)
file(GLOB bugfiles *.bug)
set(test_files ${mfiles} ${bugfiles})
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
