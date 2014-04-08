set(example_scripts tutorial/tutorial1
    tutorial/tutorial2
    tutorial/tutorial3
    object_tracking/hmm_4d_nonlin
    stoch_volatility/stoch_volatility
    stoch_volatility/switch_stoch_volatility
    stoch_kinetic/stoch_kinetic
    stoch_kinetic/stoch_kinetic_gill
    tvdp/tvdp
)

find_program(SED sed)

if(SED)

add_custom_target(mat2r_examples
)

foreach(f ${example_scripts})

    add_custom_command(
        TARGET mat2r_examples
        #OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${f}_spin.R
        COMMAND ${SED} -E -f m2R.sed ${f}.m > ${f}_spin.R
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Converting example ${f}.m to ${f}_spin.R"
    )
    
endforeach()


endif(SED)
