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


file(GLOB ex_files_common RELATIVE "${CMAKE_SOURCE_DIR}"
    *.md
)
foreach(_ex_dir tutorial object_tracking stoch_volatility stoch_kinetic)
    file(GLOB _ex_files_mat RELATIVE "${CMAKE_SOURCE_DIR}"
        ${_ex_dir}/*.m
    )
    set(ex_files_mat ${ex_files_mat} ${_ex_files_mat})
    
    file(GLOB _ex_files_r RELATIVE "${CMAKE_SOURCE_DIR}"
        ${_ex_dir}/*.R
    )
    set(ex_files_r ${ex_files_r} ${_ex_files_r})
    
    file(GLOB _ex_files_common RELATIVE "${CMAKE_SOURCE_DIR}"
        ${_ex_dir}/*.bug
        ${_ex_dir}/*.md
        ${_ex_dir}/*.csv
    )
    set(ex_files_common ${ex_files_common} ${_ex_files_common})
endforeach()
    
if (WIN32)
  set(_ext_list zip)
else()
  set(_ext_list zip tar.gz)
endif()

foreach (_ext ${_ext_list})
    file(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}/matbiips-examples_${Biips_VERSION}.${_ext}" ex_archive_mat)
    set(EXAMPLES_ARCHIVE_LIST ${EXAMPLES_ARCHIVE_LIST} ${ex_archive_mat})
    add_custom_command( OUTPUT ${ex_archive_mat}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND ${CMAKE_COMMAND} -E remove ${ex_archive_mat}
      COMMAND ${CMAKE_COMMAND} -E tar cafv ${ex_archive_mat}
      ${ex_files_common} ${ex_files_mat}
    )
    
    file(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}/rbiips-examples_${Biips_VERSION}.${_ext}" ex_archive_r)
    set(EXAMPLES_ARCHIVE_LIST ${EXAMPLES_ARCHIVE_LIST} ${ex_archive_r})
    add_custom_command( OUTPUT ${ex_archive_r}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND ${CMAKE_COMMAND} -E remove ${ex_archive_r}
      COMMAND ${CMAKE_COMMAND} -E tar cafv ${ex_archive_r}
      ${ex_files_common} ${ex_files_r}
    )
    
    file(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}/biips-examples_${Biips_VERSION}.${_ext}" ex_archive_all)
    set(EXAMPLES_ARCHIVE_LIST ${EXAMPLES_ARCHIVE_LIST} ${ex_archive_all})
    add_custom_command( OUTPUT ${ex_archive_all}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND ${CMAKE_COMMAND} -E remove ${ex_archive_all}
      COMMAND ${CMAKE_COMMAND} -E tar cafv ${ex_archive_all}
      ${ex_files_common} ${ex_files_mat} ${ex_files_r}
    )
endforeach()
    
#define target
add_custom_target(examples_package
    DEPENDS  ${EXAMPLES_ARCHIVE_LIST}
    COMMENT "Packaging Biips examples"
)
