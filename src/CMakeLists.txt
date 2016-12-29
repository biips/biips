
# include directories
include_directories (
	${Base_INCLUDE_DIRS} 
	${Core_INCLUDE_DIRS}
	${Compiler_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

# source files list generation
file (GLOB_RECURSE Core_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
)
set(Core_SRC ${Core_SRC} PARENT_SCOPE)

file (GLOB_RECURSE Base_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/base/*.cpp
)
set(Base_SRC ${Base_SRC} PARENT_SCOPE)

file (GLOB_RECURSE Compiler_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/compiler/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compiler/*.cc
)
set(Compiler_SRC ${Compiler_SRC} PARENT_SCOPE)

# add the library
add_library(BiipsCore ${Core_INCLUDES} ${Core_SRC})
add_library(BiipsBase ${Base_INCLUDES} ${Base_SRC})
add_library(BiipsCompiler ${Compiler_INCLUDES} ${Compiler_SRC})

# add the install targets
install(TARGETS BiipsCore DESTINATION ${Biips_INSTALL_LIBDIR})
install(TARGETS BiipsBase DESTINATION ${Biips_INSTALL_LIBDIR})
install(TARGETS BiipsCompiler DESTINATION ${Biips_INSTALL_LIBDIR})

