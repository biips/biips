#                                               -*- cmake -*-
#
#  CMakeLists.txt
#  Top level biips project CMakeList
#
#  @author: $LastChangedBy$
#  @date:   $LastChangedDate$
#  Id:      $Id$
#

cmake_minimum_required (VERSION 2.8)

# project configuration
project (Biips CXX)

set (Biips_VERSION_MAJOR 0)
set (Biips_VERSION_MINOR 7)
set (Biips_VERSION_PATCH 1)
set (Biips_VERSION ${Biips_VERSION_MAJOR}.${Biips_VERSION_MINOR}.${Biips_VERSION_PATCH})

file (WRITE VERSION ${Biips_VERSION})

set (Biips_AUTHOR Inria)
set (Biips_LICENSE unknown)
set (Biips_URL http://biips.gforge.inria.fr)
set (Biips_DESCRIPTION_SUMMARY "Bayesian Inference with Interacting Particle Systems")
set (Biips_DESCRIPTION )
set (Biips_CONTACT "biips-support <biips-support@lists.gforge.inria.fr>")
if (WIN32)
	execute_process (COMMAND "date" "/T" OUTPUT_VARIABLE Biips_DATE)
	string (REGEX REPLACE "(..)/(..)/(....).*" "\\3-\\2-\\1"
		Biips_DATE ${Biips_DATE})
elseif (UNIX)
	execute_process(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE Biips_DATE)
	string (REGEX REPLACE "(..)/(..)/(....).*" "\\3-\\2-\\1"
		Biips_DATE ${Biips_DATE})
else (WIN32)
	message (SEND_ERROR "date not implemented")
	set (Biips_DATE 0000-00-00)
endif (WIN32)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules/)

# add subprojects
add_subdirectory (BiipsCore)
add_subdirectory (BiipsBase)
add_subdirectory (BiipsCompiler)
add_subdirectory (BiipsTest)
add_subdirectory (BiipsTestCompiler)
add_subdirectory (RBiips)

# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_NAME libbiips)
set (CPACK_PACKAGE_VENDOR ${Biips_AUTHOR})

if (UNIX)
	execute_process(COMMAND "dpkg" "--print-architecture" OUTPUT_VARIABLE SYSTEM_ARCH)
	string(REGEX MATCH "[^\n]*" SYSTEM_ARCH ${SYSTEM_ARCH})
else (UNIX)
	set (SYSTEM_ARCH ${CMAKE_SYSTEM_PROCESSOR})
endif (UNIX)

set (CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}_${Biips_VERSION}_${CMAKE_SYSTEM_PROCESSOR}_${CMAKE_SYSTEM_NAME})
set (CPACK_PACKAGE_SOURCE_FILE_NAME ${CPACK_PACKAGE_NAME}-src_${Biips_VERSION})
set (CPACK_PACKAGE_VERSION_MAJOR ${Biips_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${Biips_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${Biips_VERSION_PATCH})
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY ${Biips_DESCRIPTION_SUMMARY})
set (CPACK_PACKAGE_INSTALL_DIRECTORY "BiiPS ${Biips_VERSION}")
set (CPACK_PACKAGE_DESCRIPTION_FILE ${Biips_SOURCE_DIR}/Description)
set (CPACK_PACKAGE_CONTACT ${Biips_CONTACT})
set (CPACK_RESOURCE_FILE_README ${Biips_SOURCE_DIR}/README)
set (CPACK_RESOURCE_FILE_WELCOME ${Biips_SOURCE_DIR}/Welcome)
set (CPACK_RESOURCE_FILE_LICENSE ${Biips_SOURCE_DIR}/COPYING)
set (CPACK_SOURCE_IGNORE_FILES /\\\\.svn/;.*~)

# debian package specific options
set (CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.7), libstdc++6 (>= 4.4.0)")
set (CPACK_DEBIAN_PACKAGE_SECTION non-free/math)
set (CPACK_DEBIAN_PACKAGE_PRIORITY optional)
set (CPACK_DEBIAN_PACKAGE_HOMEPAGE ${Biips_URL})
set (CPACK_DEBIAN_PACKAGE_REPLACES "libbiips-dev, libbiips-test, biips-test")

# rpm package specific options
set (CPACK_RPM_PACKAGE_REQUIRES "libc6 >= 2.7, libstdc++6 >= 4.4.0")
set (CPACK_RPM_PACKAGE_RELEASE 1)
set (CPACK_RPM_PACKAGE_LICENSE unknown)
set (CPACK_RPM_PACKAGE_GROUP unknown)
set (CPACK_RPM_PACKAGE_URL ${Biips_URL})
set (CPACK_RPM_CHANGELOG_FILE ${Biips_SOURCE_DIR}/Changelog)

include (CPack)

