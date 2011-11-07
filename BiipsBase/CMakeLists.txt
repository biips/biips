cmake_minimum_required (VERSION 2.6) 
project (BiipsBase CXX)


file(GLOB BiipsBase_SOURCE_FILES_1 src/*.cpp)
file(GLOB BiipsBase_SOURCE_FILES_2 src/*/*.cpp)

add_library (BiipsBase SHARED ${BiipsBase_SOURCE_FILES_1}
			      ${BiipsBase_SOURCE_FILES_2})

include_directories (${BiipsBase_SOURCE_DIR}/include
		     ${BiipsCore_SOURCE_DIR}/include
		     ${Biips_SOURCE_DIR}/boost.sandbox/math_toolkit
		     ${Biips_SOURCE_DIR}/boost.sandbox/random
		     ${Biips_SOURCE_DIR}/boost.sandbox/statistics/distribution_common
		     ${Biips_SOURCE_DIR}/boost.sandbox/statistics/distribution_toolkit
		     ${Biips_SOURCE_DIR}/boost.sandbox/statistics/random)

INSTALL(TARGETS BiipsBase DESTINATION lib)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "biips-support <biips-support@lists.gforge.inria.fr>")
 
INCLUDE(CPack)
