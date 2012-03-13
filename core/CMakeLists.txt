cmake_minimum_required (VERSION 2.8) 

# project configuration
project (BiipsCore CXX)
set (BiipsCore_LIB_NAME ${PROJECT_NAME})
set (PROJECT_LIB_NAME ${BiipsCore_LIB_NAME})
string(REGEX MATCH [a-zA-Z0-9_.-]*$ PROJECT_REL_DIR ${PROJECT_SOURCE_DIR})

# find boost libraries
find_package(Boost 1.42.0 REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
	${PROJECT_SOURCE_DIR}/include/BiipsConfig.hpp.in
	${PROJECT_BINARY_DIR}/include/BiipsConfig.hpp
)

configure_file (
	${PROJECT_SOURCE_DIR}/biips_info.in
	${PROJECT_BINARY_DIR}/biips_info
)


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
add_library (${PROJECT_LIB_NAME} ${SOURCE_FILES})

# add the install targets
install (TARGETS ${PROJECT_LIB_NAME} DESTINATION ${Biips_INSTALL_LIBDIR}
	COMPONENT Runtime)

install(DIRECTORY
	${PROJECT_BINARY_DIR}/include/
	${PROJECT_SOURCE_DIR}/include/
	DESTINATION ${Biips_INSTALL_INCLUDEDIR}/${PROJECT_REL_DIR}
	COMPONENT Development
	PATTERN .svn EXCLUDE
	PATTERN *.in EXCLUDE
)

if (UNIX)
    set (BIIPS_INFO_PROGRAM_NAME biips_info)
elseif (WIN32)
    set (BIIPS_INFO_PROGRAM_NAME biips_info.bat)
endif (UNIX)
install(PROGRAMS
	${PROJECT_BINARY_DIR}/biips_info
	DESTINATION ${Biips_INSTALL_BINDIR}
	COMPONENT Runtime
	RENAME ${BIIPS_INFO_PROGRAM_NAME}
)

