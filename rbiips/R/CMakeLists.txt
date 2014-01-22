set_output_bindir(out_bindir)

# copy files to binary directory
file(GLOB rfiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.R)
file(COPY ${rfiles} DESTINATION ${out_bindir})

# add files in out_bindir to parent scope SRC_FILES list
add_sources(${out_bindir} rfiles)
