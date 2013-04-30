# include directories
# configure config.hpp
include_directories (
	${INCLUDE_DIRS}
	${Compiler_INCLUDE_DIRS}
	${Base_INCLUDE_DIRS}
	${Core_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

file (GLOB SOURCE_FILES
	${CMAKE_CURRENT_BINARY_DIR}/include/*.hpp
	${CMAKE_CURRENT_BINARY_DIR}/include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/bidon.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)
set (BIIPS_LIBS	BiipsCompiler BiipsBase BiipsCore)
add_executable(pmmh_start bidon.cpp)
target_link_libraries(pmmh_start ${BIIPS_LIBS} ${EXTRA_LIBS})
file(COPY  hmm_1d_lin.bug DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
