#include "MatlabFunction.cpp"
#include <mex.h>
virtual void
MatlabFunction::eval(ValArray & values, const NumArray::Array & paramValues) {

     
     int nrhs = paramValues.size();
    
     mxArray * prhs = new mxArray[nrhs];
     
     mxArray  plhs[1];

     for(i = 0; i < nrhs; ++i) {
            mwSize zeros[2] = {0, 0 };
	    prhs[i] = mxCreateNumericArray(2, zeros, mxDOUBLE_CLASS, mxREAL);
            mxSetDimensions(prhs[i], paramValues[i].DimPtr()->data(), paramValues[i].NDim());
	    mxSetPr(prhs[i], paramValues[i].ValuesPtr()->data());
     }

     mexCallMATLAB(1, plhs, nrhs, prhs, name_);
   
     for(i = 0; i < values.size(); ++i) {
              values[i] = mxGetPr(plhs[1])[i]
     }

     delete [] prhs;
    }

virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const {


     int nrhs = paramDims.size();
    
     mxArray * prhs = new mxArray[nrhs];
     
     mxArray  plhs[1];

     for(i = 0; i < nrhs; ++i) {
            mwSize zeros[2] = {0, 0 };
	    prhs[i] = mxCreateNumericArray(2, zeros, mxDOUBLE_CLASS, mxREAL);
            mxSetDimensions(prhs[i], paramDims[i]->DimPtr()->data(), paramDims->NDim());
	    mxSetPr(prhs[i], paramDims[i]->ValuesPtr()->Data());
     }

     mexCallMATLAB(1, plhs, nrhs, prhs, fun_dim_);
   
     mwSize size_lhs = mxGetNumberOfElements(plhs[0]);
     double  * lhs = mxGetPr(plhs[0]);
     
     return DimArray(lhs , lhs + size_lhs);
     
    }

}

