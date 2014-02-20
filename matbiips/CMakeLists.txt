if (NOT FIND_OCTAVE)
    find_package(MATLAB)
    if (NOT MATLAB)
     message(FATAL_ERROR "cannot find Matlab") 
    endif(NOT MATLAB)
else ()
	find_package(Octave)
    if (NOT OCTAVE)
    message(FATAL_ERROR "cannot find Octave") 
    endif(NOT OCTAVE) 
endif()

add_subdirectory(matlab)
set(INTER_TARGET "${CMAKE_CURRENT_SOURCE_DIR}/src/inter_biips.${MEX_EXT}")
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(doc)

# create archive
if (WIN32)
   set(MATBIIPS_ARCHIVE "${CMAKE_BINARY_DIR}/matbiips_${Biips_VERSION}_${CMAKE_SYSTEM_PROCESSOR}_${CMAKE_SYSTEM_NAME}.zip")
else()
   set(MATBIIPS_ARCHIVE "${CMAKE_BINARY_DIR}/matbiips_${Biips_VERSION}_${CMAKE_SYSTEM_PROCESSOR}_${CMAKE_SYSTEM_NAME}.tar.gz")
endif()

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
