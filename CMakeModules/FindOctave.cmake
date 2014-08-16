# Variables defined:
# OCTAVE				path to OCTAVE exectuable or "OCTAVE-NOTFOUND"

# Variables defined if OCTAVE is not "OCTAVE-NOTFOUND":
# OCTAVE_CONFIG
# OCTAVE_VERSION
# MATLAB_COMMAND
# MATLAB_FLAGS
# MKOCTFILE
# MEX_COMMAND
# MEX_FLAGS
# MEX_EXT
# MEX_OBJ_EXT
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

if(OCTAVE_ROOT)
    file (TO_CMAKE_PATH "${OCTAVE_ROOT}/bin" OCTAVE_BINDIR)
endif()

if (OCTAVE_BINDIR)
    # try to find octave
    find_program(OCTAVE octave HINTS ${OCTAVE_BINDIR} NO_DEFAULT_PATH)
else()
    # try to find octave in default paths
    find_program(OCTAVE octave)
    get_filename_component(OCTAVE_BINDIR ${OCTAVE} DIRECTORY)
endif()

# Yes! found it
if (OCTAVE)
    message(STATUS "Found octave: ${OCTAVE}")
    set(MATLAB_COMMAND "${OCTAVE}")
    set(MATLAB_FLAGS --traditional)
    if (NOT WIN32)
        set(MATLAB_FLAGS ${MATLAB_FLAGS} --no-gui)
    endif()
    # define mex file extension
    set(MEX_EXT mex)
    # define object file extension
    set(MEX_OBJ_EXT o)

    # find mkoctfile program
    find_program(MKOCTFILE
        NAMES mkoctfile mkoctfile.exe
        PATHS ${OCTAVE_BINDIR} 
        NO_DEFAULT_PATH
    )
    set(MEX_COMMAND ${MKOCTFILE})
    set(MEX_FLAGS_Release -DOCTAVE --strip)
    set(MEX_FLAGS_Debug -DOCTAVE -g)
    set(MEX_FLAGS_RelWithDebInfo -DOCTAVE -g)
    if (WIN32)
        set(MEX_FLAGS_Release ${MEX_FLAGS_Release} -DWIN32)
        set(MEX_FLAGS_Debug ${MEX_FLAGS_Debug} -DWIN32)
        set(MEX_FLAGS_RelWithDebInfo ${MEX_FLAGS_RelWithDebInfo} -DWIN32)
    endif ()
    set(MEX_OUTPUT_OPT --output)
    set(MEX_COMPILE_OPT -c)
    set(MEX_MEX_OPT --mex)

    # find octave-config program
    find_program(OCTAVE_CONFIG octave-config ${OCTAVE_ROOT} ${OCTAVE_BINDIR})

    # octave version
    execute_process(COMMAND ${OCTAVE_CONFIG} -p VERSION
            OUTPUT_VARIABLE OCTAVE_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    # We define compile flags and find OCTAVE libraries to link with
    # for manual compilations not using mkoctfile.

    # define compile flags
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
        find_path(MATLAB_LIBRARY_DIR liboctave.so NAMES liboctave.la liboctave.dylib
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
    set(MATLAB_LIBRARIES
            "${OCTAVE_OCTINTERP_LIBRARY}"
            "${OCTAVE_OCTAVE_LIBRARY}"
        )
    if(OCTAVE_VERSION VERSION_LESS 3.8)
        find_library(OCTAVE_CRUFT_LIBRARY cruft
            PATHS ${MATLAB_LIBRARY_DIR}
            NO_DEFAULT_PATH
        )
        set(MATLAB_LIBRARIES
            ${MATLAB_LIBRARIES}
            "${OCTAVE_CRUFT_LIBRARY}"
        )
    endif()

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
