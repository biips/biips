find_program(LATEX NAMES pdflatex)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/notePMMH.pdf
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/notePMMH.tex
    COMMAND ${LATEX}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/notePMMH.tex
    )
add_custom_target(docPMMH
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/notePMMH.pdf)
