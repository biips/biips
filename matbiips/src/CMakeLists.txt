#define sources
file(GLOB MBIIPS_SRC inter_biips.cpp inter_utils.cpp Mostream.cpp MatlabFunction.cpp)
file(GLOB MBIIPS_H *.h *.hpp)

set(BIIPS_LIBS BiipsBase BiipsCompiler BiipsCore)
if (MATLAB OR OCTAVE) 
    include_directories(${Core_INCLUDE_DIRS}
		${Base_INCLUDE_DIRS}
		${Compiler_INCLUDE_DIRS}
		${MATLAB_INCLUDE_DIR}
		${Boost_INCLUDE_DIRS}
	)
	
	add_library(inter_biips SHARED EXCLUDE_FROM_ALL ${MBIIPS_SRC})
	target_link_libraries(inter_biips ${BIIPS_LIBS} ${MATLAB_LIBRARIES})
	set_target_properties(inter_biips PROPERTIES
		COMPILE_FLAGS "${MATLAB_COMPILE_FLAGS}"
		LINK_FLAGS "${MATLAB_LINK_FLAGS}"
		PREFIX ""
		SUFFIX .${MEX_EXT}
	)
	
    set(INTER_TARGET inter_biips)
    get_target_property(INTER_TARGET_L inter_biips LOCATION)
else()
    message (FATAL_ERROR "MATLAB not found on your computer.\n"
    			 "Try setting MATLAB_ROOT to the root path of 
    your MATLAB installation. eg: c:\\Program Files\\MATLAB\\R2012a\n"
    			 "Octave mkoct is not yet supported on 
    Windows.\n" 
                             "You should deactivate MATBIIPS compilation by 
    setting BUILD_MATBIIPS=OFF")
endif()

# install mexfile in the build directory
set(INTER_BIN ${CMAKE_BINARY_DIR}/matbiips/matlab/inter_biips.${MEX_EXT})
add_custom_command(OUTPUT ${INTER_BIN}
	DEPENDS ${INTER_TARGET}
    COMMAND ${CMAKE_COMMAND} -E copy ${INTER_TARGET_L} "${CMAKE_BINARY_DIR}/matbiips/matlab/"
    COMMENT "Copying mexfile in build matlab directory"
    VERBATIM
)

add_custom_target(matbiips DEPENDS ${INTER_BIN} BiipsCore BiipsCompiler
    BiipsBase)
