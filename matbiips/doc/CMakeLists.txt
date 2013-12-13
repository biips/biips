find_package(LATEX)
add_custom_command(OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/tuto_matbiips.pdf
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tuto_matbiips.tex
                   COMMAND ${PDFLATEX_COMPILER}
                   ARGS ${CMAKE_CURRENT_SOURCE_DIR}/tuto_matbiips.tex) 
add_custom_target(matbiips_doc 
                  DEPENDS  ${CMAKE_CURRENT_BINARY_DIR}/tuto_matbiips.pdf) 

