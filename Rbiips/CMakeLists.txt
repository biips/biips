# Name of the R package
set (PKG_NAME Rbiips)

# top directory of the sources: useful in subdirectories
set(PKG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# top directory of the clean package (not polluted by CMake Makefile)
set(PKG_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PKG_NAME})

# list of the package files to be copied in PKG_BIN_DIR
set(PKG_FILES ${PKG_FILES} cleanup COPYING DESCRIPTION NAMESPACE NOTICES README)

# add subdirectories
add_subdirectory(R)
add_subdirectory(src)
add_subdirectory(inst)
add_subdirectory(vignettes)


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


# add command to copy files in Core_INCLUDE_FILES, Base_INCLUDE_FILES and Compiler_INCLUDE_FILES to PKG_BIN_DIR/src
# and add the output files to the list PKG_BIN_FILES
foreach(f ${Core_INCLUDE_FILES} ${Base_INCLUDE_FILES} ${Compiler_INCLUDE_FILES})
    add_custom_command(OUTPUT ${PKG_BIN_DIR}/src/${f}
        DEPENDS ${CMAKE_BINARY_DIR}/${f}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${f} ${PKG_BIN_DIR}/src/${f}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${f} to ${PKG_NAME}/src package dir"
    )
    set(PKG_BIN_FILES ${PKG_BIN_FILES} ${PKG_BIN_DIR}/src/${f})
endforeach()

# add command to copy files in Core_SOURCE_FILES, Base_SOURCE_FILES and Compiler_SOURCE_FILES to PKG_BIN_DIR/src
# and add the output files to the list PKG_BIN_FILES
foreach(f ${Core_SOURCE_FILES} ${Base_SOURCE_FILES} ${Compiler_SOURCE_FILES})
#    get_filename_component(fn ${f} NAME)
    add_custom_command(OUTPUT ${PKG_BIN_DIR}/src/${f}
        DEPENDS ${CMAKE_BINARY_DIR}/${f}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${f} ${PKG_BIN_DIR}/src/${f}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${f} to ${PKG_NAME}/src package dir"
    )
    set(PKG_BIN_FILES ${PKG_BIN_FILES} ${PKG_BIN_DIR}/src/${f})
endforeach()

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
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/DESCRIPTION ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove NAMESPACE
    COMMAND ${R_EXECUTABLE} ${R_FLAGS} -q -e "library(roxygen2);roxygenize('.', roclets=c('collate', 'namespace'))"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Roxygenize ${PKG_NAME} DESCRIPTION and NAMESPACE"
    VERBATIM
)

# generate man directory documentation in binary dir
add_custom_command (OUTPUT ${PKG_BIN_DIR}/man/${PKG_NAME}-package.Rd
    DEPENDS ${PKG_BIN_FILES}
    COMMAND ${R_EXECUTABLE} ${R_FLAGS} -q -e "library(roxygen2);roxygenize('.', roclets='rd')"
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
  	COMMAND ${R_EXECUTABLE} ${R_FLAGS} CMD check ${PKG_NAME}
  	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  	COMMENT "Checking ${PKG_NAME} package"
  	VERBATIM
)

# add R CMD check target
add_custom_target (${PKG_NAME}_check_cran
  	DEPENDS ${PKG_NAME}_pkg_dir
  	COMMAND ${R_EXECUTABLE} ${R_FLAGS} CMD check --as-cran ${PKG_NAME}
  	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  	COMMENT "Checking ${PKG_NAME} package"
  	VERBATIM
)



if (WIN32)

    if ((${R_ARCH} STREQUAL "x64") AND (BITS EQUAL 32))
        message(FATAL_ERROR "Can not build Rbiips for 32bit. R architecture is 64bit. You can use cmake option -DBUILD_RBIIPS=OFF to deactivate Rbiips build")
    elseif ((${R_ARCH} STREQUAL "i386") AND (BITS EQUAL 64))
        message(FATAL_ERROR "Can not build Rbiips for 64bit. R architecture is 32bit. You can use cmake option -DBUILD_RBIIPS=OFF to deactivate Rbiips build")
    endif ()

    # add R CMD INSTALL --build target
    add_custom_target (${PKG_NAME}_build_bin
        DEPENDS ${PKG_NAME}_pkg_dir
        COMMAND ${R_EXECUTABLE} ${R_FLAGS} CMD INSTALL --preclean --build --no-multiarch ${R_INSTALL_FLAGS} ${PKG_BIN_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building ${PKG_NAME} binary package"
        VERBATIM
    )

elseif (UNIX)

    # add R CMD INSTALL --build target
    add_custom_target (${PKG_NAME}_build_bin
        DEPENDS ${PKG_NAME}_pkg_dir
        COMMAND ${R_EXECUTABLE} ${R_FLAGS} CMD INSTALL --build ${R_INSTALL_FLAGS} ${PKG_BIN_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building ${PKG_NAME} binary package"
        VERBATIM
    )

endif()

# add R CMD build target
add_custom_target (${PKG_NAME}_build_src
    DEPENDS ${PKG_NAME}_pkg_dir
    COMMAND ${R_EXECUTABLE} ${R_FLAGS} CMD build ${PKG_BIN_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Building ${PKG_NAME} source package"
    VERBATIM
)
    
# add R CMD INSTALL target
add_custom_target (${PKG_NAME}_install
    DEPENDS ${PKG_NAME}_pkg_dir
    COMMAND ${R_EXECUTABLE} ${R_FLAGS} CMD INSTALL --preclean ${R_INSTALL_FLAGS} ${PKG_BIN_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Installing ${PKG_NAME} package"
    VERBATIM
)

 
# add extract_src package target
add_custom_target (${PKG_NAME}_extract_src
    DEPENDS ${PKG_NAME}_build_src
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PKG_NAME}_src
    COMMAND ${CMAKE_COMMAND} -E chdir ${PKG_NAME}_src tar xvfz ../${PKG_NAME}_${Biips_VERSION}.tar.gz
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Extracting ${PKG_NAME} package"
    VERBATIM
)

# add R CMD Rd2pdf target
add_custom_target (${PKG_NAME}_rd2pdf
    DEPENDS ${PKG_NAME}_extract_src
    COMMAND ${R_EXECUTABLE} ${R_FLAGS} CMD Rd2pdf --no-preview --force ${PKG_NAME}_src/${PKG_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  	COMMENT "Generate ${PKG_NAME} documentation PDF output from the Rd sources"
  	VERBATIM
)

# add build_site target
add_custom_target (${PKG_NAME}_build_site
    DEPENDS ${PKG_NAME}_extract_src
    COMMAND ${R_EXECUTABLE} --vanilla -e "staticdocs::build_site('.', tracking_id = 'UA-31640193-1')"
    COMMENT "Build ${PKG_NAME} documentation site"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${PKG_NAME}_src/${PKG_NAME}
  	VERBATIM
)
