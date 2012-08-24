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
#  \brief    RBiips CMakeLists
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#


set (PKG_NAME RBiips)
file(RELATIVE_PATH CURRENT_REL_DIR ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})

if (BUILD_SHARED_LIBS)
    set (BIIPS_SHARED TRUE)
else ()
    set (BIIPS_SHARED FALSE)
endif()


# configure DESCRIPTION file
configure_file (
	DESCRIPTION.in
	${PKG_NAME}/DESCRIPTION
)

# configure README file
configure_file (
	README.in
	${PKG_NAME}/README
	@ONLY
)

# configure NOTICES file
configure_file (
	NOTICES.in
	${PKG_NAME}/NOTICES
)

# configure R/windows/zzz.R file
configure_file (
	R/windows/zzz.R.in
	${PKG_NAME}/R/windows/zzz.R
)

# configure src/Makevars.in file
configure_file (
	src/Makevars.in.in
	${PKG_NAME}/src/Makevars.in
	COPYONLY
)

# configure src/Makevars.win file
configure_file (
	src/Makevars.win.in
	${PKG_NAME}/src/Makevars.win
	@ONLY
)

# configure configure.ac file
configure_file (
	configure.ac.in
	${PKG_NAME}/configure.ac
	@ONLY
	NEWLINE_STYLE UNIX
)

# configure RBiips-package.Rd file
configure_file (
	man/RBiips-package.Rd.in
	${PKG_NAME}/man/RBiips-package.Rd
	@ONLY
)


# copy files to binary directory
file ( GLOB SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*
)
file (COPY ${SOURCE_FILES}
	DESTINATION ${PKG_NAME}/
	PATTERN CMakeLists.txt EXCLUDE
	PATTERN *.in EXCLUDE
	PATTERN .svn EXCLUDE
	PATTERN .project EXCLUDE
	PATTERN .cproject EXCLUDE
	PATTERN *~ EXCLUDE
)

# Generate configuration scripts
if (UNIX)
	find_program (AUTOCONF autoconf)
	if (AUTOCONF)
		execute_process (COMMAND autoconf
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PKG_NAME}
		)
	else ()
		message (SEND_ERROR "autoconf not found")
	endif ()
endif ()

# FIXME: we need a FindR() module
# and introduce dependencies between targets

# add R package tools targets
add_custom_target (${PKG_NAME}_check
	R CMD check ${CURRENT_REL_DIR}/${PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Checking ${PKG_NAME} R package"
	VERBATIM
)

add_custom_target (${PKG_NAME}_build
	R CMD build ${CURRENT_REL_DIR}/${PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Building ${PKG_NAME} R source package"
	VERBATIM
)

add_custom_target (${PKG_NAME}_INSTALL
	R CMD INSTALL ${CURRENT_REL_DIR}/${PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Installing ${PKG_NAME} R package"
	VERBATIM
)

add_custom_target (${PKG_NAME}_INSTALL_build
	R CMD INSTALL --build ${CURRENT_REL_DIR}/${PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Building ${PKG_NAME} R binary package"
	VERBATIM
)

