#define sources
set(MBIIPS_SRC inter_biips.cpp Mostream.cpp inter_utils.cpp)
file(GLOB MBIIPS_H *.h)
foreach(f ${MBIIPS_SRC})
   set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/${f})
endforeach(f ${MBIIPS_SRC})
	 
# define libraries
if (WIN32)
    set(MBIIPS_LIBS BiipsBase-${MATLAB_ARCH} BiipsCompiler-${MATLAB_ARCH} BiipsCore-${MATLAB_ARCH})
else (WIN32)
    set(MBIIPS_LIBS BiipsBase BiipsCompiler BiipsCore)
endif (WIN32)

#set(Biips_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

foreach(l ${MBIIPS_LIBS})
	get_target_property(name ${l} PROPERTY OUTPUT_NAME)
	if (BUILD_SHARED_LIBS)
		get_target_property(dir ${l} PROPERTY LIBRARY_OUTPUT_DIRECTORY)
	else(BUILD_SHARED_LIBS)
		get_target_property(dir ${l} PROPERTY ARCHIVE_OUTPUT_DIRECTORY)
	endif(BUILD_SHARED_LIBS)
	set(EXTRA_LIBS ${EXTRA_LIBS} "-L${dir} -l${name}")
	#set(EXTRA_LIBS ${EXTRA_LIBS} "-l${name}")
endforeach(l ${MBIIPS_LIBS})

#define mexfile compilation command
set(INTER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/inter_biips.cpp)
set(INTER_OUT "${CMAKE_CURRENT_BINARY_DIR}/inter_biips.${MEX_EXT}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(MEX_OPT ${MEX_OPT} "-g")
endif()

#define includes flags
set (INCLUDE_DIRS ${BiipsCore_INCLUDE_DIRS} ${BiipsBase_INCLUDE_DIRS} ${BiipsCompiler_INCLUDE_DIRS})
foreach(d ${INCLUDE_DIRS})
	set(INCLUDE_FLAGS ${INCLUDE_FLAGS} -I${d})
endforeach(d ${INCLUDE_DIRS})

add_custom_command(
OUTPUT ${INTER_OUT}
DEPENDS ${INTER_SRC} ${MBIIPS_H} ${MBIIPS_SRC}
COMMAND ${MEX_COMMAND} 
ARGS  -v  ${MEX_OPT} ${INCLUDE_FLAGS} #${Biips_INSTALL_LIBDIR} 
${EXTRA_LIBS} ${SOURCES} 
COMMENT "building mexfile")

#set(INTER_INST "${CMAKE_INSTALL_PREFIX}/inter_biips.${MEX_EXT}")

#add_custom_command(
#OUTPUT ${INTER_INST}
#DEPENDS ${INTER_OUT}
#COMMAND ${CMAKE_COMMAND}
#ARGS -E copy ${INTER_OUT} ${CMAKE_INSTALL_PREFIX})
#install(FILES ${INTER_OUT} DESTINATION ${CMAKE_INSTALL_PREFIX})

# installation dans le build du mexfile
set(INTER_TEST "${CMAKE_BINARY_DIR}/matbiips/tests/inter_biips.${MEX_EXT}")
message(STATUS ${INTER_TEST})
add_custom_command(
OUTPUT ${INTER_TEST}
DEPENDS ${INTER_OUT}
COMMAND ${CMAKE_COMMAND}
ARGS -E copy ${INTER_OUT} "${CMAKE_BINARY_DIR}/matbiips/tests/")

#define target
add_custom_target(matbiips 
DEPENDS  ${INTER_TEST}
)
