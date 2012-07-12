# search for mex utility 
find_package(Mex)
#define sources
set(MBIIPS_SRC inter_biips.cpp Mostream.cpp)
foreach(f ${MBIIPS_SRC})
   set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/${f})
endforeach(f ${MBIIPS_SRC})

#define includes
set(Biips_base_INCLUDE ${CMAKE_SOURCE_DIR}/base/include)
set(Biips_compiler_INCLUDE ${CMAKE_SOURCE_DIR}/compiler/include)
set(Biips_core_INCLUDE ${CMAKE_SOURCE_DIR}/core/include)
	 
# define libraries
set(MBIIPS_LIBS BiipsBase BiipsCore BiipsCompiler)
set(Biips_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

foreach(l ${MBIIPS_LIBS})
	set(EXTRA_LIBS ${EXTRA_LIBS} "-l${l}")
endforeach(l ${MBIIPS_LIBS})

#define mexfile compilation command
set(INTER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/inter_biips.cpp)
set(INTER_OUT "${CMAKE_CURRENT_BINARY_DIR}/inter_biips.${MEX_EXT}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(MEX_OPT ${MEX_OPT} "-g")
endif()
add_custom_command(
OUTPUT ${INTER_OUT}
DEPENDS ${INTER_SRC} 
COMMAND ${MEX_COMMAND} 
ARGS  -v ${MEX_OPT} -I${Biips_base_INCLUDE} -I${Biips_core_INCLUDE} -I${Biips_compiler_INCLUDE}  
-L${Biips_INSTALL_LIBDIR} ${EXTRA_LIBS}
${SOURCES}
COMMENT "building mexfile")

set(INTER_INST "${CMAKE_INSTALL_PREFIX}/inter_biips.${MEX_EXT}")

add_custom_command(
OUTPUT ${INTER_INST}
DEPENDS ${INTER_OUT}
COMMAND ${CMAKE_COMMAND}
ARGS -E copy ${INTER_OUT} ${CMAKE_INSTALL_PREFIX})

#define target
add_custom_target(matbiips 
DEPENDS ${INTER_INST}
)
install(FILES cono.m hmm_1d_lin.bug x.dat y.dat DESTINATION .)
