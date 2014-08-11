set(example_scripts
    tutorial/tutorial1
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


if (WIN32)
  set(_ext zip)
else()
  set(_ext tar.gz)
endif()

file(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}/biips-examples_${Biips_VERSION}.${_ext}" EXAMPLES_ARCHIVE)

file(GLOB ex_files RELATIVE "${CMAKE_SOURCE_DIR}"
    *.md
)
foreach(ex_dir tutorial object_tracking stoch_volatility stoch_kinetic)
    file(GLOB ex_dir_files RELATIVE "${CMAKE_SOURCE_DIR}"
        ${ex_dir}/*.m
        ${ex_dir}/*.bug
        ${ex_dir}/*.md
        ${ex_dir}/*.csv
    )
    set(ex_files ${ex_files} ${ex_dir_files})
endforeach()

add_custom_command( OUTPUT ${EXAMPLES_ARCHIVE}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} -E remove ${EXAMPLES_ARCHIVE}
  COMMAND ${CMAKE_COMMAND} -E tar cafv ${EXAMPLES_ARCHIVE}
  ${ex_files}
  COMMENT "Packaging Biips examples"
)
    
#define target
add_custom_target(examples_package
    DEPENDS  ${EXAMPLES_ARCHIVE}
)


