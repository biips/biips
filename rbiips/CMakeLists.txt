#                                               -*- cmake -*-
#
#  RBiips package for GNU R is an interface to BiiPS C++ libraries for
#  Bayesian inference with interacting Particle Systems.
#  Copyright (C) Inria, 2012
#  Authors: Adrien Todeschini, Francois Caron
#  
#  RBiips is derived software based on:
#  BiiPS, Copyright (C) Inria, 2012
#  rjags, Copyright (C) Martyn Plummer, 2002-2010
#  Rcpp, Copyright (C) Dirk Eddelbuettel and Romain Francois, 2009-2011
#
#  This file is part of RBiips.
#
#  RBiips is free software: you can redistribute it and/or modify
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
#  \brief    RBiips subproject CMakeList
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#

cmake_minimum_required (VERSION 2.8) 

# project configuration
project (RBiips)
set (PROJECT_PKG_NAME ${PROJECT_NAME})
string (REGEX MATCH [a-zA-Z0-9_.-]*$ PROJECT_REL_DIR ${PROJECT_SOURCE_DIR})

# configure DESCRIPTION file
configure_file (
	DESCRIPTION.in
	${PROJECT_PKG_NAME}/DESCRIPTION
)

# configure README file
configure_file (
	README.in
	${PROJECT_PKG_NAME}/README
	@ONLY
)

# configure NOTICES file
configure_file (
	NOTICES.in
	${PROJECT_PKG_NAME}/NOTICES
)

# configure R/windows/zzz.R file
configure_file (
	R/windows/zzz.R.in
	${PROJECT_PKG_NAME}/R/windows/zzz.R
)

# configure src/Makevars.in file
configure_file (
	src/Makevars.in.in
	${PROJECT_PKG_NAME}/src/Makevars.in
	COPYONLY
)

# configure src/Makevars.win file
configure_file (
	src/Makevars.win.in
	${PROJECT_PKG_NAME}/src/Makevars.win
	@ONLY
)

# configure configure.ac file
configure_file (
	configure.ac.in
	${PROJECT_PKG_NAME}/configure.ac
	@ONLY
	NEWLINE_STYLE UNIX
)

# configure RBiips-package.Rd file
configure_file (
	man/RBiips-package.Rd.in
	${PROJECT_PKG_NAME}/man/RBiips-package.Rd
	@ONLY
)


# copy files to binary directory
macro (copy_dir_to_binary dir)
	file ( GLOB SOURCE_FILES
		${PROJECT_SOURCE_DIR}/${dir}/*
	)
	file (COPY ${SOURCE_FILES}
		DESTINATION ${PROJECT_PKG_NAME}/${dir}/
		PATTERN CMakeLists.txt EXCLUDE
		PATTERN *.in EXCLUDE
		PATTERN .svn EXCLUDE
		PATTERN .project EXCLUDE
		PATTERN .cproject EXCLUDE
		PATTERN *~ EXCLUDE
	)
endmacro(copy_dir_to_binary)

copy_dir_to_binary(.)
copy_dir_to_binary(man)
copy_dir_to_binary(R)
copy_dir_to_binary(R/unix)
copy_dir_to_binary(R/windows)
copy_dir_to_binary(src)

# Generate configuration scripts
if (UNIX)
	find_program (AUTOCONF autoconf)
	if (AUTOCONF)
		execute_process (COMMAND autoconf
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/${PROJECT_PKG_NAME}
		)
	else (AUTOCONF)
		message (SEND_ERROR "autoconf not found")
	endif (AUTOCONF)
endif (UNIX)

# add R package tools targets
add_custom_target (${PROJECT_PKG_NAME}_check
	R CMD check ${PROJECT_REL_DIR}/${PROJECT_PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Checking ${PROJECT_PKG_NAME} R package"
	VERBATIM
)

add_custom_target (${PROJECT_PKG_NAME}_build
	R CMD build ${PROJECT_REL_DIR}/${PROJECT_PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Building ${PROJECT_PKG_NAME} R source package"
	VERBATIM
)

add_custom_target (${PROJECT_PKG_NAME}_INSTALL
	R CMD INSTALL ${PROJECT_REL_DIR}/${PROJECT_PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Installing ${PROJECT_PKG_NAME} R package"
	VERBATIM
)

add_custom_target (${PROJECT_PKG_NAME}_INSTALL_build
	R CMD INSTALL --build ${PROJECT_REL_DIR}/${PROJECT_PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Building ${PROJECT_PKG_NAME} R binary package"
	VERBATIM
)

