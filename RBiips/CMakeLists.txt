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


project(RBiips)

if (BUILD_SHARED_LIBS)
    set (BIIPS_SHARED TRUE)
else ()
    set (BIIPS_SHARED FALSE)
endif()

macro(set_output_bindir var)
    file(RELATIVE_PATH CURRENT_REL_DIR ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/${PROJECT_NAME}/${CURRENT_REL_DIR}" ${var})
endmacro(set_output_bindir)

add_subdirectory(data)
add_subdirectory(demo)
add_subdirectory(inst)
add_subdirectory(man)
add_subdirectory(R)
add_subdirectory(src)


set_output_bindir(out_bindir)

# configure DESCRIPTION file
configure_file (
	DESCRIPTION.in
	${out_bindir}/DESCRIPTION
)

# configure README file
configure_file (
	README.in
	${out_bindir}/README
	@ONLY
)

# configure NOTICES file
configure_file (
	NOTICES.in
	${out_bindir}/NOTICES
)

# configure configure.ac file
configure_file (
	configure.ac.in
	${out_bindir}/configure.ac
	@ONLY
	NEWLINE_STYLE UNIX
)


# copy files to binary directory
set (src_files cleanup COPYING NAMESPACE)

file (COPY ${src_files}
	DESTINATION ${out_bindir}
)

# Generate configuration scripts
if (UNIX)
	find_program (AUTOCONF autoconf)
	if (${AUTOCONF} STREQUAL AUTOCONF-NOTFOUND)
		message (SEND_ERROR "autoconf not found")
	else ()
	    add_custom_command(OUTPUT ${out_bindir}/configure
	        COMMAND ${AUTOCONF}
		    WORKING_DIRECTORY ${out_bindir}
		    COMMENT "Autoconf generating configure file"
	    )
	    add_custom_target(configure
	        SOURCES ${out_bindir}/configure
	    )
	endif ()
endif ()


# FIXME: we need a FindR() module
# and introduce dependencies between targets

# find R
find_package(R 2.13 REQUIRED COMPONENTS Rcpp)


if (R_FOUND)
    set (pkg_dir ${out_bindir})
    
    # add R package tools targets
    add_custom_target (${PROJECT_NAME}_check
    	${R_EXECUTABLE} CMD check ${pkg_dir}
    	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    	COMMENT "Checking ${PROJECT_NAME} R package"
    	VERBATIM
    )
    if (UNIX)
        add_dependencies(${PROJECT_NAME}_check
            configure)
    endif()
    
    add_custom_target (${PROJECT_NAME}_build
    	${R_EXECUTABLE} CMD build ${pkg_dir}
    	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    	COMMENT "Building ${PROJECT_NAME} R source package"
    	VERBATIM
    )
    if (UNIX)
        add_dependencies(${PROJECT_NAME}_build
            configure)
    endif()
    
    add_custom_target (${PROJECT_NAME}_INSTALL
    	${R_EXECUTABLE} CMD INSTALL ${pkg_dir}
    	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    	COMMENT "Installing ${PROJECT_NAME} R package"
    	VERBATIM
    )
    if (UNIX)
        add_dependencies(${PROJECT_NAME}_INSTALL
            configure)
    endif()
    
    add_custom_target (${PROJECT_NAME}_INSTALL_build
    	${R_EXECUTABLE} CMD INSTALL --build ${pkg_dir}
    	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    	COMMENT "Building ${PROJECT_NAME} R binary package"
    	VERBATIM
    )
    if (UNIX)
        add_dependencies(${PROJECT_NAME}_INSTALL_build
            configure)
    endif()
else ()
	message (SEND_ERROR "R not found")
endif ()