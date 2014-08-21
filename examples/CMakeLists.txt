#  Translate Matlab examples to R using sed script
# ---------------------------

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

  foreach(_f ${example_scripts})
      add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${_f}_m2R.R
          DEPENDS m2R.sed ${_f}.m
          COMMAND ${SED} -E -f m2R.sed ${_f}.m > ${_f}_m2R.R
          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )
      
      set(example_scripts_r ${example_scripts_r} ${_f}_m2R.R)
  endforeach()

  add_custom_target(mat2r_examples
    DEPENDS m2R.sed ${example_scripts_r}
    COMMENT "Translating Matlab examples to R"
  )

endif(SED)


#  make examples archives
# ---------------------------

# lists of source files
file(GLOB ex_files_common RELATIVE "${CMAKE_SOURCE_DIR}"
    *.md
)
foreach(_ex_dir tutorial object_tracking stoch_volatility stoch_kinetic)

    # matbiips source files
    file(GLOB _ex_files_mat RELATIVE "${CMAKE_SOURCE_DIR}"
        ${_ex_dir}/*.m
    )
    set(ex_files_mat ${ex_files_mat} ${_ex_files_mat})
    file(GLOB _ex_dep_mat
        ${_ex_dir}/*.m
    )
    set(ex_dep_mat ${ex_dep_mat} ${_ex_dep_mat})
    
    # rbiips source files
    file(GLOB _ex_files_r RELATIVE "${CMAKE_SOURCE_DIR}"
        ${_ex_dir}/*.R
    )
    set(ex_files_r ${ex_files_r} ${_ex_files_r})
    file(GLOB _ex_dep_r
        ${_ex_dir}/*.R
    )
    set(ex_dep_r ${ex_dep_r} ${_ex_dep_r})
    
    # common source files
    file(GLOB _ex_files_common RELATIVE "${CMAKE_SOURCE_DIR}"
        ${_ex_dir}/*.bug
        ${_ex_dir}/*.md
        ${_ex_dir}/*.csv
    )
    set(ex_files_common ${ex_files_common} ${_ex_files_common})
    file(GLOB _ex_dep_common
        ${_ex_dir}/*.bug
        ${_ex_dir}/*.md
        ${_ex_dir}/*.csv
    )
    set(ex_dep_common ${ex_dep_common} ${_ex_dep_common})
endforeach()


foreach (_ext tar.gz zip)
    if (_ext STREQUAL tar.gz)
        # using cmake -E
        set (ARCHIVE_COMMAND ${CMAKE_COMMAND} -E tar cvfz)
    else(_ext STREQUAL zip AND ZIP)
        # using zip
        set (ARCHIVE_COMMAND ${ZIP_COMMAND})
    endif()

   # matbiips examples archive
    file(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}/matbiips-examples_${Biips_VERSION}.${_ext}" ex_archive_mat)
    set(EXAMPLES_ARCHIVE_LIST ${EXAMPLES_ARCHIVE_LIST} ${ex_archive_mat})
    add_custom_command( OUTPUT ${ex_archive_mat}
      DEPENDS ${ex_dep_common} ${ex_dep_mat}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND ${ARCHIVE_COMMAND} ${ex_archive_mat}
      ${ex_files_common} ${ex_files_mat}
    )
    
   # rbiips examples archive
    file(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}/rbiips-examples_${Biips_VERSION}.${_ext}" ex_archive_r)
    set(EXAMPLES_ARCHIVE_LIST ${EXAMPLES_ARCHIVE_LIST} ${ex_archive_r})
    add_custom_command( OUTPUT ${ex_archive_r}
      DEPENDS ${ex_dep_common} ${ex_dep_r}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND ${ARCHIVE_COMMAND} ${ex_archive_r}
      ${ex_files_common} ${ex_files_r}
    )
    
   # all examples archive
    file(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}/biips-examples_${Biips_VERSION}.${_ext}" ex_archive_all)
    set(EXAMPLES_ARCHIVE_LIST ${EXAMPLES_ARCHIVE_LIST} ${ex_archive_all})
    add_custom_command( OUTPUT ${ex_archive_all}
      DEPENDS  ${ex_dep_common} ${ex_dep_mat} ${ex_dep_r}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND ${ARCHIVE_COMMAND} ${ex_archive_all}
      ${ex_files_common} ${ex_files_mat} ${ex_files_r}
    )
endforeach()

    
#define target
add_custom_target(examples_package
    DEPENDS  ${EXAMPLES_ARCHIVE_LIST}
    COMMENT "Packaging Biips examples"
)

