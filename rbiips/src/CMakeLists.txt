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



#include_directories(${Core_INCLUDE_DIRS}
#	${Base_INCLUDE_DIRS}
#	${Compiler_INCLUDE_DIRS}
#	${MATLAB_INCLUDE_DIR}
#	${Boost_INCLUDE_DIRS}
#)

#add_library(inter_biips SHARED EXCLUDE_FROM_ALL ${MBIIPS_SRC})
#target_link_libraries(inter_biips ${BIIPS_LIBS} ${MATLAB_LIBRARIES})
#set_target_properties(inter_biips PROPERTIES
#	COMPILE_FLAGS "${MATLAB_COMPILE_FLAGS}"
#	LINK_FLAGS "${MATLAB_LINK_FLAGS}"
#	PREFIX ""
#	SUFFIX .${MEX_EXT}
#)


set(src_files ${CMAKE_CURRENT_SOURCE_DIR}/Makevars.in ${cpp_files} ${h_files})
file(COPY ${src_files} DESTINATION ${out_bindir})


# add dependencies to source files 
add_targets_depends(src_files)
