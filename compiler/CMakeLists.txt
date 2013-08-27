file(RELATIVE_PATH CURRENT_REL_DIR ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
# include directories
include_directories (
	${Compiler_INCLUDE_DIRS}
	${Base_INCLUDE_DIRS}
	${Core_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

# source files list generation
file (GLOB_RECURSE SOURCE_FILES
	${CMAKE_CURRENT_BINARY_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
)

# add biips libraries
#set(BIIPS_LIBS	BiipsBase BiipsCore)

# add the library
add_library(BiipsCompiler ${SOURCE_FILES})

# add the install targets
install(TARGETS BiipsCompiler DESTINATION ${Biips_INSTALL_LIBDIR})

install(DIRECTORY
	${CMAKE_CURRENT_SOURCE_DIR}/include/
	DESTINATION ${Biips_INSTALL_INCLUDEDIR}/${CURRENT_REL_DIR}
	COMPONENT Headers
	PATTERN .svn EXCLUDE
	PATTERN *.in EXCLUDE
)

