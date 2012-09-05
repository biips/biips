# Variables defined:
# MATLAB				path to MATLAB exectuable or "MATLAB-NOTFOUND"
# MEX_FOUND 			equals to "MATLAB", "OCTAVE" or "0"

# Variables defined if MEX_FOUND is not "0":
# MEX_COMMAND			
# MEX_EXT				

# Variables defined if MATLAB is not "MATLAB-NOTFOUND":
# MATLAB_COMMAND		
# MATLAB_FLAGS			
# MATLAB_BINDIR			
# MEX_OPT
# MEXEXT_COMMAND		
# MATLAB_ARCH			equals to "i386" or "x64"
# MATLAB_LIBRARY_DIR 	(Windows only)
# MATLAB_INCLUDE_DIR 	(Windows only)
# MATLAB_COMPILE_FLAGS 	(Windows only)
# MATLAB_LIBRARIES		(Windows only)
# MATLAB_MEX_LIBRARY	(Windows only)
# MATLAB_MX_LIBRARY		(Windows only)
# MATLAB_MAT_LIBRARY	(Windows only)

# Variables defined if MEX_FOUND is "OCTAVE":
# MKOCT					path to mkoctfile exectuable or "MKOCT-NOTFOUND"
# OCTAVE				path to octave exectuable or "OCTAVE-NOTFOUND"
# if OCTAVE is not "OCTAVE-NOTFOUND":
# MATLAB_COMMAND		
# MATLAB_FLAGS			

# Variables that can be set by the user:
# MATLAB_ROOT		path to the root of MATLAB install. Helps finding MATLAB.
# MATLAB_LIBRARYDIR 	path to the MATLAB libraries (Windows only)
# MATLAB_INCLUDEDIR 	path to the MATLAB headers (Windows only)

if (NOT $ENV{MATLAB_ROOT} STREQUAL "")
    file (TO_CMAKE_PATH "$ENV{MATLAB_ROOT}" MATLAB_ROOT)
endif()

if (MATLAB_ROOT)
    file (TO_CMAKE_PATH "${MATLAB_ROOT}/bin" MATLAB_BINDIR)
elseif(APPLE)
    file(GLOB _MATLAB_PATHS "/Applications/MATLAB_*")
    find_path(MATLAB_ROOT "license.txt" ${_MATLAB_PATHS})
    file (TO_CMAKE_PATH "${MATLAB_ROOT}/bin" MATLAB_BINDIR)
endif(MATLAB_ROOT)

# try to find matlab
find_program(MATLAB matlab ${MATLAB_BINDIR})

