#define sources
file(GLOB Mbiips_SRC 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
)
file(GLOB Mbiips_HDRS 
${CMAKE_CURRENT_SOURCE_DIR}/*.h 
${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)
set(MATBIIPS_SRC ${Mbiips_SRC} ${Mbiips_HDRS})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fpermissive")

set(BIIPS_LIBS BiipsBase BiipsCompiler BiipsCore)
if (MATLAB OR OCTAVE) 
	
	if(WIN32)

		# set incude flags
		foreach(d ${Boost_INCLUDE_DIRS} ${Core_INCLUDE_DIRS} ${Base_INCLUDE_DIRS} ${Compiler_INCLUDE_DIRS})
			set(MATBIIPS_INCLUDE_FLAGS ${MATBIIPS_INCLUDE_FLAGS} -I"${d}")
		endforeach()

		# mex compile flags
		set(MATBIIPS_COMPILE_FLAGS -DWIN32)

		if (CMAKE_BUILD_TYPE STREQUAL Debug)
			set(MATBIIPS_COMPILE_FLAGS ${MATBIIPS_COMPILE_FLAGS} )
		elseif (CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
			set(MATBIIPS_COMPILE_FLAGS ${MATBIIPS_COMPILE_FLAGS} -g -O)
		endif()


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
					COMMAND ${MEX_COMMAND} ${MEX_FLAGS} -c ${MEX_OUTPUT_OPT} ${fname}.${MEX_OBJ_EXT} ${MATBIIPS_COMPILE_FLAGS} ${MATBIIPS_INCLUDE_FLAGS} ${f}
					COMMENT "Building ${fname}.${MEX_OBJ_EXT}")
				set(Mbiips_OBJ ${Mbiips_OBJ} ${fname}.${MEX_OBJ_EXT})
			endif()
		endforeach()

		file(TO_CMAKE_PATH ${CMAKE_CURRENT_BINARY_DIR}/matbiips.${MEX_EXT} MATBIIPS_TARGET_LOC)

		# matbiips mexfile target
		if (OCTAVE)
			set(MEX_FLAGS ${MEX_FLAGS} --mex)
		endif()
		add_custom_target(matbiips
			DEPENDS ${Mbiips_OBJ}
			COMMAND ${MEX_COMMAND} ${MEX_FLAGS} ${MEX_OUTPUT_OPT} matbiips.${MEX_EXT} ${MATBIIPS_COMPILE_FLAGS} ${MATBIIPS_INCLUDE_FLAGS} ${Mbiips_OBJ}
			COMMENT "Linking mexfile matbiips.${MEX_EXT}"
		)


	else()
	    include_directories(${Core_INCLUDE_DIRS}
			${Base_INCLUDE_DIRS}
			${Compiler_INCLUDE_DIRS}
			${MATLAB_INCLUDE_DIR}
			${Boost_INCLUDE_DIRS}
		)

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
    message (FATAL_ERROR "MATLAB not found on your computer.\n"
    			 "Try setting MATLAB_ROOT to the root path of 
    your MATLAB installation. eg: c:\\Program Files\\MATLAB\\R2012a\n"
    			 "Octave mkoct is not yet supported on 
    Windows.\n" 
                             "You should deactivate MATBIIPS compilation by 
    setting BUILD_MATBIIPS=OFF")
endif()


# install mexfile in the build matlab directory
add_custom_command(TARGET matbiips
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${MATBIIPS_TARGET_LOC} "${CMAKE_BINARY_DIR}/matbiips/matlab/"
    COMMENT "Copying mexfile in build matlab directory"
    VERBATIM
)

if (NOT BIIPS_DEBUG)
	# copy mexfile in the src matlab directory
	add_custom_command(TARGET matbiips
	    POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy ${MATBIIPS_TARGET_LOC} "${CMAKE_SOURCE_DIR}/matbiips/matlab/"
	    COMMENT "Copying mexfile in source matlab directory"
	    VERBATIM
	)
endif()
