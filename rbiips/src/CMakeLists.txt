set_output_bindir(out_bindir)

# copy files to binary directory
file(GLOB cpp_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
file(GLOB h_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)

set(src_files Makevars.in ${cpp_files} ${h_files})
file(COPY ${src_files} DESTINATION ${out_bindir})

# configure Makevars.win file
configure_file (
  Makevars.win.IN
  ${out_bindir}/Makevars.win
	@ONLY
)

set(src_files ${src_files} Makevars.win)

# add files in out_bindir to parent scope SRC_FILES list
add_sources(${out_bindir} src_files)

