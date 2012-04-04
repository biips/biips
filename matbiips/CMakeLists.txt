add_custom_command(
OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/inter_biips.mex
COMMAND mkoctfile --mex ${CMAKE_CURRENT_BINARY_DIR}/inter_biips.cpp
)
add_custom_target(matbiips
DEPENDS ${Document_BINARY_DIR}/inter_biips.mex
)
