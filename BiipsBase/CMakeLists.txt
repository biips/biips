cmake_minimum_required (VERSION 2.6)

# project configuration
project (BiipsBase CXX)
set (LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

# find boost libraries
find_package(Boost 1.42.0 REQUIRED)
include_directories (${Boost_LIBRARY_DIRS})

# include directories
include_directories (
	${PROJECT_SOURCE_DIR}/include
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

# add the library
add_library (${PROJECT_NAME} SHARED ${SOURCE_FILES})

# add the install targets
install (TARGETS ${PROJECT_NAME} DESTINATION lib
	COMPONENT library)

install(DIRECTORY
	${PROJECT_SOURCE_DIR}/include/
	DESTINATION include/biips/${PROJECT_NAME}
	COMPONENT development
	PATTERN .svn EXCLUDE
	PATTERN *.in EXCLUDE
)

