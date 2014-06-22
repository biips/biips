set (EXE_NAME BiipsTestCompiler)
file(RELATIVE_PATH CURRENT_REL_DIR ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set (INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)

# configure config.hpp
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.IN
    ${CMAKE_CURRENT_BINARY_DIR}/include/config.hpp
)

# include directories
include_directories (
	${INCLUDE_DIRS}
	${Compiler_INCLUDE_DIRS}
	${Base_INCLUDE_DIRS}
	${Core_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/testcompiler/include
    )
# add boost libraries
set (EXTRA_LIBS ${EXTRA_LIBS})

# source files list generation
file (GLOB SOURCE_FILES
	${CMAKE_CURRENT_BINARY_DIR}/include/*.hpp
	${CMAKE_CURRENT_BINARY_DIR}/include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
    ${CMAKE_SOURCE_DIR}/testcompiler/include/storeUnregistered.hpp
)

# add biips libraries
set (BIIPS_LIBS	BiipsCompiler BiipsBase BiipsCore)

# add the executable
add_executable(${EXE_NAME} ${SOURCE_FILES})
target_link_libraries(${EXE_NAME} ${BIIPS_LIBS} ${EXTRA_LIBS})
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

# #add target for parse_test
# set(parse_src_files ${CMAKE_CURRENT_SOURCE_DIR}/src/parse_test/parse_test.cpp)
# add_executable(parse_test ${parse_src_files})
# target_link_libraries(parse_test ${EXTRA_LIBS})

# #add test for parse_test
# get_target_property(exe_loc parse_test LOCATION)
# add_test(NAME parse_test COMMAND ${exe_loc})

