add_subdirectory(matlab)
set(INTER_TARGET "${CMAKE_CURRENT_SOURCE_DIR}/src/inter_biips.${MEX_EXT}")
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(doc)
add_subdirectory(tutorials)

# create archive
set(MATBIIPS_ARCHIVE "${CMAKE_BINARY_DIR}/matbiips_${Biips_VERSION}.tar.gz")
add_custom_command(
   OUTPUT ${MATBIIPS_ARCHIVE}
   DEPENDS matbiips
   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
   COMMAND ${CMAKE_COMMAND} -E tar czfv ${MATBIIPS_ARCHIVE}
      matbiips/matlab/inter_biips.${MEX_EXT} matbiips/matlab/*.m
      matbiips/matlab/private/*.m
      matbiips/tests/*.m
      matbiips/tests/*.bug
      matbiips/tuto/*.m
      matbiips/tuto/*.bug
   COMMENT "Packaging MatBiips"
   )
    
#define target
add_custom_target(matbiips_package
    DEPENDS  ${MATBIIPS_ARCHIVE}
)
