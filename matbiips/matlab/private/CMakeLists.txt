file(GLOB mfiles *.m)
file(COPY ${mfiles} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
