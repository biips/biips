cmake_minimum_required (VERSION 2.8) 

include (GNUInstallDirs)

# project configuration
project (BiipsCompiler CXX)
set (BiipsCompiler_LIB_NAME ${PROJECT_NAME})
set (PROJECT_LIB_NAME ${BiipsCompiler_LIB_NAME})
string(REGEX MATCH [a-zA-Z0-9_.-]*$ PROJECT_REL_DIR ${PROJECT_SOURCE_DIR})
set (LIBRARY_OUTPUT_PATH ${CMAKE_INSTALL_LIBDIR}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})
set (BiipsCompiler_LIBRARY_DIR ${PROJECT_BINARY_DIR}/${LIBRARY_OUTPUT_PATH})

# find boost libraries
find_package(Boost 1.42.0 REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

# include directories
include_directories (
	${PROJECT_SOURCE_DIR}/include
	${BiipsBase_SOURCE_DIR}/include
	${BiipsCore_SOURCE_DIR}/include
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

# linked libraries
set (EXTRA_LIBS ${EXTRA_LIBS}
	BiipsCore
	BiipsBase
)

# add the library
add_library (${PROJECT_LIB_NAME} ${SOURCE_FILES})
target_link_libraries (${PROJECT_LIB_NAME} ${EXTRA_LIBS})
if (UNIX OR MINGW)
	set (EXTRA_CCFLAGS -Wno-deprecated)
	set_target_properties(${PROJECT_LIB_NAME} PROPERTIES COMPILE_FLAGS ${EXTRA_CCFLAGS})
endif (UNIX OR MINGW)

# add the install targets
install (TARGETS ${PROJECT_LIB_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}
	COMPONENT Runtime)

install(DIRECTORY
	${PROJECT_SOURCE_DIR}/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/biips/${PROJECT_REL_DIR}
	COMPONENT Development
	PATTERN .svn EXCLUDE
	PATTERN *.in EXCLUDE
)

