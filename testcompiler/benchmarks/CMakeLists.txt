# copy bench cfg files to binary directory
file(GLOB cfg_files *.cfg)
file(COPY ${cfg_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB data_files *.R)
file(COPY ${data_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
