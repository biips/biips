set_output_bindir(out_bindir)

# configure RBiips-package.Rd file
configure_file (
	RBiips-package.Rd.IN
	${out_bindir}/RBiips-package.Rd
	@ONLY
)

# copy files to binary directory
file(GLOB rd_files *.Rd)
file(COPY ${rd_files} DESTINATION ${out_bindir})

set(src_files ${rd_files} ${out_bindir}/RBiips-package.Rd)
# add dependencies to source files
add_targets_depends(src_files)
