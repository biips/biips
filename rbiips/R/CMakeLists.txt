set_output_bindir(out_bindir)

# copy files to binary directory
file(GLOB rfiles *.R)
file(COPY ${rfiles} DESTINATION ${out_bindir})

# add dependencies to source files 
add_targets_depends(rfiles)
