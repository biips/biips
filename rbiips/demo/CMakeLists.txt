set_output_bindir(out_bindir)

# copy files to binary directory
file(GLOB rfiles *.R)
set(src_files 00Index ${rfiles})
file(COPY ${src_files} DESTINATION ${out_bindir})


# add dependencies to source files
add_targets_depends(src_files)
