file(GLOB mfiles *.m)
copy_files_to_bindir(mfiles ${CMAKE_CURRENT_BINARY_DIR})

set (MATBIIPS_FILES ${MATBIIPS_FILES} ${mfiles})