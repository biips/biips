file(RELATIVE_PATH CURRENT_REL_DIR ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
	${CMAKE_CURRENT_SOURCE_DIR}/include/BiipsConfig.hpp.IN
	${CMAKE_CURRENT_BINARY_DIR}/include/BiipsConfig.hpp
)

configure_file (
	${CMAKE_CURRENT_SOURCE_DIR}/biips_info.IN
	${CMAKE_CURRENT_BINARY_DIR}/biips_info
)

# includes 
include_directories(${Core_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

# source files list generation
file (GLOB_RECURSE Core_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
set(Core_SRC ${Core_SRC} PARENT_SCOPE)

# add the library
add_library(BiipsCore ${Core_SRC})

# add the install targets
install(TARGETS BiipsCore DESTINATION ${Biips_INSTALL_LIBDIR})

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/biips_info DESTINATION ${Biips_INSTALL_BINDIR})

install(DIRECTORY
	${CMAKE_CURRENT_BINARY_DIR}/include/
	${CMAKE_CURRENT_SOURCE_DIR}/include/
	DESTINATION ${Biips_INSTALL_INCLUDEDIR}/${CURRENT_REL_DIR}
	COMPONENT Headers
	PATTERN .svn EXCLUDE
	PATTERN *.IN EXCLUDE
)
