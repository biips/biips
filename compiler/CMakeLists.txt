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
#  \brief    BiipsCompiler CMakeLists
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#


set (LIB_NAME BiipsCompiler)
file(RELATIVE_PATH CURRENT_REL_DIR ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set (INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)
set (${LIB_NAME}_INCLUDE_DIRS "${INCLUDE_DIRS}" PARENT_SCOPE)

# include directories
include_directories (
	${INCLUDE_DIRS}
	${BiipsBase_INCLUDE_DIRS}
	${BiipsCore_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

# source files list generation
file (GLOB_RECURSE SOURCE_FILES
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

# add the library
add_biips_library(${LIB_NAME} SOURCE_FILES BIIPS_LIBS)

# add the install targets
install_biips_library(${LIB_NAME} Runtime)

install(DIRECTORY
	${CMAKE_CURRENT_SOURCE_DIR}/include/
	DESTINATION ${Biips_INSTALL_INCLUDEDIR}/${CURRENT_REL_DIR}
	COMPONENT Headers
	PATTERN .svn EXCLUDE
	PATTERN *.in EXCLUDE
)

