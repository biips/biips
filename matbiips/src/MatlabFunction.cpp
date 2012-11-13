#include "MatlabFunction.cpp"
#include <mex.h>
virtual void
MatlabFunction::eval(ValArray & values, const NumArray::Array & paramValues) {

     
     int nrhs = paramValues.size();
    
     mxArray * prhs = new mxArray[nrhs]
     
     mxArray  plhs[1];

     for(i = 0; i < nrhs; ++i) {
            mwSize zeros[2] = {0, 0 };
	    prhs[i] = mxCreateNumericArray(2, zeros, mxDOUBLE_CLASS, mxREAL);
            mxSetDimensions(prhs[i], paramValues.DimPtr(), paramValues.NDim());
	    mxSetPr(prhs[i], paramValues.ValuesPtr());
     }

     mexCallMATLAB(1, plhs, nrhs, prhs, name_);
   
     for(i = 0; i < values.size(); ++i) {
              values[i] = mxGetPr(plhs[1])[i]
     }

     delete [] prhs;
    }

