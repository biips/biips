#define sources
file(GLOB MBIIPS_SRC inter_biips.cpp inter_utils.cpp Mostream.cpp MatlabFunction.cpp)
file(GLOB MBIIPS_H *.h *.hpp)

if (MATLAB)
        if (WIN32)
          set(BIIPS_LIBS  BiipsCompiler-${MATLAB_ARCH} BiipsBase-${MATLAB_ARCH} BiipsCore-${MATLAB_ARCH})
        else(WIN32)
          set(BIIPS_LIBS  BiipsCompiler BiipsBase BiipsCore)
        endif(WIN32)
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
                
#octave
else()
    set(BIIPS_LIBS BiipsBase BiipsCompiler BiipsCore)
	foreach(l ${BIIPS_LIBS})
			get_property(MYLIB TARGET ${l} PROPERTY LOCATION)
			get_filename_component(dir ${MYLIB} PATH)
			set(EXTRA_LIBS ${EXTRA_LIBS} -L${dir} -l${l})
	endforeach(l ${BIIPS_LIBS})

	#define mexfile compilation command
	set(INTER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/inter_biips.cpp)
	set(INTER_TARGET "${CMAKE_CURRENT_BINARY_DIR}/inter_biips.${MEX_EXT}")
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(MEX_OPT ${MEX_OPT} -g)
	endif()

	#define includes flags
	set (INCLUDE_DIRS ${Core_INCLUDE_DIRS} ${Base_INCLUDE_DIRS} ${Compiler_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
	foreach(d ${INCLUDE_DIRS})
		set(INCLUDE_FLAGS ${INCLUDE_FLAGS} -I${d})
	endforeach(d ${INCLUDE_DIRS})

	add_custom_command(
    	OUTPUT ${INTER_TARGET}
    	DEPENDS ${INTER_SRC} ${MBIIPS_H} ${MBIIPS_SRC} ${BIIPS_LIBS}
    	COMMAND ${MEX_COMMAND} -v 
    	    ${MEX_OPT} ${INCLUDE_FLAGS} ${MBIIPS_SRC} ${EXTRA_LIBS} 
    	COMMENT "Building mexfile"
    	VERBATIM
	)
endif()

# install mexfile in the build directory
set(INTER_BIN ${CMAKE_BINARY_DIR}/matbiips/matlab/inter_biips.${MEX_EXT})
add_custom_command(OUTPUT ${INTER_BIN}
	DEPENDS ${INTER_TARGET}
    COMMAND ${CMAKE_COMMAND} -E copy ${INTER_TARGET_L} "${CMAKE_BINARY_DIR}/matbiips/matlab/"
    COMMENT "Copying mexfile in build matlab directory"
    VERBATIM
)

add_custom_target(matbiips ALL DEPENDS ${INTER_BIN} BiipsCore BiipsCompiler
    BiipsBase)
