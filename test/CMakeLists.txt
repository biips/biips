set (EXE_NAME BiipsTest)

# configure config.hpp
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.IN
    ${CMAKE_CURRENT_BINARY_DIR}/include/config.hpp
)

# include directories
include_directories (
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/include
	${Core_INCLUDE_DIRS}
	${Base_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

# source files list generation
file (GLOB SOURCE_FILES
	${CMAKE_CURRENT_BINARY_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# add biips libraries
set (BIIPS_LIBS	BiipsBase BiipsCore)

# add the executable
add_executable(${EXE_NAME} ${SOURCE_FILES})
target_link_libraries(${EXE_NAME} ${EXTRA_LIBS} ${BIIPS_LIBS})

# add subdirectories
add_subdirectory(cfg)

# copy cfg files to binary directory
file(COPY biipstest.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


#=========== CTest commands ===============
file(GLOB cfg_files ${CMAKE_CURRENT_BINARY_DIR}/cfg/*.cfg)
foreach(_cfg ${cfg_files})
    # define name without .cfg extension
    get_filename_component(_name ${_cfg} NAME) 
    string (FIND ${_name} .cfg off REVERSE)
    string (SUBSTRING ${_name} 0 ${off} _name)
    add_test (NAME ${_name}-test COMMAND $<TARGET_FILE:${EXE_NAME}> ${_cfg} --particles=100 --alpha=1e-5)
endforeach()
