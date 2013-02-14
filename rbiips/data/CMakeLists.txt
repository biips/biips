set_output_bindir(out_bindir)

# copy files to binary directory
file(GLOB rdata_files *.RData)
file(COPY ${rdata_files} DESTINATION ${out_bindir})


# add dependencies to source files
add_targets_depends(rdata_files)