# Yes! found it
if (MATLAB)
    set(MATLAB_COMMAND "${MATLAB}")
    set(MATLAB_FLAGS "-nojvm -nosplash")
    if (NOT MATLAB_BINDIR)
        # if matlab if found in /usr/local/bin
        # mex and mexext programs will certainly not be there
        # hence we define MATLAB_BINDIR using the 'matlabroot' MATLAB command
        execute_process(COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS}
            -r "fprintf(fopen('_matlab_root','w'), '%s', matlabroot); exit"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        file (READ "${CMAKE_BINARY_DIR}/_matlab_root" MATLAB_ROOT)
        file (REMOVE "${CMAKE_BINARY_DIR}/_matlab_root")
        file (TO_CMAKE_PATH "${MATLAB_ROOT}/bin" MATLAB_BINDIR)
    endif(NOT MATLAB_BINDIR)

    # find mex program
    find_program(MEX_COMMAND
        NAMES mex mex.bat
        PATHS ${MATLAB_BINDIR} 
        NO_DEFAULT_PATH
    )
    if (MEX_COMMAND)
        message (STATUS "mex (Matlab) found : ${MEX_COMMAND}")
        set(MEX_FOUND MATLAB)
    endif(MEX_COMMAND)

    # find mexext program
    find_program(MEXEXT_COMMAND
        NAMES mexext mexext.bat
        PATHS ${MATLAB_BINDIR} 
        NO_DEFAULT_PATH
    )
    if (MEXEXT_COMMAND)
        # define mex extension
        execute_process(COMMAND ${MEXEXT_COMMAND}
            OUTPUT_VARIABLE MEX_EXT
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        message(STATUS "mex extension on this machine : ${MEX_EXT}")

        # define matlab architecture
        # use MATLAB_ARCH on Windows to link with the correct BiiPS libs
        if ("${MEX_EXT}" MATCHES ".*64.*")
            set(MATLAB_ARCH x64)
            set(MEX_OPT -largeArrayDims)
        else()
            set(MATLAB_ARCH i386)
        endif()
    endif(MEXEXT_COMMAND)

    # On Windows, do not use mex program (gcc not supported)
    # to compile mex-files but use cmake commands instead
    # hence, define compile flags and find MATLAB libraries to link with
    if (WIN32)
        # define compile flags
        set(MATLAB_COMPILE_FLAGS -DMATLAB_MEX_FILE)

        if (NOT $ENV{MATLAB_LIBRARYDIR} STREQUAL "")
            file (TO_CMAKE_PATH $ENV{MATLAB_LIBRARYDIR} MATLAB_LIBRARYDIR)
        endif()
        if (MATLAB_LIBRARYDIR)
            file (TO_CMAKE_PATH ${MATLAB_LIBRARYDIR} MATLAB_LIBRARYDIR)
        else(MATLAB_LIBRARYDIR)
            if (${MATLAB_ARCH} STREQUAL "i386")
                file(TO_CMAKE_PATH "${MATLAB_BINDIR}/win32" MATLAB_LIBRARY_DIR)
                set(MATLAB_COMPILE_FLAGS "${MATLAB_COMPILE_FLAGS} -m32")
                set(MATLAB_LINK_FLAGS "${MATLAB_LINK_FLAGS} -m32")
            elseif (${MATLAB_ARCH} STREQUAL "x64")
                file(TO_CMAKE_PATH "${MATLAB_BINDIR}/win64" MATLAB_LIBRARY_DIR)
                set(MATLAB_COMPILE_FLAGS "${MATLAB_COMPILE_FLAGS} -m64")
                set(MATLAB_LINK_FLAGS "${MATLAB_LINK_FLAGS} -m64")
            endif()
        endif(MATLAB_LIBRARYDIR)

        # find MATLAB libaraies
        find_library(MATLAB_MEX_LIBRARY mex
            PATHS ${MATLAB_LIBRARY_DIR}
        )
        find_library(MATLAB_MX_LIBRARY mx
            PATHS ${MATLAB_LIBRARY_DIR}
        )
            find_library(MATLAB_MAT_LIBRARY mat
        PATHS ${MATLAB_LIBRARY_DIR}
        )
        set(MATLAB_LIBRARIES
            "${MATLAB_MEX_LIBRARY}"
            "${MATLAB_MX_LIBRARY}"
            "${MATLAB_MAT_LIBRARY}"
        )

        # find MATLAB includes
        if (NOT $ENV{MATLAB_INCLUDEDIR} STREQUAL "")
            file(TO_CMAKE_PATH $ENV{MATLAB_INCLUDEDIR} MATLAB_INCLUDEDIR)
        endif()
        if (MATLAB_INCLUDEDIR)
            find_path(MATLAB_INCLUDE_DIR mex.h
                PATHS ${MATLAB_INCLUDEDIR}
            )
        else(MATLAB_INCLUDEDIR)
            file(TO_CMAKE_PATH "${MATLAB_BINDIR}/../extern/include" MATLAB_INCLUDEDIR)
            find_path(MATLAB_INCLUDE_DIR mex.h
                PATHS ${MATLAB_INCLUDEDIR}
            )
        endif(MATLAB_INCLUDEDIR)
    endif(WIN32)

else(MATLAB) # We did not find matlab
    # try with octave	
    find_program(MKOCT mkoctfile)
    if(MKOCT)
        message (STATUS "mex (Octave) found : ${MKOCT}")
        set(MEX_FOUND OCTAVE)
        set(MEX_COMMAND "${MKOCT} --mex")
        set(MEX_EXT mex)
        find_program(OCTAVE "octave")
        if (OCTAVE)
            set(MATLAB_COMMAND "${OCTAVE}")
            set(MATLAB_FLAGS --traditional)
        endif(OCTAVE)	    
    else(MKOCT)
        message(FATAL_ERROR "No Matlab or Octave mex compiler found") 
        set(MEX_FOUND 0)
    endif(MKOCT)
endif(MATLAB)

mark_as_advanced(
    MATLAB_LIBRARIES
    MATLAB_MEX_LIBRARY
    MATLAB_MX_LIBRARY
    MATLAB_MAT_LIBRARY
    MEX_FOUND
    MEX_COMMAND
    MEX_EXT
    MATLAB_COMMAND
    MATLAB_FLAGS
    MATLAB_BINDIR
    MEXEXT_COMMAND
    MATLAB_ARCH
    MATLAB_LIBRARY_DIR
    MATLAB_INCLUDE_DIR
    MATLAB_COMPILE_FLAGS
)
