#                                               -*- cmake -*-
#
#  BiiPS software is a set of C++ libraries for
#  Bayesian inference with interacting Particle Systems.
#  Copyright (C) Inria, 2012
#  Contributors: Adrien Todeschini, Francois Caron
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
#  \brief    BiipsTest subproject CMakeList
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#

cmake_minimum_required (VERSION 2.8) 

# project configuration
project (BiipsTest CXX C)
set (BiipsTest_EXE_NAME ${PROJECT_NAME})
set (PROJECT_EXE_NAME ${BiipsTest_EXE_NAME})
string(REGEX MATCH [a-zA-Z0-9_.-]*$ PROJECT_REL_DIR ${PROJECT_SOURCE_DIR})

# find boost libraries
find_package(Boost ${BOOST_VERSION_REQUIRED} REQUIRED
	program_options
)
include_directories (${Boost_INCLUDE_DIRS})
set (EXTRA_LIBS ${EXTRA_LIBS}
	${Boost_LIBRARIES}
)

# should we use qwt5-qt4
option(USE_Qwt5_Qt4 "Use qwt5-qt4 library to plot results of tests" OFF)

# find qwt5-qt4
if (USE_Qwt5_Qt4)
    find_package(Qwt5 REQUIRED)
    if (NOT Qwt5_Qt4_FOUND)
    	message(FATAL_ERROR "Qwt5 linked with Qt4 not found")
    endif (NOT Qwt5_Qt4_FOUND)
    include_directories (${Qwt5_INCLUDE_DIR})
    include(UseQt4)
    set (EXTRA_LIBS ${EXTRA_LIBS}
    	${Qwt5_Qt4_LIBRARY}
    )
endif (USE_Qwt5_Qt4)


# configure config.hpp
configure_file(
    ${PROJECT_SOURCE_DIR}/include/config.hpp.in
    ${PROJECT_BINARY_DIR}/include/config.hpp
)


# include Biips libraries
include_directories (
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_BINARY_DIR}/include
	${BiipsBase_SOURCE_DIR}/include
	${BiipsCore_SOURCE_DIR}/include
)

# source files list generation
file (GLOB SOURCE_FILES
	${PROJECT_BINARY_DIR}/include/*.hpp
	${PROJECT_BINARY_DIR}/include/*.h
	${PROJECT_BINARY_DIR}/src/*.cpp
	${PROJECT_BINARY_DIR}/src/*.cc
	${PROJECT_SOURCE_DIR}/include/*.hpp
	${PROJECT_SOURCE_DIR}/include/*.h
	${PROJECT_SOURCE_DIR}/src/*.cpp
	${PROJECT_SOURCE_DIR}/src/*.cc
)

# linked libraries
set (EXTRA_LIBS ${EXTRA_LIBS}
    BiipsBase
	BiipsCore
)

# add the executable
add_executable (${PROJECT_EXE_NAME} ${SOURCE_FILES})
target_link_libraries (${PROJECT_EXE_NAME} ${EXTRA_LINK_FLAGS} ${EXTRA_LIBS})

# add the install targets
install (TARGETS ${PROJECT_EXE_NAME}
	DESTINATION ${Biips_INSTALL_TESTROOTDIR}/${PROJECT_REL_DIR}/bin
	COMPONENT Test
)

install (FILES
	biipstest.cfg
	DESTINATION ${Biips_INSTALL_TESTROOTDIR}/${PROJECT_REL_DIR}
	COMPONENT Test
)

# TODO: .bat script for windows
install (PROGRAMS
	BiipsTest.sh
	DESTINATION ${Biips_INSTALL_TESTROOTDIR}/${PROJECT_REL_DIR}
	COMPONENT Test
)

install(DIRECTORY
	${PROJECT_SOURCE_DIR}/bench/
	DESTINATION ${Biips_INSTALL_TESTROOTDIR}/${PROJECT_REL_DIR}/bench
	COMPONENT Test
	FILES_MATCHING PATTERN *.cfg
	PATTERN .svn EXCLUDE
)

