cmake_minimum_required (VERSION 2.6) 

# project configuration
project (RBiips-src)
set (PROJECT_PKG_NAME RBiips)

# configure DESCRIPTION file
configure_file (
	DESCRIPTION.in
	${PROJECT_PKG_NAME}/DESCRIPTION
)

# configure README file
configure_file (
	README.in
	${PROJECT_PKG_NAME}/README
)

# configure R/windows/zzz.R file
configure_file (
	R/windows/zzz.R.in
	${PROJECT_PKG_NAME}/R/windows/zzz.R
)

# configure configure.ac file
configure_file (
	configure.ac.in
	${PROJECT_PKG_NAME}/configure.ac
	@ONLY
)

# Generate configuration scripts
if (UNIX)
	execute_process( COMMAND "autoconf"
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/${PROJECT_PKG_NAME})
endif (UNIX)

# copy files to binary directory
macro (copy_dir_to_binary)
	file (
		GLOB SOURCE_FILES
		${PROJECT_SOURCE_DIR}/${dir}*
	)
	file (	COPY ${SOURCE_FILES}
		DESTINATION ${PROJECT_PKG_NAME}/${dir}
		PATTERN CMakeLists.txt EXCLUDE
		PATTERN *.in EXCLUDE
		PATTERN *.win EXCLUDE
		PATTERN .svn EXCLUDE
		PATTERN .project EXCLUDE
		PATTERN .cproject EXCLUDE
	)
endmacro(copy_dir_to_binary)

copy_dir_to_binary(.)
copy_dir_to_binary(man/)
copy_dir_to_binary(R/)
copy_dir_to_binary(R/unix/)
copy_dir_to_binary(R/windows/)
copy_dir_to_binary(src/)

# add R package tools targets
add_custom_target (${PROJECT_PKG_NAME}_check
	R CMD check ${PROJECT_PKG_NAME}/${PROJECT_PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Checking ${PROJECT_PKG_NAME} R package"
	VERBATIM
)

add_custom_target (${PROJECT_PKG_NAME}_build
	R CMD build ${PROJECT_PKG_NAME}/${PROJECT_PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Building ${PROJECT_PKG_NAME} R package"
	VERBATIM
)

add_custom_target (${PROJECT_PKG_NAME}_install
	R CMD INSTALL ${PROJECT_PKG_NAME}/${PROJECT_PKG_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Installing ${PROJECT_PKG_NAME} R package"
	VERBATIM
)

