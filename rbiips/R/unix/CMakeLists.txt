set_output_bindir(out_bindir)

# copy files to binary directory
set(zzz_file zzz.R)
file(COPY ${zzz_file} DESTINATION ${out_bindir})

# add dependencies to source files
add_targets_depends(zzz_file)
