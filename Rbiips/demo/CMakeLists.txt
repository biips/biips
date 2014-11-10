file(GLOB r_files RELATIVE ${PKG_SRC_DIR} *.R)

# add files to parent scope PKG_FILES list
set(PKG_FILES ${PKG_FILES} ${r_files} PARENT_SCOPE)
