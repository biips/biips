# Variables defined:
# MATLAB				path to MATLAB exectuable or "MATLAB-NOTFOUND"

# Variables defined if MATLAB is not "MATLAB-NOTFOUND":
# MATLAB_COMMAND
# MATLAB_FLAGS
# MATLAB_BINDIR
# MEX_COMMAND
# MEX_EXT
# MEX_OPT
# MEXEXT_COMMAND
# MATLAB_ARCH			equals to "i386" or "x64"
# MATLAB_LIBRARY_DIR 	
# MATLAB_INCLUDE_DIR 	
# MATLAB_COMPILE_FLAGS 	
# MATLAB_LIBRARIES		
# MATLAB_MEX_LIBRARY	
# MATLAB_MX_LIBRARY		
# MATLAB_MAT_LIBRARY	

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
find_program(MATLAB matlab HINTS ${MATLAB_BINDIR} NO_DEFAULT_PATH)
    
if (NOT MATLAB)
    # try to find matlab in default paths
    find_program(MATLAB matlab)
endif()

# Yes! found it
if (MATLAB)
    message(STATUS "Found MATLAB: ${MATLAB}")
    set(MATLAB_COMMAND "${MATLAB}")
    set(MATLAB_FLAGS -nojvm -nosplash)
    if (NOT MATLAB_BINDIR)
        # if matlab is found in /usr/local/bin
        # mex and mexext programs will certainly not be there
        # hence we define MATLAB_BINDIR using the 'matlabroot' MATLAB command
        file (TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/matlab_root" _matlab_root_file)
		execute_process(COMMAND ${MATLAB_COMMAND} ${MATLAB_FLAGS}
			-r "fprintf(fopen('matlab_root','w'), '%s', matlabroot); exit"
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            OUTPUT_QUIET)
		while (NOT MATLAB_ROOT)
			while (NOT EXISTS ${_matlab_root_file})
				# wait for previous execute_process
			endwhile()
			file (READ "${_matlab_root_file}" MATLAB_ROOT)
		endwhile()
        file (REMOVE "${_matlab_root_file}")
        file (TO_CMAKE_PATH "${MATLAB_ROOT}/bin" MATLAB_BINDIR)
    endif(NOT MATLAB_BINDIR)

    # find mex program
    find_program(MEX_COMMAND
        NAMES mex mex.bat
        PATHS ${MATLAB_BINDIR} 
        NO_DEFAULT_PATH
    )

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
        message(STATUS "mex extension on this machine: ${MEX_EXT}")

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
    # define compile flags
    set(MATLAB_COMPILE_FLAGS "-DMATLAB_MEX_FILE -fpermissive")

    if (NOT $ENV{MATLAB_LIBRARYDIR} STREQUAL "")
        file (TO_CMAKE_PATH $ENV{MATLAB_LIBRARYDIR} MATLAB_LIBRARYDIR)
    endif()
    if (MATLAB_LIBRARYDIR)
        file (TO_CMAKE_PATH ${MATLAB_LIBRARYDIR} MATLAB_LIBRARYDIR)
    else(MATLAB_LIBRARYDIR)
        if (WIN32)
        if (${MATLAB_ARCH} STREQUAL "i386")
            file(TO_CMAKE_PATH "${MATLAB_BINDIR}/win32" MATLAB_LIBRARY_DIR)
            set(MATLAB_COMPILE_FLAGS "${MATLAB_COMPILE_FLAGS} -m32")
            set(MATLAB_LINK_FLAGS "${MATLAB_LINK_FLAGS} -m32")
        elseif (${MATLAB_ARCH} STREQUAL "x64")
            file(TO_CMAKE_PATH "${MATLAB_BINDIR}/win64" MATLAB_LIBRARY_DIR)
            set(MATLAB_COMPILE_FLAGS "${MATLAB_COMPILE_FLAGS} -m64")
            set(MATLAB_LINK_FLAGS "${MATLAB_LINK_FLAGS} -m64")
	endif()
        elseif(APPLE)
            file(TO_CMAKE_PATH "${MATLAB_BINDIR}/maci64" MATLAB_LIBRARY_DIR)
            set(MATLAB_COMPILE_FLAGS "${MATLAB_COMPILE_FLAGS} -m64")
            set(MATLAB_LINK_FLAGS "${MATLAB_LINK_FLAGS} -m64")
        else() #nonwin32 or apple
        if (${MATLAB_ARCH} STREQUAL "i386")
            file(TO_CMAKE_PATH "${MATLAB_BINDIR}/glnx86" MATLAB_LIBRARY_DIR)
            set(MATLAB_COMPILE_FLAGS "${MATLAB_COMPILE_FLAGS} -m32")
            set(MATLAB_LINK_FLAGS "${MATLAB_LINK_FLAGS} -m32")
        elseif (${MATLAB_ARCH} STREQUAL "x64")
            file(TO_CMAKE_PATH "${MATLAB_BINDIR}/glnxa64" MATLAB_LIBRARY_DIR)
            set(MATLAB_COMPILE_FLAGS "${MATLAB_COMPILE_FLAGS} -m64")
            set(MATLAB_LINK_FLAGS "${MATLAB_LINK_FLAGS} -m64")
        endif()
    
        endif(WIN32)
    endif(MATLAB_LIBRARYDIR)

    # find MATLAB libaries
    find_library(MATLAB_MEX_LIBRARY mex
        PATHS ${MATLAB_LIBRARY_DIR}
        NO_DEFAULT_PATH
    )
    find_library(MATLAB_MX_LIBRARY mx
        PATHS ${MATLAB_LIBRARY_DIR}
        NO_DEFAULT_PATH
    )
    find_library(MATLAB_MAT_LIBRARY mat
        PATHS ${MATLAB_LIBRARY_DIR}
        NO_DEFAULT_PATH
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

endif(MATLAB)

mark_as_advanced(
    MATLAB_LIBRARIES
    MATLAB_MEX_LIBRARY
    MATLAB_MX_LIBRARY
    MATLAB_MAT_LIBRARY
    MATLAB_COMMAND
    MATLAB_FLAGS
    MATLAB_BINDIR
    MATLAB_ARCH
    MATLAB_LIBRARY_DIR
    MATLAB_INCLUDE_DIR
    MATLAB_COMPILE_FLAGS
)
