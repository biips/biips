add_subdirectory(extdata)
add_subdirectory(staticdocs)

# add files to parent scope PKG_FILES list
set(PKG_FILES ${PKG_FILES} PARENT_SCOPE)
