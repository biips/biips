file(RELATIVE_PATH CURRENT_REL_DIR ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
# include directories
include_directories (
	${Base_INCLUDE_DIRS} 
	${Core_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

# source files list generation
file (GLOB_RECURSE Base_SRC
	${CMAKE_CURRENT_BINARY_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
set(Base_SRC ${Base_SRC} PARENT_SCOPE)

# add the library
add_library(BiipsBase ${Base_SRC})

# add the install targets
install(TARGETS BiipsBase DESTINATION ${Biips_INSTALL_LIBDIR})

install(DIRECTORY
	${CMAKE_CURRENT_SOURCE_DIR}/include/
	DESTINATION ${Biips_INSTALL_INCLUDEDIR}/${CURRENT_REL_DIR}
	COMPONENT Headers
	PATTERN .svn EXCLUDE
	PATTERN *.in EXCLUDE
)
