file(GLOB mfiles *.m)
set(test_files ${mfiles} hmm_1d_lin.bug)
copy_files_to_bindir(test_files ${CMAKE_CURRENT_BINARY_DIR})
