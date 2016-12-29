# copy bug files to binary directory
file (GLOB bug_files *.bug)
file(COPY ${bug_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
