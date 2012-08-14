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
#  \brief    BiipsCore subproject CMakeList
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#

cmake_minimum_required (VERSION 2.8) 

# project configuration
project (BiipsCore CXX)
set (BiipsCore_LIB_NAME ${PROJECT_NAME})
set (PROJECT_LIB_NAME ${BiipsCore_LIB_NAME})
string(REGEX MATCH [a-zA-Z0-9_.-]*$ PROJECT_REL_DIR ${PROJECT_SOURCE_DIR})

# find boost libraries
find_package(Boost ${BOOST_VERSION_REQUIRED} REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
	${PROJECT_SOURCE_DIR}/include/BiipsConfig.hpp.in
	${PROJECT_BINARY_DIR}/include/BiipsConfig.hpp
)

configure_file (
	${PROJECT_SOURCE_DIR}/biips_info.in
	${PROJECT_BINARY_DIR}/biips_info
)


# include directories
include_directories (
	${PROJECT_BINARY_DIR}/include
	${PROJECT_SOURCE_DIR}/include
)

# source files list generation
file (GLOB_RECURSE SOURCE_FILES
	${PROJECT_BINARY_DIR}/include/*.hpp
	${PROJECT_BINARY_DIR}/include/*.h
	${PROJECT_BINARY_DIR}/src/*.cpp
	${PROJECT_BINARY_DIR}/src/*.cc
	${PROJECT_SOURCE_DIR}/include/*.hpp
	${PROJECT_SOURCE_DIR}/include/*.h
	${PROJECT_SOURCE_DIR}/src/*.cpp
	${PROJECT_SOURCE_DIR}/src/*.cc
)

# add the library
add_biips_library(${PROJECT_LIB_NAME} SHARED SOURCE_FILES "")
add_biips_library(${PROJECT_LIB_NAME}-static STATIC SOURCE_FILES "")

# add the install targets
install_biips_library(${PROJECT_LIB_NAME})

install_biips_script(${PROJECT_BINARY_DIR} biips_info)

install(DIRECTORY
	${PROJECT_BINARY_DIR}/include/
	${PROJECT_SOURCE_DIR}/include/
	DESTINATION ${Biips_INSTALL_INCLUDEDIR}/${PROJECT_REL_DIR}
	COMPONENT Headers
	PATTERN .svn EXCLUDE
	PATTERN *.in EXCLUDE
)

