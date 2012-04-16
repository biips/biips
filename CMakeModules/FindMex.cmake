#first search for MATLAB

find_program(MATLAB matlab)
if (MATLAB)
   message (STATUS "MATLAB executable found=" ${MATLAB})
   get_filename_component(MATLAB_BINDIR ${MATLAB} PATH)
   message(STATUS "MATLAB_BINDIR=" ${MATLAB_BINDIR}$)
else(MATLAB)
  find_program(MKOCT mkoctfile)
  if(MKOCT)
    message (STATUS "mkoctfile =" ${MKOCT})
  else(MKOCT)
    message(STATUS "No octave or Matlab found => no mex on your computer") 
    set(MEX_FOUND 0)
  endif(MKOCT)
endif(MATLAB)

