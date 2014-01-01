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
#  \file     FindR.cmake
#  \brief    Try to find R
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#

#  COPY: This file is derived from OpenTURNS sources (C) Copyright 2005-2012 EDF-EADS-Phimeca
#  - Try to find R
#  Once done this will define
#
#  R_FOUND - System has R
#  R_EXECUTABLE - The R interpreter


if ( R_EXECUTABLE)
   # in cache already
   set( R_FIND_QUIETLY TRUE )
endif ( R_EXECUTABLE)

set (CMAKE_FIND_APPBUNDLE LAST)
find_program ( R_EXECUTABLE
    NAMES R R.exe
    DOC "Path to the R command interpreter"
)

set ( R_PACKAGES )
if ( R_EXECUTABLE )
    if (WIN32)
        set (R_FLAGS --vanilla --slave --ess)
    else ()
        set (R_FLAGS --vanilla --slave --no-readline)
    endif()
    
    # define R_VERSION
    execute_process( COMMAND ${R_EXECUTABLE} ${R_FLAGS}
        -e "cat(paste(R.version$major, R.version$minor, sep='.'))"
        OUTPUT_VARIABLE R_VERSION_STRING
    )
	STRING(REGEX MATCH "[0-9]+\\.[0-9]+(\\.[0-9])?" R_VERSION "${R_VERSION_STRING}")
    foreach ( _component ${R_FIND_COMPONENTS} )
        if ( NOT R_${_component}_FOUND )
            execute_process ( COMMAND ${R_EXECUTABLE} ${R_FLAGS}
                -e "library(${_component})"
                RESULT_VARIABLE _res
                OUTPUT_VARIABLE _trashout
                ERROR_VARIABLE  _trasherr
            )
            if ( NOT _res )
                message ( STATUS "Looking for R package ${_component} - found" )
                set ( R_${_component}_FOUND 1 CACHE INTERNAL "True if R package ${_component} is here" )
            else ( NOT _res )
                message (STATUS "Looking for R package ${_component} - not found" )
                set ( R_${_component}_FOUND 0 CACHE INTERNAL "True if R package ${_component} is here" )
            endif ( NOT _res )
            list ( APPEND R_PACKAGES R_${_component}_FOUND )
        endif ( NOT R_${_component}_FOUND )
    endforeach ( _component )
    # chomp the string
    #string(REGEX REPLACE "[ \t\n]+" \; R_INCLUDE "${R_INCLUDE}")
    #message(STATUS "INCLUDE for R =" ${R_INCLUDE})
endif ( R_EXECUTABLE )

include ( FindPackageHandleStandardArgs )

# handle the QUIETLY and REQUIRED arguments and set R_FOUND to TRUE if 
# all listed variables are TRUE
find_package_handle_standard_args (R
    REQUIRED_VARS R_EXECUTABLE ${R_PACKAGES}
    VERSION_VAR R_VERSION
)

# find Rscript to extract PKG_CXXFLAGS
get_filename_component(PATH_TO_R ${R_EXECUTABLE} PATH)
find_program (R_SCRIPT NAMES Rscript PATHS ${PATH_TO_R})
if (R_SCRIPT)
  message(STATUS "found R_SCRIPT")
  execute_process(COMMAND ${R_SCRIPT} -e "Rcpp:::CxxFlags()" OUTPUT_VARIABLE RCPP_CXXFLAGS)
  execute_process(COMMAND ${R_SCRIPT} -e "Rcpp:::LdFlags()" OUTPUT_VARIABLE RCPP_LIBS)
  if (WIN32)
    execute_process(COMMAND ${R_SCRIPT} -e "cat(R.home(\\\"include\\\"))" OUTPUT_VARIABLE R_INCLUDE)
  else ()
    execute_process(COMMAND ${R_SCRIPT} -e "cat(R.home(\"include\"))" OUTPUT_VARIABLE R_INCLUDE)
  endif ()
  message(STATUS "Rcpp_FLAGS = " ${RCPP_CXXFLAGS})
  message(STATUS "R_INCLUDE = " ${R_INCLUDE})
  message(STATUS "Rcpp_LIBS = " ${RCPP_LIBS})
endif(R_SCRIPT)

mark_as_advanced (R_EXECUTABLE ${R_PACKAGES})
