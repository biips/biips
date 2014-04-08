set(example_scripts tutorial/tutorial1
    tutorial/tutorial2
    tutorial/tutorial3
)

find_program(SED sed)

if(SED)

add_custom_target(mat2r_examples
    COMMENT "Converting matlab examples to R"
)

foreach(f ${example_scripts})

    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${f}_spin.R
        COMMAND ${SED} -E -f m2R.sed ${f}.m > ${f}_spin.R
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Converting example ${f}.m to ${f}_spin.R"
    )
    
    add_dependencies(mat2r_examples
        ${CMAKE_CURRENT_SOURCE_DIR}/${f}_spin.R
    )
    
endforeach()


endif(SED)
