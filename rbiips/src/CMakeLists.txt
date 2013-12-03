set_output_bindir(out_bindir)

# configure Makevars.win file
configure_file (
	Makevars.win.IN
	${out_bindir}/Makevars.win
	@ONLY
)

# copy files to binary directory
file(GLOB cpp_files *.cpp)
file(GLOB h_files *.h)

set(src_files ${CMAKE_CURRENT_SOURCE_DIR}/Makevars.in ${cpp_files} ${h_files})
file(COPY ${src_files} DESTINATION ${out_bindir})
# add dependencies to source files 
add_targets_depends(src_files)

