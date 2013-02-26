# configure config.hpp
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.in
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

# find qwt5-qt4
if (USE_Qwt5_Qt4)
    find_package(Qwt5 REQUIRED)
    if (NOT Qwt5_Qt4_FOUND)
    	message(FATAL_ERROR "Qwt5 linked with Qt4 not found")
    endif ()
    include_directories (${Qwt5_INCLUDE_DIR})
    include(UseQt4)
    set (EXTRA_LIBS ${EXTRA_LIBS}
    	${Qwt5_Qt4_LIBRARY}
    )
endif ()

# source files list generation
file (GLOB SOURCE_FILES
	${CMAKE_CURRENT_BINARY_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# add biips libraries
set (BIIPS_LIBS	BiipsBase BiipsCore)

# add the executable
add_executable(BiipsTest ${SOURCE_FILES})
target_link_libraries(BiipsTest ${EXTRA_LIBS} ${BIIPS_LIBS})

# add subdirectories
add_subdirectory(benchmarks)


# copy cfg files to binary directory
file(COPY biipstest.cfg  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


#=========== CTest commands ===============
file(GLOB cfg_files ${CMAKE_CURRENT_BINARY_DIR}/benchmarks/*.cfg)
foreach(_cfg ${cfg_files})
    # define name without .cfg extension
    get_filename_component(_name ${_cfg} NAME) 
    string (FIND ${_name} .cfg off REVERSE)
    string (SUBSTRING ${_name} 0 ${off} _name)
    get_target_property(exe_loc BiipsTest LOCATION)
    add_test (NAME ${_name}-test COMMAND ${exe_loc} ${_cfg} --particles=100
	    --data-rng-seed=42 --smc-rng-seed=42)
endforeach()
