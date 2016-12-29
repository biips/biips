set (EXE_NAME biipstestcompiler)

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
	${Util_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
    )

# source files list generation
file (GLOB SOURCE_FILES
	${CMAKE_CURRENT_BINARY_DIR}/include/*.hpp
	${CMAKE_CURRENT_BINARY_DIR}/include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# add biips libraries
set (BIIPS_LIBS biipsutil biipscompiler biipsbase biipscore)

# add the executable
add_executable(${EXE_NAME} ${SOURCE_FILES})
target_link_libraries(${EXE_NAME} ${EXTRA_LIBS} ${BIIPS_LIBS})
# add subdirectories
add_subdirectory(cfg)
add_subdirectory(model)

# copy cfg files to binary directory
file(COPY biipstestcompiler.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#=========== CTest commands ===============
file(GLOB cfg_files ${CMAKE_CURRENT_BINARY_DIR}/cfg/hmm_1d*.cfg)
foreach(_cfg ${cfg_files})
    # define name without .cfg extension
    get_filename_component(_name ${_cfg} NAME) 
    string (FIND ${_name} .cfg off REVERSE)
    string (SUBSTRING ${_name} 0 ${off} _name)
    add_test (NAME ${_name}-testcompiler COMMAND $<TARGET_FILE:${EXE_NAME}> ${_cfg}
        --particles=100 --alpha=1e-5)
endforeach()

# #add target for parse_test
# set(parse_src_files ${CMAKE_CURRENT_SOURCE_DIR}/src/parse_test/parse_test.cpp)
# add_executable(parse_test ${parse_src_files})
# target_link_libraries(parse_test ${EXTRA_LIBS})

# #add test for parse_test
# get_target_property(exe_loc parse_test LOCATION)
# add_test(NAME parse_test COMMAND ${exe_loc})

