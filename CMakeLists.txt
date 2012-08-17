#                                               -*- cmake -*-
#
#  BiiPS software is a set of C++ libraries for
#  Bayesian inference with interacting Particle Systems.
#  Copyright (C) Inria, 2012
#  Authors: Adrien Todeschini, Francois Caron
#  
#  BiiPS is derived software based on:
#  JAGS, Copyright (C) Martyn Plummer, 2002-2010
#  SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
#
#  This file is part of BiiPS.
#
#  BiiPS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
#  \file     CMakeLists.txt
#  \brief    Top level Biips project CMakeList
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#

cmake_minimum_required (VERSION 2.8)


# project configuration
# =====================
project (Biips CXX)

set (Biips_VERSION_MAJOR 0)
set (Biips_VERSION_MINOR 7)
set (Biips_VERSION_PATCH 3)
set (Biips_VERSION ${Biips_VERSION_MAJOR}.${Biips_VERSION_MINOR}.${Biips_VERSION_PATCH})

file (WRITE VERSION ${Biips_VERSION})

set (Biips_OWNER Inria)
set (Biips_COPYRIGHT_YEAR 2012)
set (Biips_LICENSE GPL-3)
set (Biips_URL http://alea.bordeaux.inria.fr/biips)
set (Biips_SOURCES_URL https://gforge.inria.fr/projects/biips/)
set (Biips_DESCRIPTION_SUMMARY "Bayesian inference with interacting Particle Systems")
set (Biips_CONTACT "biips-project <biips-project@lists.gforge.inria.fr>")
set (Biips_CONTACT_EMAIL "biips-project@lists.gforge.inria.fr")
set (Biips_DESCRIPTION " BiiPS is free software, licensed under the GPL.
 It borrows from the BUGS/JAGS software, widely used in Bayesian statistics, the statistical modeling with graphical models and the language associated with their descriptions.
 Both tools can be used by non-statisticians, via their automated \"black box\" inference engines.
 Unlike BUGS oriented MCMC (Markov chain Monte Carlo), BiiPS software is based on sequential Monte Carlo algorithms (SMC), also referred to as \"interacting particles\" or \"particle filtering\" and is thus more adapted to dynamic problems (tracking, signal filtering, etc).")

file (WRITE Description ${Biips_DESCRIPTION})

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

# configure resource files
configure_file (README.md.in README.txt @ONLY)
#configure_file (Welcome Welcome.txt COPYONLY)
configure_file (COPYING COPYING.txt COPYONLY)

# configure NOTICES file
configure_file (
	${PROJECT_SOURCE_DIR}/NOTICES.in
	${PROJECT_BINARY_DIR}/NOTICES
)

# set dependencies versions
set (JAGS_VERSION 3.2.0)
set (SMCTC_VERSION 1.0-RC4)
set (Boost_VERSION_REQUIRED 1.46.1)

set (rjags_VERSION 3.2)
set (Rcpp_VERSION 0.9.4)
set (R_VERSION 2.13.0)


# print system info
option(PRINT_SYSTEM_INFO "Print system information when running cmake" OFF)
if (PRINT_SYSTEM_INFO)
    message (CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME})
    message (CMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION})
    message (CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR})
    message (WIN32=${WIN32})
    message (UNIX=${UNIX})
    message (MINGW=${MINGW})
    message (CMAKE_GENERATOR=${CMAKE_GENERATOR})
    message (CMAKE_BUILD_TOOL=${CMAKE_BUILD_TOOL})
    message (CMAKE_SIZEOF_VOID_P=${CMAKE_SIZEOF_VOID_P})
    message (CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})
    message (CMAKE_COMPILER_IS_GNUCXX=${CMAKE_COMPILER_IS_GNUCXX})
endif (PRINT_SYSTEM_INFO)

# configure build
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules/)

set (BUILD_ARCH_LIST "")
set (BUILD_BITNB_LIST "")

if (WIN32)
    option (BUILD_BOTH_ARCH "Force i386 (32bit) and x64 (64bit) compilation." ON)
    if (BUILD_BOTH_ARCH)
        list (APPEND BUILD_ARCH_LIST i386)
        list (APPEND BUILD_BITNB_LIST 32)
        set (BUILD_i386 ON)
        list (APPEND BUILD_ARCH_LIST x64)
        list (APPEND BUILD_BITNB_LIST 64)
        set (BUILD_x64 ON)
    endif (BUILD_BOTH_ARCH)
endif (WIN32)

