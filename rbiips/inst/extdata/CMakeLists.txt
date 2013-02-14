set_output_bindir(out_bindir)

# copy files to binary directory
file(GLOB bugs_files *.bug)
file(COPY ${bugs_files} DESTINATION ${out_bindir})

# add dependencies to source files
add_targets_depends(bugs_files)
