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

macro(set_output_bindir var)
    file(RELATIVE_PATH CURRENT_REL_DIR ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/${PROJECT_NAME}/${CURRENT_REL_DIR}" ${var})
endmacro(set_output_bindir)

set_output_bindir(out_bindir)


# find R
find_package(R ${R_VERSION_REQUIRED} REQUIRED COMPONENTS Rcpp)

if (R_FOUND)
    set (pkg_dir ${out_bindir})
    
    if (WIN32)
        if (BUILD_i386)
		    set(BIIPS_LIBS BiipsCompiler-i386 BiipsBase-i386 BiipsCore-i386)
		endif ()
        if (BUILD_x64)
		    set(BIIPS_LIBS ${BIIPS_LIBS} BiipsCompiler-x64 BiipsBase-x64 BiipsCore-x64)
		endif ()
    else()
		set(BIIPS_LIBS BiipsCompiler BiipsBase BiipsCore)
    endif()
    
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set (R_INSTALL_FLAGS -d)
    endif()
		
    # add R CMD check target
    add_custom_target (${PROJECT_NAME}_check
    	DEPENDS ${BIIPS_LIBS}
    	COMMAND ${R_EXECUTABLE} CMD check ${PROJECT_NAME}
    	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    	COMMENT "Checking ${PROJECT_NAME} R package"
    	VERBATIM
    )
    if (NOT WIN32)
        add_dependencies(${PROJECT_NAME}_check
            ${out_bindir}/configure
        )
    endif()
    
    
    if (WIN32)
        # add R CMD INSTALL --build target
        add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_${Biips_VERSION}.zip
    	    DEPENDS ${BIIPS_LIBS}
        	COMMAND ${R_EXECUTABLE} CMD INSTALL --build ${R_INSTALL_FLAGS} ${pkg_dir}
        	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        	COMMENT "Building ${PROJECT_NAME} R binary package"
        	VERBATIM
        )
        add_custom_target (${PROJECT_NAME}_INSTALL_build
    	    DEPENDS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_${Biips_VERSION}.zip
        )
    elseif (APPLE)
        # add R CMD INSTALL --build target
        add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_${Biips_VERSION}.tar.gz
    	    DEPENDS ${BIIPS_LIBS} ${out_bindir}/configure
        	COMMAND ${R_EXECUTABLE} CMD INSTALL --build ${R_INSTALL_FLAGS} ${pkg_dir}
        	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        	COMMENT "Building ${PROJECT_NAME} R binary package"
        	VERBATIM
        )
        add_custom_target (${PROJECT_NAME}_INSTALL_build
    	    DEPENDS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_${Biips_VERSION}.tar.gz
        )
    elseif (UNIX)
        # add R CMD build target
        add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_${Biips_VERSION}.tar.gz
    	    DEPENDS ${BIIPS_LIBS} ${out_bindir}/configure
        	COMMAND ${R_EXECUTABLE} CMD build ${pkg_dir}
        	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        	COMMENT "Building ${PROJECT_NAME} R source package"
        	VERBATIM
        )
        add_custom_target (${PROJECT_NAME}_build
    	    DEPENDS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_${Biips_VERSION}.tar.gz
        )
        
        # add R CMD INSTALL target
        add_custom_target (${PROJECT_NAME}_INSTALL
        	DEPENDS ${BIIPS_LIBS} ${out_bindir}/configure
        	COMMAND ${R_EXECUTABLE} CMD INSTALL ${R_INSTALL_FLAGS} ${pkg_dir}
        	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        	COMMENT "Installing ${PROJECT_NAME} R package"
        	VERBATIM
        )
    endif()
else ()
    message (SEND_ERROR "R not found")
endif ()


macro(add_targets_depends src_files)
    if (R_FOUND)
        add_dependencies(${PROJECT_NAME}_check
            ${${src_files}}
        )
        
        if (WIN32 OR APPLE)
            add_dependencies(${PROJECT_NAME}_INSTALL_build
                ${${src_files}}
            )
        elseif(UNIX)
            add_dependencies(${PROJECT_NAME}_build
                ${${src_files}}
            )
			add_dependencies(${PROJECT_NAME}_INSTALL
				${${src_files}}
			)
        endif(WIN32 OR APPLE)
    endif(R_FOUND)
endmacro(add_targets_depends)

# add subdirectories
add_subdirectory(data)
add_subdirectory(demo)
add_subdirectory(inst)
add_subdirectory(man)
add_subdirectory(R)
add_subdirectory(src)


# Generate configuration scripts
if (NOT WIN32)
	find_program (AUTOCONF autoconf)
	if (${AUTOCONF} STREQUAL AUTOCONF-NOTFOUND)
		message (SEND_ERROR "autoconf not found")
	else ()
	    add_custom_command(OUTPUT ${out_bindir}/configure
		    DEPENDS ${out_bindir}/configure.ac
	        COMMAND ${AUTOCONF}
		    WORKING_DIRECTORY ${out_bindir}
		    COMMENT "Autoconf generating configure file"
	    )
	endif ()
endif ()


# configure DESCRIPTION file
configure_file (
	DESCRIPTION.in
	${out_bindir}/DESCRIPTION
	@ONLY
)
set (RBiips_SOURCES ${RBiips_SOURCES} ${CMAKE_CURRENTBINARY_DIR}/DESCRIPTION)

# configure README file
configure_file (
	README.in
	${out_bindir}/README
	@ONLY
)
set (RBiips_SOURCES ${RBiips_SOURCES} ${CMAKE_CURRENTBINARY_DIR}/README)

# configure NOTICES file
configure_file (
	NOTICES.in
	${out_bindir}/NOTICES
	@ONLY
)
set (RBiips_SOURCES ${RBiips_SOURCES} ${CMAKE_CURRENTBINARY_DIR}/README)

# configure configure.ac file
configure_file (
	configure.ac.in
	${out_bindir}/configure.ac
	@ONLY
	NEWLINE_STYLE UNIX
)


# copy files to binary directory
set (src_files cleanup COPYING NAMESPACE)
copy_files_to_bindir (src_files ${out_bindir})


# add dependencies to source files
set (src_files ${src_files}
    ${out_bindir}/DESCRIPTION
    ${out_bindir}/README
    ${out_bindir}/NOTICES
    ${out_bindir}/configure.ac
)

add_targets_depends(src_files)
