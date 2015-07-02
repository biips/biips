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

# copy include files to binary directory and add them to parent scope Base_INCLUDE_FILES list
file(GLOB_RECURSE _hdr RELATIVE ${CMAKE_SOURCE_DIR} include/*.hpp include/*.hh include/*.h)
foreach(_f ${_hdr})
    configure_file(${CMAKE_SOURCE_DIR}/${_f} ${CMAKE_BINARY_DIR}/${_f} COPYONLY)
endforeach()
set(Base_INCLUDE_FILES ${Base_INCLUDE_FILES} ${_hdr} PARENT_SCOPE)

# copy source files to binary directory and add them to parent scope Base_SOURCE_FILES list
file(GLOB_RECURSE _src RELATIVE ${CMAKE_SOURCE_DIR} src/*.cpp src/*.cc src/*.c src/*.h)
foreach(_f ${_src})
    configure_file(${CMAKE_SOURCE_DIR}/${_f} ${CMAKE_BINARY_DIR}/${_f} COPYONLY)
endforeach()
set(Base_SOURCE_FILES ${Base_SOURCE_FILES} ${_src} PARENT_SCOPE)
