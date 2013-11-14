set_output_bindir(out_bindir)

# configure Makevars.win file
configure_file (
	Makevars.win.in
	${out_bindir}/Makevars.win
	@ONLY
)

# copy files to binary directory
file(GLOB cpp_files *.cpp)
file(GLOB h_files *.h)
#add R CMD shlib target a la mano


include_directories(${Core_INCLUDE_DIRS}
	${Base_INCLUDE_DIRS}
	${Compiler_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)
add_definitions(${RCPP_FLAGS})
add_library(rbiips_shlib SHARED ${rcpp_files})
target_link_libraries(rbiips_shlib ${BIIPS_LIBS} )
set_target_properties(rbiips_shlib PROPERTIES
    COMPILE_FLAGS "${RCPP_CXXFLAGS}"
    LINK_FLAGS "${RCPP_LIBS}"
	PREFIX ""
	SUFFIX .so
)


set(src_files ${CMAKE_CURRENT_SOURCE_DIR}/Makevars.in ${cpp_files} ${h_files})
file(COPY ${src_files} DESTINATION ${out_bindir})


# add dependencies to source files 
add_targets_depends(src_files)
