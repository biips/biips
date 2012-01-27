cmake_minimum_required (VERSION 2.6) 
project (BiipsCore CXX)


file(GLOB BiipsCore_SOURCE_FILES src/*/*.cpp)

add_library (BiipsCore SHARED ${BiipsCore_SOURCE_FILES})

include_directories (${BiipsCore_SOURCE_DIR}/include ${Biips_SOURCE_DIR}/boost.sandbox/statistics/random)

INSTALL(TARGETS BiipsCore DESTINATION lib)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_VERSION "0.7.0")
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.7), libstdc++6 (>= 4.4.0)")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "biips-support <biips-support@lists.gforge.inria.fr>")
 
INCLUDE(CPack)
