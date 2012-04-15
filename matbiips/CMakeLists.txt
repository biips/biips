set(INTER_BIIPS  inter_biips.cpp Mostream.cpp)
set(Biips_base_INCLUDE ${CMAKE_SOURCE_DIR}/base/include)
set(Biips_compiler_INCLUDE ${CMAKE_SOURCE_DIR}/compiler/include)
set(Biips_core_INCLUDE ${CMAKE_SOURCE_DIR}/core/include)
	 
add_custom_command(
OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/inter_biips.mex
DEPENDS ${INTER_BIIPS}
COMMAND mkoctfile 
ARGS  -I${Biips_base_INCLUDE} -I${Biips_core_INCLUDE}
-I${Biips_compiler_INCLUDE}  "--mex" ${INTER_BIIPS} 
COMMENT "building mexfile")
add_custom_target(matbiips ALL
DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/inter_biips.mex
)
