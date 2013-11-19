add_subdirectory(unix)
add_subdirectory(windows)

set_output_bindir(out_bindir)

# copy files to binary directory
file(GLOB rfiles *.R)
file(COPY ${rfiles} DESTINATION ${out_bindir})
# we seek for sed to convert files into the "test" paradigm
# i.e, all "PACKAGE = RBIIPS" are changed to rbiips_shlib.so
find_program(sed_exe NAMES sed HINTS /bin /usr/bin)
if (sed_exe)
    # convert the files
    foreach(f ${rfiles})
        get_filename_component(fic ${f} NAME)
        add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${fic}"
        COMMAND ${sed_exe} 
        ARGS -e "s/PACKAGE\ *=\ *\"RBiips\"/\"rbiips_shlib.so\"/g" ${f} > "${CMAKE_CURRENT_BINARY_DIR}/${fic}"
        COMMENT "converting ${f}" VERBATIM)
        add_custom_command(OUTPUT  "${CMAKE_BINARY_DIR}/rbiips/tests/${fic}"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${fic}"
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy "${CMAKE_CURRENT_BINARY_DIR}/${fic}" "${CMAKE_BINARY_DIR}/rbiips/tests/${fic}")
        set(target_modified  "${CMAKE_BINARY_DIR}/rbiips/tests/${fic}"
        ${target_modified})
    endforeach()
add_custom_target(files_modified ALL DEPENDS ${target_modified})
endif(sed_exe)
# add dependencies to source files 
add_targets_depends(rfiles)
