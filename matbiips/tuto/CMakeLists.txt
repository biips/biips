#define sources
file(GLOB tuto_files *.m)
file(GLOB bugfiles *.bug)
set(a_copier ${bugfiles} ${tuto_files})
copy_files_to_bindir(a_copier ${CMAKE_CURRENT_BINARY_DIR})
# install mexfile in the build directory
foreach(_tf ${tuto_files})
   if (MATLAB)
       set (opt_exec " -nodesktop -nosplash -r ")
   else(MATLAB)
       set (opt_exec " --eval ") 
   endif(MATLAB)
   get_filename_component(_namewe ${_tf} NAME_WE)
   set(MAT_COMS "addpath('../tests/');publish('${_namewe}.m');exit")
   set(OUT_TUTO "${CMAKE_CURRENT_BINARY_DIR}/html/${_namewe}.html")
      add_custom_command(
      	OUTPUT ${OUT_TUTO}
        COMMENT "making tutorial for :" ${_tf}
	COMMAND ${MATLAB_COMMAND} ${opt_exec} "\"${MAT_COMS}\""
        VERBATIM)
set(OUT_LST ${OUT_LST} ${OUT_TUTO})
endforeach()

#define target
add_custom_target(matbiips_tuto
	DEPENDS  ${OUT_LST} matbiips
)
