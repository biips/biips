cmake_minimum_required (VERSION 2.6) 
project (BiipsCompiler CXX)


file(GLOB BiipsCompiler_SOURCE_FILES_1 src/*.cpp)
file(GLOB BiipsCompiler_SOURCE_FILES_2 src/*/*.cpp)
file(GLOB BiipsCompiler_SOURCE_FILES_3 src/*/*.cc)

add_library (BiipsCompiler SHARED ${BiipsCompiler_SOURCE_FILES_1}
				  ${BiipsCompiler_SOURCE_FILES_2}
				  ${BiipsCompiler_SOURCE_FILES_3})

include_directories (${BiipsCompiler_SOURCE_DIR}/include
		     ${BiipsBase_SOURCE_DIR}/include
		     ${BiipsCore_SOURCE_DIR}/include)

INSTALL(TARGETS BiipsCompiler DESTINATION lib)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "biips-support <biips-support@lists.gforge.inria.fr>")
 
INCLUDE(CPack)
