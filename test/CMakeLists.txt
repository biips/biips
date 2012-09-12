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
#  \brief    BiipsTest CMakeLists
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#


set (EXE_NAME BiipsTest)
file(RELATIVE_PATH CURRENT_REL_DIR ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set (INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)

# configure config.hpp
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/config.hpp
)

# include directories
include_directories (
	${INCLUDE_DIRS}
	${BiipsCore_INCLUDE_DIRS}
	${BiipsBase_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

# add boost libraries
set (EXTRA_LIBS ${EXTRA_LIBS}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
)

# add boost 64-bit libraries
if (WIN32 AND BUILD_x64)
    set (EXTRA_LIBS64 ${EXTRA_LIBS64}
    	${Boost_PROGRAM_OPTIONS_LIBRARY_x64}
    )
endif ()

# find qwt5-qt4
if (USE_Qwt5_Qt4)
    find_package(Qwt5 REQUIRED)
    if (NOT Qwt5_Qt4_FOUND)
    	message(FATAL_ERROR "Qwt5 linked with Qt4 not found")
    endif ()
    include_directories (${Qwt5_INCLUDE_DIR})
    include(UseQt4)
    set (EXTRA_LIBS ${EXTRA_LIBS}
    	${Qwt5_Qt4_LIBRARY}
    )
endif ()

# source files list generation
file (GLOB SOURCE_FILES
	${CMAKE_CURRENT_BINARY_DIR}/include/*.hpp
	${CMAKE_CURRENT_BINARY_DIR}/include/*.h
	${CMAKE_CURRENT_BINARY_DIR}/src/*.cpp
	${CMAKE_CURRENT_BINARY_DIR}/src/*.cc
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
)

# add biips libraries
set (BIIPS_LIBS	BiipsBase BiipsCore)

# add the executable
add_biips_executable (${EXE_NAME} SOURCE_FILES EXTRA_LIBS EXTRA_LIBS64 BIIPS_LIBS)

if (WIN32)
    foreach (arch IN LISTS BUILD_ARCH_LIST)
        get_target_property(exe_loc ${EXE_NAME}-${arch} LOCATION)
        add_custom_command(TARGET ${EXE_NAME}-${arch}
            POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy ${exe_loc} "${CMAKE_CURRENT_BINARY_DIR}/${arch}/"
            COMMENT "Copying ${EXE_NAME} in build testcompiler/${arch} directory"
            VERBATIM
        )
    endforeach()
else()
    get_target_property(exe_loc ${EXE_NAME} LOCATION)
    add_custom_command(TARGET ${EXE_NAME}
        POST_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy ${exe_loc} "${CMAKE_CURRENT_BINARY_DIR}/"
        COMMENT "Copying ${EXE_NAME} in build testcompiler directory"
        VERBATIM
    )
endif()


# add subdirectories
add_subdirectory(benchmarks)


# copy cfg files to binary directory
set (cfg_file biipstest.cfg)
copy_files_to_bindir(cfg_file ${CMAKE_CURRENT_BINARY_DIR})


#=========== CTest commands ===============
file(GLOB cfg_files ${CMAKE_CURRENT_BINARY_DIR}/benchmarks/*.cfg)
foreach(_cfg ${cfg_files})
    # define name without .cfg extension
    get_filename_component(_name ${_cfg} NAME) 
    string (FIND ${_name} .cfg off REVERSE)
    string (SUBSTRING ${_name} 0 ${off} _name)
    
    if (WIN32)
        foreach (arch IN LISTS BUILD_ARCH_LIST)
            get_target_property(exe_loc ${EXE_NAME}-${arch} LOCATION)
            get_filename_component(exe_outname ${exe_loc}-${arch} NAME)
            add_test (NAME ${_name}-test-${arch}
                COMMAND ${arch}/${exe_outname} ${_cfg} --particles=100
            )
        endforeach()
    else ()
        get_target_property(exe_loc ${EXE_NAME} LOCATION)
        get_filename_component(exe_outname ${exe_loc} NAME)
        add_test (NAME ${_name}-test
            COMMAND ${exe_outname} ${_cfg} --particles=100
        )
    endif()
endforeach()
