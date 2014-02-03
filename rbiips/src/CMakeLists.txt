# configure Makevars.win file
if (WIN32)
    configure_file (
      Makevars.win.IN
      ${CMAKE_CURRENT_SOURCE_DIR}/Makevars.win
    	@ONLY
    )
endif()

file(GLOB cpp_files RELATIVE ${PKG_SRC_DIR} *.cpp)
file(GLOB h_files RELATIVE ${PKG_SRC_DIR} *.h)

# add files to parent scope PKG_FILES list
set(PKG_FILES ${PKG_FILES} src/Makevars.in src/Makevars.win ${cpp_files} ${h_files} PARENT_SCOPE)
