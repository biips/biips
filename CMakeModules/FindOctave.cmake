# Variables defined:
# OCTAVE				path to OCTAVE exectuable or "OCTAVE-NOTFOUND"

# Variables defined if OCTAVE is not "OCTAVE-NOTFOUND":
# OCTAVE_CONFIG
# MATLAB_COMMAND
# MATLAB_FLAGS
# MEX_EXT
# MATLAB_LIBRARY_DIR 	
# MATLAB_INCLUDE_DIR 	
# MATLAB_COMPILE_FLAGS 	
# MATLAB_LIBRARIES	
# OCTAVE_OCTINTERPRET_LIBRARY		
# OCTAVE_OCTAVE_LIBRARY	
# OCTAVE_CRUFT_LIBRARY	

# Variables that can be set by the user:
# OCTAVE_ROOT		path to the OCTAVE bindir. Helps finding OCTAVE.
# OCTAVE_LIBRARYDIR 	path to the OCTAVE libraries
# OCTAVE_INCLUDEDIR 	path to the OCTAVE headers



if (NOT $ENV{OCTAVE_ROOT} STREQUAL "")
    file (TO_CMAKE_PATH "$ENV{OCTAVE_ROOT}" OCTAVE_ROOT)
endif()


if (OCTAVE_ROOT)
    # try to find octave
    find_program(OCTAVE octave HINTS ${OCTAVE_ROOT} NO_DEFAULT_PATH)
else()
    # try to find octave in default paths
    find_program(OCTAVE octave)
endif()

if (OCTAVE)
    message(STATUS "octave found: ${OCTAVE}")
    set(MATLAB_COMMAND "${OCTAVE}")
    set(MATLAB_FLAGS --traditional)
    set(MEX_EXT mex)

    find_program(OCTAVE_CONFIG octave-config ${OCTAVE_ROOT})

    if(NOT OCTAVE_CONFIG)
        message(ERROR "cannot find octave-config")
    endif()

    message (STATUS "octave-config found: ${OCTAVE_CONFIG}")
    set(MATLAB_COMPILE_FLAGS -DOCTAVE)

    # find OCTAVE libaries
    if (NOT $ENV{OCTAVE_LIBRARYDIR} STREQUAL "")
        file (TO_CMAKE_PATH $ENV{OCTAVE_LIBRARYDIR} OCTAVE_LIBRARYDIR)
    endif()

    if (OCTAVE_LIBRARYDIR)
        file (TO_CMAKE_PATH ${OCTAVE_LIBRARYDIR} MATLAB_LIBRARY_DIR)
    else(OCTAVE_LIBRARYDIR)
        execute_process(COMMAND ${OCTAVE_CONFIG} -p LIBDIR
                OUTPUT_VARIABLE OCTAVE_LIBDIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND ${OCTAVE_CONFIG} -p OCTLIBDIR
                OUTPUT_VARIABLE OCTAVE_OCTLIBDIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        find_path(MATLAB_LIBRARY_DIR liboctave.so NAMES liboctave.la
                  PATHS ${OCTAVE_LIBDIR} ${OCTAVE_OCTLIBDIR})
    endif(OCTAVE_LIBRARYDIR)

    find_library(OCTAVE_OCTINTERP_LIBRARY octinterp  
        PATHS ${MATLAB_LIBRARY_DIR}
        NO_DEFAULT_PATH
    )
    find_library(OCTAVE_OCTAVE_LIBRARY octave 
        PATHS ${MATLAB_LIBRARY_DIR}
        NO_DEFAULT_PATH
    )
    find_library(OCTAVE_CRUFT_LIBRARY cruft
        PATHS ${MATLAB_LIBRARY_DIR}
        NO_DEFAULT_PATH
    )
    set(MATLAB_LIBRARIES
        "${OCTAVE_OCTINTERP_LIBRARY}"
        "${OCTAVE_OCTAVE_LIBRARY}"
        "${OCTAVE_CRUFT_LIBRARY}"
    )

    # find OCTAVE includes
    if (NOT $ENV{OCTAVE_INCLUDEDIR} STREQUAL "")
        file (TO_CMAKE_PATH $ENV{OCTAVE_INCLUDEDIR} OCTAVE_INCLUDEDIR)
    endif()

    if (OCTAVE_INCLUDEDIR)
        find_path(MATLAB_INCLUDE_DIR mex.h
            PATHS ${OCTAVE_INCLUDEDIR}
        )
    else(OCTAVE_INCLUDEDIR)
        execute_process(COMMAND ${OCTAVE_CONFIG} -p OCTINCLUDEDIR
                OUTPUT_VARIABLE OCTAVE_INCLUDEDIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        find_path(MATLAB_INCLUDE_DIR "mex.h"
                  PATHS ${OCTAVE_INCLUDEDIR} ${OCTAVE_INCLUDEDIR}/octave)
    endif(OCTAVE_INCLUDEDIR)


endif(OCTAVE)



mark_as_advanced(
    MATLAB_LIBRARIES
    OCTAVE_OCTINTERPRET_LIBRARY		
    OCTAVE_OCTAVE_LIBRARY	
    OCTAVE_CRUFT_LIBRARY	
    MATLAB_COMMAND
    MATLAB_FLAGS
    MATLAB_LIBRARY_DIR
    MATLAB_INCLUDE_DIR
    MATLAB_COMPILE_FLAGS
)
