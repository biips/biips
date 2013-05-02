# include directories
# configure config.hpp
include_directories (
	${INCLUDE_DIRS}
	${Compiler_INCLUDE_DIRS}
	${Base_INCLUDE_DIRS}
	${Core_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/testcompiler/include
)

file (GLOB SOURCE_FILES
	${CMAKE_CURRENT_BINARY_DIR}/include/*.hpp
	${CMAKE_CURRENT_BINARY_DIR}/include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/bidon.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
    ${CMAKE_SOURCE_DIR}/testcompiler/include/storeUnregistered.hpp
    ${CMAKE_SOURCE_DIR}/testcompiler/src/storeUnregistered.cpp
)
set (BIIPS_LIBS	BiipsCompiler BiipsBase BiipsCore)
add_executable(pmmh_start ${SOURCE_FILES})
target_link_libraries(pmmh_start ${BIIPS_LIBS} ${EXTRA_LIBS})
file(COPY hmm_1d_lin.bug dumpdata.R DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
