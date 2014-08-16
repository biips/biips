#define sources
file(GLOB Mbiips_SRC 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
)
file(GLOB Mbiips_HDRS 
${CMAKE_CURRENT_SOURCE_DIR}/*.h 
${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)
set(MATBIIPS_SRC ${Mbiips_SRC} ${Mbiips_HDRS})

set(BIIPS_LIBS BiipsBase BiipsCompiler BiipsCore)
if (MATLAB OR OCTAVE) 
	
 	if(WIN32)

		# set incude flags
		foreach(d ${Boost_INCLUDE_DIRS} ${Core_INCLUDE_DIRS} ${Base_INCLUDE_DIRS} ${Compiler_INCLUDE_DIRS})
			set(MATBIIPS_INCLUDE_FLAGS ${MATBIIPS_INCLUDE_FLAGS} -I"${d}")
		endforeach()

		# biips src files targets
		foreach(f ${Core_SRC} ${Base_SRC} ${Compiler_SRC} ${Mbiips_SRC})
			# file extension
			get_filename_component(fext ${f} EXT)
			# file name
			get_filename_component(fname ${f} NAME_WE)

			# check cpp source file
			if(fext STREQUAL .c OR fext STREQUAL .cpp OR fext STREQUAL .cc)
				# target command
				add_custom_command(OUTPUT ${fname}.${MEX_OBJ_EXT}
					COMMAND ${MEX_COMMAND} ${MEX_FLAGS_${CMAKE_BUILD_TYPE}} ${MEX_COMPILE_OPT} ${MEX_OUTPUT_OPT} ${fname}.${MEX_OBJ_EXT} ${MATBIIPS_INCLUDE_FLAGS} ${f}
					COMMENT "Building ${fname}.${MEX_OBJ_EXT}"
					IMPLICIT_DEPENDS CXX ${f})
				set(Mbiips_OBJ ${Mbiips_OBJ} ${fname}.${MEX_OBJ_EXT})
			endif()
		endforeach()

		file(TO_CMAKE_PATH ${CMAKE_CURRENT_BINARY_DIR}/matbiips.${MEX_EXT} MATBIIPS_TARGET_LOC)

		# matbiips mexfile target
		add_custom_command(OUTPUT ${MATBIIPS_TARGET_LOC}
			DEPENDS ${Mbiips_OBJ}
			COMMAND ${MEX_COMMAND} ${MEX_MEX_OPT} ${MEX_FLAGS_${CMAKE_BUILD_TYPE}} ${MEX_OUTPUT_OPT} matbiips.${MEX_EXT} ${MATBIIPS_INCLUDE_FLAGS} ${Mbiips_OBJ}
			COMMENT "Linking mexfile matbiips.${MEX_EXT}"
		)
		add_custom_target(matbiips
			DEPENDS ${MATBIIPS_TARGET_LOC}
		)


 	elseif (UNIX)
 	    include_directories(${Core_INCLUDE_DIRS}
 			${Base_INCLUDE_DIRS}
 			${Compiler_INCLUDE_DIRS}
 			${MATLAB_INCLUDE_DIR}
 			${Boost_INCLUDE_DIRS}
 		)
 		
 		if (COMPILER_NAME STREQUAL gcc AND BIIPS_DEBUG)
 		    set(MATLAB_COMPILE_FLAGS "${MATLAB_COMPILE_FLAGS} -fpermissive")
 		endif()
 
 		add_library(matbiips SHARED EXCLUDE_FROM_ALL ${MATBIIPS_SRC})
 		target_link_libraries(matbiips ${BIIPS_LIBS} ${MATLAB_LIBRARIES})
 		set_target_properties(matbiips PROPERTIES
 			COMPILE_FLAGS "${MATLAB_COMPILE_FLAGS}"
 			LINK_FLAGS "${MATLAB_LINK_FLAGS}"
 			PREFIX ""
 			SUFFIX .${MEX_EXT}
 		)
 	
     	get_target_property(MATBIIPS_TARGET_LOC matbiips LOCATION)
 	endif()
else()
    if (FIND_OCTAVE)
        message (FATAL_ERROR "OCTAVE not found on your computer.\n"
        			 "Try setting OCTAVE_ROOT to the root path of 
        your OCTAVE installation. eg: c:\\Octave\\3.6.4\n"
                     "You can deactivate MATBIIPS compilation by 
        setting BUILD_MATBIIPS=OFF")
    else()
        message (FATAL_ERROR "MATLAB not found on your computer.\n"
        			 "Try setting MATLAB_ROOT to the root path of 
        your MATLAB installation. eg: c:\\Program Files\\MATLAB\\R2012a\n"
                     "You can deactivate MATBIIPS compilation by 
        setting BUILD_MATBIIPS=OFF")
    endif()
endif()


# install mexfile in the build private directory
add_custom_command(TARGET matbiips
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${MATBIIPS_TARGET_LOC} "${CMAKE_BINARY_DIR}/matbiips/private/"
    COMMENT "Copying mexfile in build private directory"
    VERBATIM
)
# install mexfile in the build tests directory
add_custom_command(TARGET matbiips
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${MATBIIPS_TARGET_LOC} "${CMAKE_BINARY_DIR}/matbiips/tests/"
    COMMENT "Copying mexfile in build tests directory"
    VERBATIM
)

#if (NOT BIIPS_DEBUG)
	# copy mexfile in the src private directory
	add_custom_command(TARGET matbiips
	    POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy ${MATBIIPS_TARGET_LOC} "${CMAKE_SOURCE_DIR}/matbiips/private/"
	    COMMENT "Copying mexfile in source private directory"
	    VERBATIM
	)
#endif()
