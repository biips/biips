file(GLOB bug_files RELATIVE ${PKG_SRC_DIR} *.bug)

# add files to parent scope PKG_FILES list
set(PKG_FILES ${PKG_FILES} ${bug_files} PARENT_SCOPE)