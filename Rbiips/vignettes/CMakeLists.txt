file(GLOB rmd_files RELATIVE ${PKG_SRC_DIR} *.Rmd)

# add files to parent scope PKG_FILES list
set(PKG_FILES ${PKG_FILES} ${rmd_files} PARENT_SCOPE)
