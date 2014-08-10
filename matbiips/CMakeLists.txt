if (FIND_OCTAVE)
    find_package(Octave)
    if (NOT OCTAVE)
      message(FATAL_ERROR "Can not find Octave") 
    endif() 
else()
    find_package(MATLAB)
    if (NOT MATLAB)
      message(FATAL_ERROR "Can not find Matlab") 
    endif()
    
    if ((${MATLAB_ARCH} STREQUAL "x64") AND (BITS EQUAL 32))
        message(FATAL_ERROR "Can not build Matbiips for 32bit. Matlab architecture is 64bit. You can use cmake option -DBUILD_MATBIIPS=OFF to deactivate Matbiips build")
    elseif ((${MATLAB_ARCH} STREQUAL "i386") AND (BITS EQUAL 64))
        message(FATAL_ERROR "Can not build Matbiips for 64bit. Matlab architecture is 32bit. You can use cmake option -DBUILD_MATBIIPS=OFF to deactivate Matbiips build")
    endif ()
endif()

add_subdirectory(matlab)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(doc)

# create archive
if (OCTAVE)
  set(_name matbiips-octave)
else()
  set(_name matbiips)
endif()

if (WIN32)
  set(_ext zip)
else()
  set(_ext tar.gz)
endif()

set(MATBIIPS_ARCHIVE "${CMAKE_BINARY_DIR}/${_name}_${Biips_VERSION}_${SYSTEM_ARCH}_${CMAKE_SYSTEM_NAME}.${_ext}")

file(GLOB_RECURSE packfiles RELATIVE "${CMAKE_SOURCE_DIR}"
    matlab/*.m
    matlab/*.${MEX_EXT}
)
add_custom_command( OUTPUT ${MATBIIPS_ARCHIVE}
  DEPENDS matbiips
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar cafv ${MATBIIPS_ARCHIVE}
  ${packfiles}
  COMMENT "Packaging MatBiips"
)
    
#define target
add_custom_target(matbiips_package
    DEPENDS  ${MATBIIPS_ARCHIVE}
)
