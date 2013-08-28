project(RBiips)

macro(set_output_bindir var)
    file(RELATIVE_PATH CURRENT_REL_DIR ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/${PROJECT_NAME}/${CURRENT_REL_DIR}" ${var})
endmacro(set_output_bindir)

set_output_bindir(out_bindir)

# source files list generation
file (GLOB SOURCE_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/src/*.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/*.cpp
)

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
    
    
    # add clean object files command
    add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cleaned_objects.tmp
        DEPENDS ${BIIPS_LIBS} ${SOURCE_FILES}
        COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_NAME}/src/*.o
        COMMAND ${CMAKE_COMMAND} -E touch cleaned_objects.tmp
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Cleaning ${PROJECT_NAME} object files"
    )
    if (NOT WIN32)
        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cleaned_objects.tmp
            DEPENDS ${out_bindir}/configure
            APPEND
        )
    endif()
    
    
    
    if (WIN32)
        # add R CMD INSTALL --build target
        add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_${Biips_VERSION}.zip
    	    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cleaned_objects.tmp
        	COMMAND ${R_EXECUTABLE} CMD INSTALL --build ${R_INSTALL_FLAGS} ${pkg_dir}
        	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        	COMMENT "Building ${PROJECT_NAME} R binary package"
        	VERBATIM
        )
        add_custom_target (${PROJECT_NAME}_INSTALL_build ALL 
    	    DEPENDS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_${Biips_VERSION}.zip
        )
    elseif (APPLE)
        # add R CMD INSTALL --build target
        add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_${Biips_VERSION}.tar.gz
    	    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cleaned_objects.tmp
        	COMMAND ${R_EXECUTABLE} CMD INSTALL --build ${R_INSTALL_FLAGS} ${pkg_dir}
        	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        	COMMENT "Building ${PROJECT_NAME} R binary package"
        	VERBATIM
        )
     add_custom_target (${PROJECT_NAME}_INSTALL_build ALL
    	    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_${Biips_VERSION}.tar.gz
        )
    elseif (UNIX)
        # add R CMD build target
        add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_${Biips_VERSION}.tar.gz
    	    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cleaned_objects.tmp
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
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cleaned_objects.tmp
            BiipsCompiler BiipsBase
        	COMMAND ${R_EXECUTABLE} CMD INSTALL ${R_INSTALL_FLAGS} ${pkg_dir}
        	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        	COMMENT "Installing ${PROJECT_NAME} R package"
        	VERBATIM
        )
        # add R CMD check target
        add_custom_target (${PROJECT_NAME}_check  
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cleaned_objects.tmp BiipsCompiler ${PROJECT_NAME}_INSTALL 
        	COMMAND ${R_EXECUTABLE} CMD check ${PROJECT_NAME}
        	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        	COMMENT "Checking ${PROJECT_NAME} R package"
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
add_subdirectory(tests)


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
file(COPY ${src_files} DESTINATION ${out_bindir})


# add dependencies to source files
set (src_files ${src_files}
    ${out_bindir}/DESCRIPTION
    ${out_bindir}/README
    ${out_bindir}/NOTICES
    ${out_bindir}/configure.ac
)

add_targets_depends(src_files)
