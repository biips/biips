# include directories
# configure config.hpp
include_directories(
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


# for testing
set(current_pmm ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(pmmh_test_files ${current_pmm}/tests/pmmh_test.cpp ${current_pmm}/pmmh.cpp)
add_executable(pmmh_test ${pmmh_test_files})
target_link_libraries(pmmh_test ${EXTRA_LIBS} ${BIIPS_LIBS})
get_target_property(exe_loc pmmh_test LOCATION)
file(COPY data_jags.R hmm_1d_nonlin_param.bug DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME pmmh_test COMMAND ${exe_loc})