math (EXPR ARCH_BITNB 8*${CMAKE_SIZEOF_VOID_P})
if (ARCH_BITNB EQUAL 32)
    if (NOT BUILD_i386)
    	list (APPEND BUILD_ARCH_LIST i386)
        list (APPEND BUILD_BITNB_LIST 32)
        set (BUILD_i386 ON)
    endif (NOT BUILD_i386)
elseif (ARCH_BITNB EQUAL 64)
    if (NOT BUILD_x64)
    	list (APPEND BUILD_ARCH_LIST x64)
        list (APPEND BUILD_BITNB_LIST 64)
        set (BUILD_x64 ON)
    endif (NOT BUILD_x64)
endif (ARCH_BITNB EQUAL 32)

option (SUPPRESS_DEPRECATED_WARNINGS "Suppress deprecated warnings: -Wno-deprecated." ON)
if (SUPPRESS_DEPRECATED_WARNINGS)
	add_definitions (-Wno-deprecated)
	set (EXTRA_COMPILE_FLAGS "${EXTRA_COMPILE_FLAGS} -Wno-deprecated")
endif (SUPPRESS_DEPRECATED_WARNINGS)

# configure boost
option (Boost_USE_STATIC_LIBS "Force the use of the static boost libraries" ON)
option (Boost_USE_MULTITHREADED "Use the non-multithreaded boost libraries" OFF)
option (Boost_USE_STATIC_RUNTIME
	"If enabled, searches for boost libraries linked against a static C++ standard library ('s' ABI tag)" OFF)

macro (clear_boost)
    unset (Boost_LIBRARIES)
    unset (Boost_INCLUDE_DIR CACHE)
    unset (Boost_LIBRARY_DIRS CACHE)
endmacro (clear_boost)

# should we use qwt5-qt4
if (NOT WIN32)
    option(USE_Qwt5_Qt4 "Use qwt5-qt4 library to plot results of tests" OFF)
endif (NOT WIN32)

