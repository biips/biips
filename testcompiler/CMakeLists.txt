set (EXE_NAME BiipsTestCompiler)
file(RELATIVE_PATH CURRENT_REL_DIR ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set (INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)

# configure config.hpp
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/config.hpp
)

# include directories
include_directories (
	${INCLUDE_DIRS}
	${Compiler_INCLUDE_DIRS}
	${Base_INCLUDE_DIRS}
	${Core_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

# add boost libraries
set (EXTRA_LIBS ${EXTRA_LIBS})

# source files list generation
file (GLOB SOURCE_FILES
	${CMAKE_CURRENT_BINARY_DIR}/include/*.hpp
	${CMAKE_CURRENT_BINARY_DIR}/include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

# add biips libraries
set (BIIPS_LIBS	BiipsCompiler BiipsBase BiipsCore)

# add the executable
add_executable(BiipsTestCompiler ${SOURCE_FILES})
target_link_libraries(BiipsTestCompiler ${BIIPS_LIBS} ${EXTRA_LIBS})
# add subdirectories
add_subdirectory(benchmarks)
add_subdirectory(model)

# copy cfg files to binary directory
file(COPY  biipstestcompiler.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#=========== CTest commands ===============
file(GLOB cfg_files ${CMAKE_CURRENT_BINARY_DIR}/benchmarks/hmm_1d*.cfg)
foreach(_cfg ${cfg_files})
    # define name without .cfg extension
    get_filename_component(_name ${_cfg} NAME) 
    string (FIND ${_name} .cfg off REVERSE)
    string (SUBSTRING ${_name} 0 ${off} _name)
    get_target_property(exe_loc ${EXE_NAME} LOCATION)
    add_test (NAME ${_name}-testcompiler COMMAND ${exe_loc} ${_cfg}
        --particles=100 --alpha=1e-6)
endforeach()

