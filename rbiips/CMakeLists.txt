project(RBiips)

# define a directory for the configured package sources
# not polluted by cmake
macro(set_output_bindir var)
    file(RELATIVE_PATH CURRENT_REL_DIR ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/${PROJECT_NAME}/${CURRENT_REL_DIR}" ${var})
endmacro(set_output_bindir)

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/${PROJECT_NAME}" out_bindir)

if (NOT WIN32)
# configure configure.ac file
configure_file (configure.ac.IN
  ${CMAKE_CURRENT_SOURCE_DIR}/configure.ac
	@ONLY
	NEWLINE_STYLE UNIX
)
endif()

# copy files to binary directory
set (src_files cleanup configure.ac COPYING README NOTICES)
file(COPY ${src_files} DESTINATION ${out_bindir})

# configure DESCRIPTION file
configure_file (
  DESCRIPTION.IN
  ${CMAKE_CURRENT_BINARY_DIR}/DESCRIPTION
  @ONLY
)

# Generate configuration scripts
if (NOT WIN32)
  find_program (AUTOCONF autoconf)
  if (${AUTOCONF} STREQUAL AUTOCONF-NOTFOUND)
		message (SEND_ERROR "autoconf not found")
	else ()
	    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/configure
		    DEPENDS configure.ac
	        COMMAND ${AUTOCONF}
	        COMMAND ${CMAKE_COMMAND} -E copy configure ${out_bindir}
		    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		    COMMENT "Autoconf generating configure file"
	    )
	    add_custom_command(OUTPUT ${out_bindir}/configure
		    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/configure
	        COMMAND ${CMAKE_COMMAND} -E copy configure ${out_bindir}
		    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		    COMMENT "Copying configure file to binary directory"
	    )
	endif ()
endif ()



# find R
find_package(R ${R_VERSION_REQUIRED} REQUIRED COMPONENTS Rcpp roxygen2)

if (R_FOUND)
  set (pkg_dir ${out_bindir})
  
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      set (R_INSTALL_FLAGS -d)
  endif()

  add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION ${CMAKE_CURRENT_SOURCE_DIR}/NAMESPACE
    COMMAND ${CMAKE_COMMAND} -E copy  ${CMAKE_CURRENT_BINARY_DIR}/DESCRIPTION ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${R_EXECUTABLE} -q -e "library(roxygen2);roxygenize('.', roclets=c('collate', 'namespace'))"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Roxygenize ${PROJECT_NAME} DESCRIPTION and NAMESPACE"
		VERBATIM
    )
  add_custom_command (OUTPUT ${pkg_dir}/DESCRIPTION ${pkg_dir}/NAMESPACE
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION ${CMAKE_CURRENT_SOURCE_DIR}/NAMESPACE
    COMMAND ${CMAKE_COMMAND} -E copy DESCRIPTION ${pkg_dir}
    COMMAND ${CMAKE_COMMAND} -E copy NAMESPACE ${pkg_dir}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Copying ${PROJECT_NAME} DESCRIPTION and NAMESPACE to pkg_dir"
		VERBATIM
    )
	
  add_custom_command (OUTPUT ${pkg_dir}/man/${PROJECT_NAME}-package.Rd
    COMMAND ${R_EXECUTABLE} -q -e "library(roxygen2);roxygenize('.', roclets='rd')"
		WORKING_DIRECTORY ${pkg_dir}
		COMMENT "Roxygenize ${PROJECT_NAME} man directory"
		VERBATIM
	)

  # add pkg_dir target
  if (WIN32)
      add_custom_target (${PROJECT_NAME}_pkg_dir
        DEPENDS ${pkg_dir}/DESCRIPTION ${pkg_dir}/NAMESPACE ${pkg_dir}/man/${PROJECT_NAME}-package.Rd
        COMMENT "Setting up ${PROJECT_NAME} package directory"
	    )
  else()
      add_custom_target (${PROJECT_NAME}_pkg_dir
        DEPENDS ${pkg_dir}/DESCRIPTION ${pkg_dir}/NAMESPACE ${pkg_dir}/man/${PROJECT_NAME}-package.Rd ${out_bindir}/configure
        COMMENT "Setting up ${PROJECT_NAME} package directory"
	    )
  endif()

    
	# add R CMD check target
	add_custom_target (${PROJECT_NAME}_check  
		DEPENDS ${PROJECT_NAME}_pkg_dir
		COMMAND ${R_EXECUTABLE} CMD check ${PROJECT_NAME}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Checking ${PROJECT_NAME} R package"
		VERBATIM
	)

  # add R CMD Rd2pdf target
	add_custom_target (${PROJECT_NAME}_Rd2pdf
		DEPENDS ${pkg_dir}/man/${PROJECT_NAME}-package.Rd
		COMMAND ${R_EXECUTABLE} CMD Rd2pdf --no-preview --force ${pkg_dir}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Generate ${PROJECT_NAME} documentation PDF output from the Rd sources"
		VERBATIM
	)    
    
  if (WIN32)
      # add R CMD INSTALL --build target
      add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.zip
  	    DEPENDS ${PROJECT_NAME}_pkg_dir
      	COMMAND ${R_EXECUTABLE} --arch 64 CMD INSTALL --build --no-multiarch ${R_INSTALL_FLAGS} ${pkg_dir}
      	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      	COMMENT "Building ${PROJECT_NAME} R binary package"
      	VERBATIM
      )
      add_custom_target (${PROJECT_NAME}_INSTALL_build
        DEPENDS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.zip
      )
  elseif (APPLE)
      # add R CMD INSTALL --build target
      add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.tgz
  	    DEPENDS ${PROJECT_NAME}_pkg_dir
      	COMMAND ${R_EXECUTABLE} CMD INSTALL --build ${R_INSTALL_FLAGS} ${pkg_dir}
      	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      	COMMENT "Building ${PROJECT_NAME} R binary package"
      	VERBATIM
      )
      add_custom_target (${PROJECT_NAME}_INSTALL_build
        DEPENDS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.tgz
      )
  elseif (UNIX)
      # add R CMD build target
      add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.tar.gz
  	    DEPENDS ${PROJECT_NAME}_pkg_dir
      	COMMAND ${R_EXECUTABLE} CMD build ${pkg_dir}
      	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      	COMMENT "Building ${PROJECT_NAME} R source package"
      	VERBATIM
      )
      add_custom_target (${PROJECT_NAME}_build
        DEPENDS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.tar.gz
      )
  endif()
      
  # add R CMD INSTALL target
  add_custom_target (${PROJECT_NAME}_INSTALL
    DEPENDS ${PROJECT_NAME}_pkg_dir
    COMMAND ${R_EXECUTABLE} CMD INSTALL --preclean ${R_INSTALL_FLAGS} ${pkg_dir}
  	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  	COMMENT "Installing ${PROJECT_NAME} R package"
  	VERBATIM
  )
else ()
    message (SEND_ERROR "R not found")
endif ()


macro(add_sources bindir src_files)
    foreach (f ${${src_files}})
        set(SRC_FILES ${SRC_FILES} ${bindir}/${f} PARENT_SCOPE)
    endforeach()
endmacro()

add_sources(${out_bindir} src_files)

# add subdirectories
add_subdirectory(R)
add_subdirectory(src)


# add dependencies to source files
if (R_FOUND)
  add_custom_command(OUTPUT ${pkg_dir}/NAMESPACE
      DEPENDS ${SRC_FILES}
      APPEND
  )
endif ()
