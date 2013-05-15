# include directories
# configure config.hpp
include_directories (
	${INCLUDE_DIRS}
	${Compiler_INCLUDE_DIRS}
	${Base_INCLUDE_DIRS}
	${Core_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/testcompiler/include
    ${CMAKE_SOURCE_DIR}/pmmh/include
    )

file (GLOB SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/bidon.cpp
)

set (BIIPS_LIBS	BiipsCompiler BiipsBase BiipsCore)
add_executable(pmmh_start ${SOURCE_FILES})
target_link_libraries(pmmh_start ${BIIPS_LIBS} ${EXTRA_LIBS})
file(COPY hmm_1d_lin.bug dumpdata.R DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# for testing
set(current_pmm ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(prs_src_files ${current_pmm}/parse_varname.cpp ${current_pmm}/tests/parse_test.cpp)
add_executable(prs_test ${prs_src_files})
target_link_libraries(prs_test ${EXTRA_LIBS})
add_test(parse_varname_tes prs_test)
