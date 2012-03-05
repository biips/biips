cmake_minimum_required (VERSION 2.8) 

# project configuration
project (RBiips)
set (PROJECT_PKG_NAME ${PROJECT_NAME})
string(REGEX MATCH [a-zA-Z0-9_.-]*$ PROJECT_REL_DIR ${PROJECT_SOURCE_DIR})

# configure DESCRIPTION file
configure_file (
	DESCRIPTION.IN
	${PROJECT_PKG_NAME}/DESCRIPTION
)

# configure README file
configure_file (
	README.IN
	${PROJECT_PKG_NAME}/README
)

# configure R/windows/zzz.R file
configure_file (
	R/windows/zzz.R.IN
	${PROJECT_PKG_NAME}/R/windows/zzz.R
)

# configure configure.ac file
configure_file (
	configure.ac.IN
	${PROJECT_PKG_NAME}/configure.ac
	@ONLY
)

# Generate configuration scripts
if (UNIX)
	find_program (AUTOCONF autoconf)
	if (AUTOCONF)
		execute_process( COMMAND "autoconf"
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/${PROJECT_PKG_NAME})
	else (AUTOCONF)
		message (SEND_ERROR "autoconf not found")
	endif (AUTOCONF)
endif (UNIX)

# copy files to binary directory
macro (copy_dir_to_binary dir)
	file (
		GLOB SOURCE_FILES
		${PROJECT_SOURCE_DIR}/${dir}/*
	)
	file (	COPY ${SOURCE_FILES}
		DESTINATION ${PROJECT_PKG_NAME}/${dir}/
		PATTERN CMakeLists.txt EXCLUDE
		PATTERN *.IN EXCLUDE
		PATTERN .svn EXCLUDE
		PATTERN .project EXCLUDE
		PATTERN .cproject EXCLUDE
	)
endmacro(copy_dir_to_binary)

copy_dir_to_binary(.)
copy_dir_to_binary(man)
copy_dir_to_binary(R)
copy_dir_to_binary(R/unix)
copy_dir_to_binary(R/windows)
copy_dir_to_binary(src)

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
	COMMENT "Building ${PROJECT_PKG_NAME} R package"
	VERBATIM
)

add_custom_target (${PROJECT_PKG_NAME}_install
	R CMD INSTALL ${PROJECT_REL_DIR}/${PROJECT_PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Installing ${PROJECT_PKG_NAME} R package"
	VERBATIM
)

