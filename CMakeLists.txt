#                                               -*- cmake -*-
#
#  CMakeLists.txt
#  Top level Biips project CMakeList
#
#  @author: $LastChangedBy:$
#  @date:   $LastChangedDate:$
#  @version:$LastChangedRevision:$
#  Id:      $Id:$
#

cmake_minimum_required (VERSION 2.8)

# project configuration
# =====================
project (Biips CXX)

option(PRINT_SYSTEM_INFO "Print system information when running cmake" OFF)
if (PRINT_SYSTEM_INFO)
    message (STATUS CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME})
    message (STATUS CMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION})
    message (STATUS CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR})
    message (STATUS WIN32=${WIN32})
    message (STATUS UNIX=${UNIX})
    message (STATUS MINGW=${MINGW})
    message (STATUS CMAKE_GENERATOR=${CMAKE_GENERATOR})
    message (STATUS CMAKE_BUILD_TOOL=${CMAKE_BUILD_TOOL})
    message (STATUS CMAKE_SIZEOF_VOID_P=${CMAKE_SIZEOF_VOID_P})
    message (STATUS CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})
    message (STATUS CMAKE_COMPILER_IS_GNUCXX=${CMAKE_COMPILER_IS_GNUCXX})
endif (PRINT_SYSTEM_INFO)

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

if (NOT Biips_DATE)
	if (WIN32)
		execute_process (COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE TODAY)
	elseif (UNIX)
		execute_process(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE TODAY)
	endif (WIN32)
	if (TODAY)
		string (REGEX REPLACE
			"(..)/(..)/(....).*"
			"\\3-\\2-\\1"
			Biips_DATE
			${TODAY}
		)
	else (TODAY)
		message (SEND_ERROR "date not implemented.\nPlease set variable Biips_DATE to yyyy-mm-dd")
	endif (TODAY)
endif (NOT Biips_DATE)
message (STATUS Biips_DATE=${Biips_DATE})

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules/)

# configure build
math (EXPR BUILD_ARCH_BIT_NB 8*${CMAKE_SIZEOF_VOID_P})
option (BUILD_FORCE_32BIT "Force 32bit compilation." OFF)
option (BUILD_FORCE_64BIT "Force 64bit compilation." OFF)
if (BUILD_FORCE_32BIT)
	if (BUILD_FORCE_64BIT)
		message (FATAL_ERROR "Can not set both BUILD_FORCE_32BIT and BUILD_FORCE_64BIT to ON.\n")
	endif (BUILD_FORCE_64BIT)
	set (BUILD_ARCH_BIT_NB 32)
	add_definitions (-m32)
	set (EXTRA_COMPILE_FLAGS -m32)
	set (EXTRA_LINK_FLAGS -m32)
elseif (BUILD_FORCE_64BIT)
	set (BUILD_ARCH_BIT_NB 64)
	add_definitions (-m64)
	set (EXTRA_COMPILE_FLAGS -m64)
	set (EXTRA_LINK_FLAGS -m64)
endif (BUILD_FORCE_32BIT)
message (STATUS BUILD_ARCH_BIT_NB=${BUILD_ARCH_BIT_NB})

option (SUPPRESS_DEPRECATED_WARNINGS "Suppress deprecated warnings: -Wno-deprecated." ON)
if (SUPPRESS_DEPRECATED_WARNINGS)
	add_definitions (-Wno-deprecated)
	set (EXTRA_COMPILE_FLAGS "${EXTRA_COMPILE_FLAGS} -Wno-deprecated")
endif (SUPPRESS_DEPRECATED_WARNINGS)

# configure boost
option (Boost_USE_STATIC_LIBS "Force the use of the static boost libraries" ON)
option (Boost_USE_MULTITHREADED "Use the non-multithreaded boost libraries" ON)
option (Boost_USE_STATIC_RUNTIME
	"If enabled, searches for boost libraries linked against a static C++ standard library ('s' ABI tag)" OFF)

