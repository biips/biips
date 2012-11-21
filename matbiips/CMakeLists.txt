add_subdirectory(matlab)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(tuto)

# create archive
set(MATBIIPS_ARCHIVE "${CMAKE_BINARY_DIR}/matbiips_${Biips_VERSION}.zip")
add_custom_command(
   OUTPUT ${MATBIIPS_ARCHIVE}
   DEPENDS matbiips
   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
   COMMAND ${CMAKE_COMMAND} -E tar caf ${MATBIIPS_ARCHIVE}
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