# configure install directories
include (GNUInstallDirs)
if (UNIX)
	set (Biips_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
	set (Biips_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
	set (Biips_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/biips)
	set (Biips_INSTALL_TESTROOTDIR ${CMAKE_INSTALL_DATAROOTDIR}/biips)
elseif (WIN32)
    set (Biips_INSTALL_BINDIR_i386 i386/${CMAKE_INSTALL_BINDIR})
    set (Biips_INSTALL_BINDIR_x64 x64/${CMAKE_INSTALL_BINDIR})
	set (Biips_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
	set (Biips_INSTALL_DATAROOTDIR ${CMAKE_INSTALL_DATAROOTDIR})
endif (UNIX)

# configure output directories
set (Biips_LIBRARY_OUTPUT_PATH ${CMAKE_BUILD_TYPE}/${CMAKE_SYSTEM_NAME})
set (Biips_EXECUTABLE_OUTPUT_PATH ${CMAKE_BUILD_TYPE}/${CMAKE_SYSTEM_NAME})
set (LIBRARY_OUTPUT_PATH ${Biips_LIBRARY_OUTPUT_PATH})
set (EXECUTABLE_OUTPUT_PATH ${Biips_EXECUTABLE_OUTPUT_PATH})


# configure interfaces build
option (BUILD_RBIIPS "Activate RBiips (BiiPS interface for GNU R) build" ON)
option (BUILD_MATBIIPS "Activate MatBiips (BiiPS interface for Matlab) build" ON)


# macro to add libraries
macro (add_biips_library name type sources biips_libs)
    if (WIN32)
        foreach (arch IN LISTS BUILD_ARCH_LIST)
            add_library (${name}-${arch} ${type} ${${sources}})
            list(FIND BUILD_ARCH_LIST ${arch} offset)
            list(GET BUILD_BITNB_LIST ${offset} bitnb)
            set_target_properties(${name}-${arch}
                PROPERTIES COMPILE_FLAGS -m${bitnb}
                LINK_FLAGS -m${bitnb}
                )
            set (extra_libs "")
            foreach (lib IN LISTS ${biips_libs})
                list(APPEND extra_libs ${lib}-${arch})
            endforeach (lib IN LISTS ${biips_libs})
            target_link_libraries (${name}-${arch} ${extra_libs})
        endforeach(arch IN LISTS BUILD_ARCH_LIST)
    else (WIN32)
        add_library (${name} ${type} ${${sources}})
        target_link_libraries (${name} ${${biips_libs}})
    endif(WIN32)
endmacro (add_biips_library)

# macro to install libraries
macro (install_biips_library name)
    if (WIN32)
        foreach (arch IN LISTS BUILD_ARCH_LIST)
            list(FIND BUILD_ARCH_LIST ${arch} offset)
            list(GET BUILD_BITNB_LIST ${offset} bitnb)
            install (TARGETS ${name}-${arch}
                DESTINATION ${Biips_INSTALL_BINDIR_${arch}}
            	COMPONENT "Runtime_${bitnb}bit"
            	)
        endforeach (arch IN LISTS BUILD_ARCH_LIST)
    else (WIN32)
        install (TARGETS ${name}
            DESTINATION ${Biips_INSTALL_LIBDIR}
            )
    endif (WIN32)
endmacro (install_biips_library)

# macro to install script
macro (install_biips_script dir name)
    if (WIN32)
        foreach (arch IN LISTS BUILD_ARCH_LIST)
            list(FIND BUILD_ARCH_LIST ${arch} offset)
            list(GET BUILD_BITNB_LIST ${offset} bitnb)
            install (PROGRAMS ${dir}/${name}
                DESTINATION ${Biips_INSTALL_BINDIR_${arch}}
                RENAME ${name}.bat
            	COMPONENT "Runtime_${bitnb}bit"
            	)
        endforeach (arch IN LISTS BUILD_ARCH_LIST)
    else (WIN32)
        install (PROGRAMS ${dir}/${name}
            DESTINATION ${Biips_INSTALL_BINDIR}
            )
    endif (WIN32)
endmacro (install_biips_script)

# macro to add executable
macro (add_biips_executable name sources extra_libs extra_libs64 biips_libs)
    if (WIN32)
        foreach (arch IN LISTS BUILD_ARCH_LIST)
            add_executable (${name}-${arch} ${${sources}})
            list(FIND BUILD_ARCH_LIST ${arch} offset)
            list(GET BUILD_BITNB_LIST ${offset} bitnb)
            set_target_properties(${name}-${arch}
                PROPERTIES COMPILE_FLAGS -m${bitnb}
                LINK_FLAGS -m${bitnb}
                )
                
            if (bitnb EQUAL 64)
                foreach (lib IN LISTS ${biips_libs})
                    list(APPEND ${extra_libs64} ${lib}-${arch})
                endforeach (lib IN LISTS ${biips_libs})
                target_link_libraries (${name}-${arch} ${${extra_libs64}})
            else (bitnb EQUAL 64)
                foreach (lib IN LISTS ${biips_libs})
                    list(APPEND ${extra_libs} ${lib}-${arch})
                endforeach (lib IN LISTS ${biips_libs})
                target_link_libraries (${name}-${arch} ${${extra_libs}})
            endif (bitnb EQUAL 64)
        endforeach(arch IN LISTS BUILD_ARCH_LIST)
    else (WIN32)
        add_executable (${name} ${${sources}})
        target_link_libraries (${name} ${${extra_libs}} ${${biips_libs}})
    endif(WIN32)
endmacro (add_biips_executable)

# macro to install test program
macro (install_biips_test_program name)
    if (WIN32)
        foreach (arch IN LISTS BUILD_ARCH_LIST)
            list(FIND BUILD_ARCH_LIST ${arch} offset)
            list(GET BUILD_BITNB_LIST ${offset} bitnb)
            install (TARGETS ${name}-${arch}
                DESTINATION ${Biips_INSTALL_BINDIR_${arch}}
            	COMPONENT "Runtime_${bitnb}bit"
            	)
        endforeach (arch IN LISTS BUILD_ARCH_LIST)
    else (WIN32)
        install (TARGETS ${name}
            DESTINATION ${Biips_INSTALL_TESTROOTDIR}/${PROJECT_REL_DIR}/bin
            )
    endif (WIN32)
endmacro (install_biips_test_program)


# macro to install test files
macro (install_biips_test_file name dir)
    if (WIN32)
        install (FILES
        	${name}
        	DESTINATION ${Biips_INSTALL_DATADIR}/${PROJECT_REL_DIR}/${dir}
        )
    else (WIN32)
        install (FILES
        	${name}
        	DESTINATION ${Biips_INSTALL_TESTROOTDIR}/${PROJECT_REL_DIR}/${dir}
        )
    endif (WIN32)
endmacro (install_biips_test_file)

# macro to install test file directories
macro (install_biips_test_dir dir patt)
    if (WIN32)
        install(DIRECTORY
        	${PROJECT_SOURCE_DIR}/${dir}/
        	DESTINATION ${Biips_INSTALL_DATADIR}/${PROJECT_REL_DIR}/${dir}
        	FILES_MATCHING PATTERN ${patt}
        	PATTERN .svn EXCLUDE
        )
    else (WIN32)
        install(DIRECTORY
        	${PROJECT_SOURCE_DIR}/${dir}/
        	DESTINATION ${Biips_INSTALL_TESTROOTDIR}/${PROJECT_REL_DIR}/${dir}
        	FILES_MATCHING PATTERN ${patt}
        	PATTERN .svn EXCLUDE
        )
    endif (WIN32)
endmacro (install_biips_test_dir)


# for static compilations to work with MATLAB
add_definitions(-fPIC)
enable_testing()
# ==============================
# ==============================
# Add subprojects
# ==============================
# ==============================
add_subdirectory (core)
add_subdirectory (base)
add_subdirectory (test)
add_subdirectory (compiler)
add_subdirectory (testcompiler)
if (BUILD_RBIIPS)
    add_subdirectory (rbiips)
endif (BUILD_RBIIPS)
if (BUILD_MATBIIPS)
    find_package(Mex)
    add_subdirectory (matbiips)
endif (BUILD_MATBIIPS)


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

# Generic package options
set (CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}_${Biips_VERSION}_${CMAKE_SYSTEM_PROCESSOR}_${CMAKE_SYSTEM_NAME})
set (CPACK_PACKAGE_SOURCE_FILE_NAME ${CPACK_PACKAGE_NAME}-src_${Biips_VERSION})
set (CPACK_PACKAGE_VERSION_MAJOR ${Biips_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${Biips_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${Biips_VERSION_PATCH})
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY ${Biips_DESCRIPTION_SUMMARY})
set (CPACK_PACKAGE_INSTALL_DIRECTORY "BiiPS-${Biips_VERSION}")
set (CPACK_PACKAGE_DESCRIPTION_FILE ${Biips_SOURCE_DIR}/Description)
set (CPACK_PACKAGE_CONTACT ${Biips_CONTACT})
set (CPACK_RESOURCE_FILE_README ${PROJECT_BINARY_DIR}/README.txt)
#set (CPACK_RESOURCE_FILE_WELCOME ${PROJECT_BINARY_DIR}/Welcome.txt)
set (CPACK_RESOURCE_FILE_LICENSE ${PROJECT_BINARY_DIR}/COPYING.txt)
set (CPACK_SOURCE_IGNORE_FILES /\\\\.svn/;.*~;${CPACK_SOURCE_IGNORE_FILES})
set (CPACK_PACKAGE_ICON "${Biips_SOURCE_DIR}\\\\doc\\\\biips-logo\\\\biips-logo-large-256.bmp")

#set (CPACK_PACKAGE_EXECUTABLES "biips_info;Biips info")

# Linux Debian package specific options
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
set (CPACK_DEBIAN_PACKAGE_SECTION math)
set (CPACK_DEBIAN_PACKAGE_PRIORITY optional)
set (CPACK_DEBIAN_PACKAGE_HOMEPAGE ${Biips_URL})
set (CPACK_DEBIAN_PACKAGE_REPLACES "libbiips, libbiips-dev, libbiips-test, biips-test")

# Linux RPM package specific options
set (CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.7, libstdc++46 >= 4.4.0, boost-devel >= 1.42.0")
if (USE_Qwt5_Qt4)
    set (CPACK_RPM_PACKAGE_REQUIRES
        "${CPACK_RPM_PACKAGE_REQUIRES}, libqwt5 >= 5.2.0")
endif (USE_Qwt5_Qt4)
set (CPACK_RPM_PACKAGE_RELEASE 1)
set (CPACK_RPM_PACKAGE_LICENSE ${Biips_LICENSE})
set (CPACK_RPM_PACKAGE_GROUP Applications/Engineering)
set (CPACK_RPM_PACKAGE_URL ${Biips_URL})
set (CPACK_RPM_CHANGELOG_FILE ${Biips_SOURCE_DIR}/ChangeLog)

# Windows NSIS package specific options
if (WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set (CPACK_NSIS_EXECUTABLES_DIRECTORY ${Biips_INSTALL_BINDIR})
  set (CPACK_NSIS_INSTALL_ROOT "\$PROGRAMFILES64\\\\BiiPS")
  set (CPACK_NSIS_DISPLAY_NAME "BiiPS ${Biips_VERSION}")
  set (CPACK_NSIS_HELP_LINK ${Biips_URL})
  set (CPACK_NSIS_URL_INFO_ABOUT ${Biips_URL})
  set (CPACK_NSIS_CONTACT ${Biips_CONTACT})
  set (CPACK_NSIS_MUI_ICON "${Biips_SOURCE_DIR}/doc/biips-logo/biips-icon-48.ico")
  set (CPACK_NSIS_MUI_UNICON "${Biips_SOURCE_DIR}/doc/biips-logo/biips-icon-dark-blue-48.ico")
endif (WIN32 AND NOT UNIX)
include (CPack)
