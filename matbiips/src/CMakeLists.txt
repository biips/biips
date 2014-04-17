#define sources
file(GLOB MATBIIPS_SRC 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h 
${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fpermissive")

set(BIIPS_LIBS BiipsBase BiipsCompiler BiipsCore)
if (MATLAB OR OCTAVE) 
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
