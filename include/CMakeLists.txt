# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
	${CMAKE_CURRENT_SOURCE_DIR}/BiipsConfig.hpp.IN
	${CMAKE_CURRENT_BINARY_DIR}/BiipsConfig.hpp
)

# include directories
set(Core_INCLUDE_DIRS 
	${CMAKE_CURRENT_SOURCE_DIR}/core
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR} # for BiipsConfig.hpp
	PARENT_SCOPE
)

set(Base_INCLUDE_DIRS 
	${CMAKE_CURRENT_SOURCE_DIR}/base
	${CMAKE_CURRENT_SOURCE_DIR}
	PARENT_SCOPE
)

set(Compiler_INCLUDE_DIRS 
	${CMAKE_CURRENT_SOURCE_DIR}/compiler
	${CMAKE_CURRENT_SOURCE_DIR}
	PARENT_SCOPE

)

# include files list generation
file (GLOB_RECURSE Core_INCLUDES
	${CMAKE_CURRENT_BINARY_DIR}/BiipsConfig.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/*.h
)
set(Core_INCLUDES ${Core_INCLUDES} PARENT_SCOPE)

file (GLOB_RECURSE Base_INCLUDES
	${CMAKE_CURRENT_SOURCE_DIR}/base/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/base/*.h
)
set(Base_INCLUDES ${Base_INCLUDES} PARENT_SCOPE)

file (GLOB_RECURSE Compiler_INCLUDES
	${CMAKE_CURRENT_SOURCE_DIR}/compiler/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/compiler/*.h
)
set(Compiler_INCLUDES ${Compiler_INCLUDES} PARENT_SCOPE)


if (WIN32)
	set(Compiler_INCLUDE_DIRS ${Compiler_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/win32)
endif()

if (WIN32)
	set(REGEX_EXCLUDE "CMakeFiles")
else()
	set(REGEX_EXCLUDE "CMakeFiles|win32")
endif()

# add the install targets
install(DIRECTORY
	${CMAKE_CURRENT_BINARY_DIR}/
	${CMAKE_CURRENT_SOURCE_DIR}/
	DESTINATION ${Biips_INSTALL_INCLUDEDIR}
	COMPONENT Headers
	FILES_MATCHING 
	PATTERN "*.h"
	PATTERN "*.hpp"
	REGEX ${REGEX_EXCLUDE} EXCLUDE
)

