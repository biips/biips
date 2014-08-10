# Name of the R package
set (PKG_NAME RBiips)

# top directory of the sources: useful in subdirectories
set(PKG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# top directory of the clean package (not polluted by CMake Makefile)
set(PKG_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PKG_NAME})

# list of the package files to be copied in PKG_BIN_DIR
set(PKG_FILES ${PKG_FILES} cleanup configure.ac configure COPYING DESCRIPTION NAMESPACE NOTICES README)

# add subdirectories
add_subdirectory(R)
add_subdirectory(src)


# add command to copy files in PKG_FILES to PKG_BIN_DIR
# and set PKG_BIN_FILES to be the list of the output files
foreach(f ${PKG_FILES})
    add_custom_command(OUTPUT ${PKG_BIN_DIR}/${f}
        DEPENDS ${PKG_SRC_DIR}/${f}
        COMMAND ${CMAKE_COMMAND} -E copy ${PKG_SRC_DIR}/${f} ${PKG_BIN_DIR}/${f}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${f} to ${PKG_NAME} package dir"
    )
    set(PKG_BIN_FILES ${PKG_BIN_FILES} ${PKG_BIN_DIR}/${f})
endforeach()

# Generate configuration scripts
if (NOT WIN32)
    # configure configure.ac file
    configure_file (configure.ac.IN
      ${CMAKE_CURRENT_SOURCE_DIR}/configure.ac
    	@ONLY
    	NEWLINE_STYLE UNIX
    )

  # find autoconf
  find_program (AUTOCONF autoconf)
  
  if (${AUTOCONF} STREQUAL AUTOCONF-NOTFOUND)
		message (SEND_ERROR "autoconf not found")
  else ()
	    # Copy configure.ac to bindir
	    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/configure.ac
		    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/configure.ac
		    COMMAND ${CMAKE_COMMAND} -E copy configure.ac ${CMAKE_CURRENT_BINARY_DIR}
		    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		    COMMENT "Copying configure.ac to binary dir"
	    )
	    # Autoconf takes place in binary dir to avoid polluting source dir with output files
	    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/configure
		    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/configure.ac
	        COMMAND ${AUTOCONF}
		    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		    COMMENT "Autoconf generating configure file"
	    )
	    # Copy configure to sources
	    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/configure
		    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/configure
		    COMMAND ${CMAKE_COMMAND} -E copy configure ${CMAKE_CURRENT_SOURCE_DIR}
		    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		    COMMENT "Copying configure to source dir"
	    )
  endif ()
endif ()

# configure DESCRIPTION file in binary dir to avoid overwriting modifications by roxygen
configure_file (
  DESCRIPTION.IN
  ${CMAKE_CURRENT_BINARY_DIR}/DESCRIPTION
  @ONLY
)

# find R
find_package(R ${R_VERSION_REQUIRED} REQUIRED COMPONENTS Rcpp roxygen2)

# add debug flag
if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set (R_INSTALL_FLAGS -d)
endif()

# generate DESCRIPTION and NAMESPACE files in source dir
file(GLOB R_files R/*.R)
add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION ${CMAKE_CURRENT_SOURCE_DIR}/NAMESPACE
    DEPENDS ${R_files} ${CMAKE_CURRENT_BINARY_DIR}/DESCRIPTION
    COMMAND ${CMAKE_COMMAND} -E copy  ${CMAKE_CURRENT_BINARY_DIR}/DESCRIPTION .
    COMMAND rm NAMESPACE
    COMMAND ${R_EXECUTABLE} -q -e "library(roxygen2);roxygenize('.', roclets=c('collate', 'namespace'))"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Roxygenize ${PKG_NAME} DESCRIPTION and NAMESPACE"
    VERBATIM
)
	
# generate man directory documentation in binary dir
add_custom_command (OUTPUT ${PKG_BIN_DIR}/man/${PKG_NAME}-package.Rd
    DEPENDS ${PKG_BIN_FILES}
    COMMAND ${R_EXECUTABLE} -q -e "library(roxygen2);roxygenize('.', roclets='rd')"
    WORKING_DIRECTORY ${PKG_BIN_DIR}
    COMMENT "Roxygenize ${PKG_NAME} man dir"
    VERBATIM
)

# add pkg_dir target to set up the clean package directory
add_custom_target(${PKG_NAME}_pkg_dir
    DEPENDS ${PKG_BIN_DIR}/man/${PKG_NAME}-package.Rd
    COMMENT "Setting up ${PKG_NAME} package dir"
)
    
# add R CMD check target
add_custom_target (${PKG_NAME}_check  
	DEPENDS ${PKG_NAME}_pkg_dir
	COMMAND ${R_EXECUTABLE} CMD check ${PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Checking ${PKG_NAME} package"
	VERBATIM
)

# add R CMD Rd2pdf target
add_custom_target (${PKG_NAME}_Rd2pdf
	DEPENDS ${PKG_BIN_DIR}/man/${PKG_NAME}-package.Rd
	COMMAND ${R_EXECUTABLE} CMD Rd2pdf --no-preview --force ${PKG_BIN_DIR}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Generate ${PKG_NAME} documentation PDF output from the Rd sources"
	VERBATIM
)    


if (WIN32)
# TODO: customize arch #######################################
    # add R CMD INSTALL --build target
    add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${PKG_NAME}.zip
        DEPENDS ${PKG_NAME}_pkg_dir
        COMMAND ${R_EXECUTABLE} --arch ${BITS} CMD INSTALL --preclean --build --no-multiarch ${R_INSTALL_FLAGS} ${PKG_BIN_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building ${PKG_NAME} binary package"
        VERBATIM
    )
    add_custom_target (${PKG_NAME}_INSTALL_build
        DEPENDS ${CMAKE_BINARY_DIR}/${PKG_NAME}.zip
    )
  
elseif (APPLE)

    # add R CMD INSTALL --build target
    add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${PKG_NAME}.tgz
        DEPENDS ${PKG_NAME}_pkg_dir
        COMMAND ${R_EXECUTABLE} CMD INSTALL --build ${R_INSTALL_FLAGS} ${PKG_BIN_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building ${PKG_NAME} binary package"
        VERBATIM
    )
    add_custom_target (${PKG_NAME}_INSTALL_build
        DEPENDS ${CMAKE_BINARY_DIR}/${PKG_NAME}.tgz
    )
  
elseif (UNIX AND NOT APPLE)

    # add R CMD build target
    add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${PKG_NAME}.tar.gz
        DEPENDS ${PKG_NAME}_pkg_dir
        COMMAND ${R_EXECUTABLE} CMD build ${PKG_BIN_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building ${PKG_NAME} source package"
        VERBATIM
    )
    add_custom_target (${PKG_NAME}_build
        DEPENDS ${CMAKE_BINARY_DIR}/${PKG_NAME}.tar.gz
    )
  
endif()
      
# add R CMD INSTALL target
add_custom_target (${PKG_NAME}_INSTALL
    DEPENDS ${PKG_NAME}_pkg_dir
    COMMAND ${R_EXECUTABLE} CMD INSTALL --preclean ${R_INSTALL_FLAGS} ${PKG_BIN_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Installing ${PKG_NAME} package"
    VERBATIM
)

