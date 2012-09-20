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



if (MEX)
	if (${MEX} MATCHES "mkoctfile")
             find_package(Octave)
        elseif (${MEX} MATCHES "mex")
		find_package(MATLAB)
        else(${MEX} MATCHES "mkoctfile")
		message(FATAL_ERROR "I do not understand your MEX definitions : try mkoctfile or mex")
        endif(${MEX} MATCHES "mkoctfile")
else(MEX)
        find_package(MATLAB)
	find_package(Octave)
	if (MATLAB)
	 message("USE matlab as mexfile compiler")
	elseif(Octave)
	     message("USE mkoctfile as mexfile compiler")
        else(MATLAB)
           message(FATAL_ERROR "No Matlab or Octave mex compiler found") 
   endif(MATLAB)
endif(MEX)	

mark_as_advanced(
    MEX_FOUND
    MEX_COMMAND
    MEX_EXT
    MEXEXT_COMMAND
)
