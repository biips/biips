file(GLOB mfiles *.m)
file(GLOB bugfiles *.bug)
file(COPY ${mfiles} ${bugfiles} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