# configure install directories
include (GNUInstallDirs)
if (UNIX)
	set (Biips_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
	set (Biips_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
	set (Biips_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/biips)
	set (Biips_INSTALL_TESTROOTDIR ${CMAKE_INSTALL_DATAROOTDIR}/biips)
elseif (WIN32)
	if (BUILD_ARCH_BIT_NB EQUAL 32)
		set (Biips_INSTALL_BINDIR i386/${CMAKE_INSTALL_BINDIR})
	elseif (BUILD_ARCH_BIT_NB EQUAL 64)
		set (Biips_INSTALL_BINDIR x64/${CMAKE_INSTALL_BINDIR})
	endif (BUILD_ARCH_BIT_NB EQUAL 32)
	set (Biips_INSTALL_LIBDIR ${Biips_INSTALL_BINDIR})
	set (Biips_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
	set (Biips_INSTALL_TESTROOTDIR ${CMAKE_INSTALL_DATAROOTDIR})
endif (UNIX)

# configure output directories
set (LIBRARY_OUTPUT_PATH ${CMAKE_INSTALL_LIBDIR}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_INSTALL_BINDIR}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})

# Add subprojects
# ===============
add_subdirectory (core)
add_subdirectory (base)
add_subdirectory (test)
add_subdirectory (compiler)
add_subdirectory (testcompiler)
add_subdirectory (rbiips)

#============= CPACK commands ============
# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_NAME biips)
set (CPACK_PACKAGE_VENDOR BiiPS)

#if (UNIX)
#	execute_process(COMMAND "dpkg" "--print-architecture" OUTPUT_VARIABLE SYSTEM_ARCH)
#	string(REGEX MATCH "[^\n]*" SYSTEM_ARCH ${SYSTEM_ARCH})
#else (UNIX)
#	set (SYSTEM_ARCH ${CMAKE_SYSTEM_PROCESSOR})
#endif (UNIX)

set (CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}_${Biips_VERSION}_${CMAKE_SYSTEM_PROCESSOR}_${CMAKE_SYSTEM_NAME})
set (CPACK_PACKAGE_SOURCE_FILE_NAME ${CPACK_PACKAGE_NAME}-src_${Biips_VERSION})
set (CPACK_PACKAGE_VERSION_MAJOR ${Biips_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${Biips_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${Biips_VERSION_PATCH})
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY ${Biips_DESCRIPTION_SUMMARY})
set (CPACK_PACKAGE_INSTALL_DIRECTORY "BiiPS-${Biips_VERSION}")
set (CPACK_PACKAGE_DESCRIPTION_FILE ${Biips_SOURCE_DIR}/Description)
set (CPACK_PACKAGE_CONTACT ${Biips_CONTACT})
set (CPACK_RESOURCE_FILE_README ${Biips_SOURCE_DIR}/README)
set (CPACK_RESOURCE_FILE_WELCOME ${Biips_SOURCE_DIR}/Welcome)
set (CPACK_RESOURCE_FILE_LICENSE ${Biips_SOURCE_DIR}/COPYING)

#set (CPACK_PACKAGE_EXECUTABLES "biips_info;Biips info")

set (CPACK_SOURCE_IGNORE_FILES /\\\\.svn/;.*~;${CPACK_SOURCE_IGNORE_FILES})

# Debian package specific options
set (CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.7), libstdc++6 (>= 4.4.0)")
if (NOT Boost_USE_STATIC_LIBS)
    set (CPACK_DEBIAN_PACKAGE_DEPENDS
        "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libboost-program-options-dev (>= 1.42.0)")
endif (NOT Boost_USE_STATIC_LIBS)
set (CPACK_DEBIAN_PACKAGE_SUGGESTS "libboost-dev (>= 1.42.0), r-base (>= 2.13)")
if (USE_Qwt5_Qt4)
    set (CPACK_DEBIAN_PACKAGE_DEPENDS
        "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libqwt5-qt4 (>= 5.2.0)")
endif (USE_Qwt5_Qt4)
set (CPACK_DEBIAN_PACKAGE_SECTION non-free/math)
set (CPACK_DEBIAN_PACKAGE_PRIORITY optional)
set (CPACK_DEBIAN_PACKAGE_HOMEPAGE ${Biips_URL})
set (CPACK_DEBIAN_PACKAGE_REPLACES "libbiips-dev, libbiips-test, biips-test")

# RPM package specific options
set (CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.7, libstdc++46 >= 4.4.0, boost-devel >= 1.42.0")
if (USE_Qwt5_Qt4)
    set (CPACK_RPM_PACKAGE_REQUIRES
        "${CPACK_RPM_PACKAGE_REQUIRES}, libqwt5 >= 5.2.0")
endif (USE_Qwt5_Qt4)
set (CPACK_RPM_PACKAGE_RELEASE 1)
set (CPACK_RPM_PACKAGE_LICENSE unknown)
set (CPACK_RPM_PACKAGE_GROUP unknown)
set (CPACK_RPM_PACKAGE_URL ${Biips_URL})
set (CPACK_RPM_CHANGELOG_FILE ${Biips_SOURCE_DIR}/ChangeLog)

# NSIS package specific options
if (WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set (CPACK_NSIS_EXECUTABLES_DIRECTORY ${Biips_INSTALL_BINDIR})
  set (CPACK_NSIS_INSTALL_ROOT "\$PROGRAMFILES64\\\\BiiPS")
  set (CPACK_NSIS_DISPLAY_NAME "BiiPS ${Biips_VERSION}")
  set (CPACK_NSIS_HELP_LINK ${Biips_URL})
  set (CPACK_NSIS_URL_INFO_ABOUT ${Biips_URL})
  set (CPACK_NSIS_CONTACT ${Biips_CONTACT})
endif (WIN32 AND NOT UNIX)

include (CPack)
