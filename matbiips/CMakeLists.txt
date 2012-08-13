find_package(Mex)
add_subdirectory(src)
add_subdirectory(tests)
#add tests
enable_testing()
add_test(test_cono "${MATLAB_COMMAND}" "<" matbiips/tests/cono.m)
