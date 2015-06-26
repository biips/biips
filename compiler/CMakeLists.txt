file(RELATIVE_PATH CURRENT_REL_DIR ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
# include directories
include_directories (
	${Compiler_INCLUDE_DIRS}
	${Base_INCLUDE_DIRS}
	${Core_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

# source files list generation
file (GLOB_RECURSE Compiler_SRC
	${CMAKE_CURRENT_BINARY_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
)
set(Compiler_SRC ${Compiler_SRC} PARENT_SCOPE)

# add the library
add_library(BiipsCompiler ${Compiler_SRC})

# add the install targets
install(TARGETS BiipsCompiler DESTINATION ${Biips_INSTALL_LIBDIR})

install(DIRECTORY
	${CMAKE_CURRENT_SOURCE_DIR}/include/
	DESTINATION ${Biips_INSTALL_INCLUDEDIR}/${CURRENT_REL_DIR}
	COMPONENT Headers
	PATTERN .svn EXCLUDE
	PATTERN *.IN EXCLUDE
)

# add files to parent scope Compiler_SOURCES list
file(GLOB_RECURSE _src RELATIVE ${CMAKE_SOURCE_DIR} *.hpp *.hh *.h *.cpp *.cc *.c)
foreach(_f ${_src})
    configure_file(${CMAKE_SOURCE_DIR}/${_f} ${CMAKE_BINARY_DIR}/${_f} COPYONLY)
endforeach()
set(Compiler_SOURCES ${Compiler_SOURCES} ${_src} PARENT_SCOPE)
