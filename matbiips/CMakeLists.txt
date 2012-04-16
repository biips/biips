#define sources
set(MBIIPS_SRC inter_biips.cpp Mostream.cpp)
foreach(f ${MBIIPS_SRC})
   set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/${f})
endforeach(f ${MBIIPS_SRC})

#define includes
set(Biips_INSTALL_INC ${CMAKE_INSTALL_PREFIX}/include/biips)
set(Biips_base_INCLUDE ${Biips_INSTALL_INC}/base/)
set(Biips_compiler_INCLUDE ${Biips_INSTALL_INC}/compiler/)
set(Biips_core_INCLUDE ${Biips_INSTALL_INC}/core/)
	 
# define libraries
set(MBIIPS_LIBS BiipsBase BiipsCore BiipsCompiler)
set(Biips_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

foreach(l ${MBIIPS_LIBS})
	set(EXTRA_LIBS ${EXTRA_LIBS} "-l${l}")
endforeach(l ${MBIIPS_LIBS})

#define mexfile compilation command
add_custom_command(
OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/inter_biips.mex
DEPENDS ${SOURCES} 
COMMAND mex 
ARGS  -I${Biips_base_INCLUDE} -I${Biips_core_INCLUDE} -I${Biips_compiler_INCLUDE}  
-L${Biips_INSTALL_LIBDIR} ${EXTRA_LIBS}
${SOURCES}
COMMENT "building mexfile")

#define target
add_custom_target(matbiips ALL
DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/inter_biips.mex
)
