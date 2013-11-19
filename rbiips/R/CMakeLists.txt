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
        message(STATUS "${CMAKE_CURRENT_BINARY_DIR}/${fic}")
        execute_process(COMMAND ${sed_exe} -e 
        "s/PACKAGE\ *=\ *\"RBiips\"/\"rbiips_shlib.so\"/g" ${f}
        OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${fic}")
        file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/${fic}" "${CMAKE_BINARY_DIR}/rbiips/tests/${fic}")
    endforeach()
endif(sed_exe)

# add dependencies to source files 
add_targets_depends(rfiles)
