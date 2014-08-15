
if (WIN32)
    find_program(ZIP wzzip NAMES 7z
      PATHS "$ENV{ProgramFiles}/WinZip"
      "$ENV{ProgramW6432}/WinZip"
      "$ENV{ProgramFiles}/7-Zip"
      "$ENV{ProgramW6432}/7-Zip")

    string(REGEX MATCH wzzip|7z ZIP_PROGRAM ${ZIP})

    if (ZIP_PROGRAM STREQUAL wzzip)
        set(ZIP_COMMAND ${ZIP})
    elseif (ZIP_PROGRAM STREQUAL 7z)
        set(ZIP_COMMAND ${ZIP} a -tzip)
    endif()

    if(NOT ZIP)
      find_package(Cygwin)
      find_program(ZIP zip PATHS "${CYGWIN_INSTALL_PATH}/bin")
      set(ZIP_COMMAND ${ZIP})
    endif()

else()

  find_program(ZIP zip)
  set(ZIP_COMMAND ${ZIP})

endif()

include ( FindPackageHandleStandardArgs )

# handle the QUIETLY and REQUIRED arguments and set R_FOUND to TRUE if 
# all listed variables are TRUE
find_package_handle_standard_args (Zip
    REQUIRED_VARS ZIP
)

mark_as_advanced(
  ZIP_COMMAND
)
