set(INTER_BIIPS  inter_biips.cpp)
add_custom_command(
OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/inter_biips.mex
DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/inter_biips.cpp 
COMMAND mkoctfile 
ARGS --mex  ${CMAKE_CURRENT_SOURCE_DIR}/inter_biips.cpp 
COMMENT "building mexfile"
)
add_custom_target(matbiips ALL
DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/inter_biips.mex
)
