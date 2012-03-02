cmake_minimum_required (VERSION 2.6) 

include (GNUInstallDirs)

# project configuration
project (BiipsCore CXX)
set (PROJECT_LIB_NAME ${PROJECT_NAME})
string(REGEX MATCH [a-zA-Z0-9_.-]*$ PROJECT_REL_DIR ${PROJECT_SOURCE_DIR})
set (LIBRARY_OUTPUT_PATH ${CMAKE_INSTALL_LIBDIR}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
	${PROJECT_SOURCE_DIR}/include/BiipsConfig.hpp.in
	${PROJECT_BINARY_DIR}/include/BiipsConfig.hpp
)

# find boost libraries
find_package(Boost 1.42.0 REQUIRED)
include_directories (${Boost_LIBRARY_DIRS})

# include directories
include_directories (
	${PROJECT_BINARY_DIR}/include
	${PROJECT_SOURCE_DIR}/include
)

# source files list generation
file (GLOB_RECURSE SOURCE_FILES
	${PROJECT_BINARY_DIR}/include/*.hpp
	${PROJECT_BINARY_DIR}/include/*.h
	${PROJECT_BINARY_DIR}/src/*.cpp
	${PROJECT_BINARY_DIR}/src/*.cc
	${PROJECT_SOURCE_DIR}/include/*.hpp
	${PROJECT_SOURCE_DIR}/include/*.h
	${PROJECT_SOURCE_DIR}/src/*.cpp
	${PROJECT_SOURCE_DIR}/src/*.cc
)

# add the library
add_library (${PROJECT_LIB_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_LIB_NAME} PROPERTIES COMPILE_FLAGS -Wno-deprecated)

# add the install targets
install (TARGETS ${PROJECT_LIB_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}
	COMPONENT Runtime)

install(DIRECTORY
	${PROJECT_BINARY_DIR}/include/
	${PROJECT_SOURCE_DIR}/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/biips/${PROJECT_REL_DIR}
	COMPONENT Development
	PATTERN .svn EXCLUDE
	PATTERN *.in EXCLUDE
)

